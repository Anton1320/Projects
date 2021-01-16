import pygame
import random
pygame.init()
pygame.font.init()

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
CYAN = (127,255,212)

screenSize = (800, 600)
sc = pygame.display.set_mode(screenSize)
pygame.display.set_caption('Game')

clock = pygame.time.Clock()
deltatime = clock.tick()
class Coord:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Item:
    def __init__(self, pos, image):
        self.pos = Coord(pos[0], pos[1])
        self.size = Coord(image.get_size()[0], image.get_size()[1])
        self.image = image

    def draw(self):
        sc.blit(self.image, (self.pos.x, self.pos.y))

    def collision(self, Obj):
        if (-Obj.size.x < Obj.pos.x - self.pos.x < self.size.x) and\
            (-Obj.size.y < Obj.pos.y - self.pos.y < self.size.y):
            return True
        return False

class Apple(Item):
    def __init__(self, pos, image):
        Item.__init__(self, pos, image)
    def Move(self):
        self.pos.x = random.randint(0, screenSize[0])
        self.pos.y = random.randint(0, screenSize[1])

class Movable(Item):
    def __init__(self, pos, image, speed):
        Item.__init__(self, pos, image)
        self.speed = speed
        self.moveRight = False
        self.moveLeft = False
        self.moveUp = False
        self.moveDown = False
    def Move(self):
        if self.moveRight and self.pos.x + self.size.x + self.speed < screenSize[0]:
            self.pos.x += self.speed*deltatime
        if self.moveLeft and self.pos.x - self.speed > 0:
            self.pos.x -= self.speed*deltatime
        if self.moveUp and self.pos.y - self.speed > 0:
            self.pos.y -= self.speed*deltatime
        if self.moveDown and self.pos.y + self.size.y - self.speed < screenSize[1]:
            self.pos.y += self.speed*deltatime
    
class Bullet(Movable):
    def __init__(self, pos, image, speed):
        Movable.__init__(self, pos, image, speed)
        self.startPos = Coord(pos[0], pos[1])
        self.range = 400

    def remove(self):
        self.pos.x = -self.size.x
        self.pos.y = -self.size.y
        self.moveRight = self.moveLeft = self.moveUp = self.moveDown = False

class Player(Movable):
    def __init__(self, pos:tuple, speed, image:pygame.Surface):
        Movable.__init__(self, pos, image, speed)
        self.hp = 10
        self.score = 0
    
    def shoot(self, bullet:Bullet):
        bullet.pos.x = self.pos.x+self.size.x//2
        bullet.pos.y = self.pos.y+self.size.y//2
        bullet.startPos = Coord(self.pos.x, self.pos.y)
        bullet.moveDown = self.moveDown
        bullet.moveUp = self.moveUp
        bullet.moveLeft = self.moveLeft
        bullet.moveRight = self.moveRight

    def pickUp(self, apple:Apple):
        self.score += 1
        apple.Move()

class Enemy(Movable):
    def __init__(self, pos:tuple, speed, image:pygame.Surface):
        Movable.__init__(self, pos, image, speed)
        self.liveTimer = 0

    def move(self, target):
        self.liveTimer += deltatime
        self.pos.x += 2*((target.pos.x > self.pos.x)-0.5) * (target.pos.x != self.pos.x)*self.speed*deltatime
        self.pos.y += 2*((target.pos.y > self.pos.y)-0.5) * (target.pos.y != self.pos.y)*self.speed*deltatime

player = Player((50, 50), 0.5, pygame.Surface((20, 20)))
player.image.fill(GREEN)

apple = Apple((100, 100), pygame.Surface((20, 20)))
apple.image.fill(RED)

font1 = pygame.font.Font(None, 30)
score = font1.render('0', True, BLACK)
hp = font1.render('10', True, RED)

bullet = Bullet((-20, -20), pygame.Surface((20, 20)), 1)
bullet.image.fill(BLACK)

enemySurface = pygame.Surface((20, 20))
enemySurface.fill(BLUE)
enemys = [Enemy((800, 600), 0.25, enemySurface)]

def spawnEnemy():
    r = random.randint(1, 4)
    pos = [0, 0]
    if r == 1:
        pos[0] = -20
        pos[1] = random.randint(0, screenSize[1])
    elif r == 2:
        pos[1] = screenSize[1]+1
        pos[0] = random.randint(0, screenSize[0])
    elif r == 3:
        pos[1] = -20
        pos[0] = random.randint(0, screenSize[0])
    elif r == 4:
        pos[0] = screenSize[0]+1
        pos[1] = random.randint(0, screenSize[1])
    es = pygame.Surface((20, 20))
    es.fill(BLUE)
    enemys.append(Enemy((pos[0], pos[1]), 0.25, es))
timer = 0

running = True
while running:
    deltatime = clock.tick()
    timer += deltatime
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_RIGHT:
                player.moveRight = True
            if event.key == pygame.K_LEFT:
                player.moveLeft = True
            if event.key == pygame.K_UP:
                player.moveUp = True
            if event.key == pygame.K_DOWN:
                player.moveDown = True
            if event.key == pygame.K_LCTRL:
                player.shoot(bullet)
        if event.type == pygame.KEYUP:
            if event.key == pygame.K_RIGHT:
                player.moveRight = False
            if event.key == pygame.K_LEFT:
                player.moveLeft = False
            if event.key == pygame.K_UP:
                player.moveUp = False
            if event.key == pygame.K_DOWN:
                player.moveDown = False
    if player.collision(apple):
        player.pickUp(apple)
        spawnEnemy()
        player.hp += 1
        score = font1.render(str(player.score), True, BLACK)
        hp = font1.render(str(player.hp), True, RED)
    if abs(bullet.startPos.x - bullet.pos.x) > bullet.range or\
        abs(bullet.startPos.y - bullet.pos.y) > bullet.range:
        bullet.remove()
    
    sc.fill(CYAN)
    for enemy in enemys:
        if enemy.collision(player):
            player.hp -= 5
            hp = font1.render(str(player.hp), True, RED)
            enemys.remove(enemy)
            if player.hp <= 0: running = False
        if enemy.collision(bullet):
            enemys.remove(enemy)
            player.score += 2
            score = font1.render(str(player.score), True, BLACK)
            bullet.remove()
        if enemy.liveTimer > 10000:
            enemys.remove(enemy)

        enemy.move(player)
        enemy.draw()
    bullet.Move()
    bullet.draw()
    player.Move()
    player.draw()
    apple.draw()
    sc.blit(score, (10, 10))
    sc.blit(hp, (770, 10))
    pygame.display.update()