#include<vector>
#include<iostream>
#include<Windows.h>
using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

const int width = 80; //standart - 80 25 
const int height = 25;
int field[height][width];
int step = 0;

const int startPopulationNum = 550;
const int startFoodNum = 100;
const int speed = 10;

const int maxAge = 100;
const int virusDamage = 0;
const int foodSpawnSpeed = 20;
const int mutationSpeed = 1;
const int startEnergy = 100;
const int divisionCost = 30;
const int foodCost = 50;
const int herbivorousDamage = 1;
int burned = 0;
int died = 0;

struct Coordinates
{
	int x;
	int y;
};

Coordinates qwerty(int x, int y) {
	Coordinates out;
	if (x >= width) 
		x -= width;
	else if (x < 0) 
		x += width;
	if (y >= height)
		y -= height;
	else if (y < 0)
		y += height;
	out.x = x;
	out.y = y;
	return out;
}

struct creature {
	int genes[64] = { 0, 0, 0, 0, 0, 0, 0, 0,   //genes  0 - 7: virus
					  0, 0, 0, 0, 0, 0, 0, 0,	//8 - 15: !step
					  0, 0, 0, 0, 0, 0, 0, 0,	//16 - 23 !attack
					  0, 0, 0, 0, 0, 0, 0, 0,	//24 - 31 !eat
					  0, 0, 0, 0, 0, 0, 0, 0,	//32 - 63 if
					  0, 0, 0, 0, 0, 0, 0, 0,	
					  0, 0, 0, 0, 0, 0, 0, 0,    //             1 0 7
					  0, 0, 0, 0, 0, 0, 0, 0 };  //rotation -   2 @ 6
												 //             3 4 5
	int type = 2; // 2 - травоядное 3 - плотоядное
	int divisionGen = 0;
	int x = rand() % width;
	int y = rand() % height;
	int rotation = 0;
	int currentGen = 0;
	int energy = startEnergy;
	int age = 0;
};

creature population[100000];
vector<vector<bool>> food;
int popSize = startPopulationNum;

int creatureSearch(int x, int y) {
	int out = -1;
	for (int i = 0; i < popSize; ++i){
		if (population[i].x == x && population[i].y == y) {
			out = i;
			break;
		}
	}
	return out;
}

void dividion(int creatureId) {
	Coordinates xy;
	int mutationNum = 0;
	switch (population[creatureId].divisionGen)
	{
	case 0:
		xy = qwerty(population[creatureId].x, population[creatureId].y - 1);
	case 1:
		xy = qwerty(population[creatureId].x - 1, population[creatureId].y - 1);
	case 2:
		xy = qwerty(population[creatureId].x - 1, population[creatureId].y);
	case 3:
		xy = qwerty(population[creatureId].x - 1, population[creatureId].y + 1);
	case 4:
		xy = qwerty(population[creatureId].x, population[creatureId].y + 1);
	case 5:
		xy = qwerty(population[creatureId].x + 1, population[creatureId].y + 1);
	case 6:
		xy = qwerty(population[creatureId].x + 1, population[creatureId].y);
	case 7:
		xy = qwerty(population[creatureId].x + 1, population[creatureId].y - 1);
	}
	int creatureId2 = -1;
	for (int i = 0; i < popSize; ++i) {
		if (i != creatureId && population[i].type == population[creatureId].type) { creatureId2 = i; break; }
	}
	if (creatureId2 != -1) {
		population[popSize] = population[creatureId];
		population[popSize].x = xy.x;
		population[popSize].y = xy.y;

		if (creatureSearch(xy.x, xy.y) == -1) {
			for (int i = 31; i < 64; ++i) { population[popSize].genes[i] = population[creatureId2].genes[i]; }
			mutationNum = rand() % mutationSpeed;
			population[creatureId].energy /= 2;
			population[popSize].energy /= 2;
			population[popSize].age = 0;
			population[popSize].divisionGen = rand() % 8;
			for (int i = 0; i < mutationNum; ++i) { population[popSize].genes[rand() % 64] = rand() % 32; }
			population[popSize].currentGen = 0;
			++popSize;
			++burned;
		}
	}
}

bool foodSearch(int x, int y) { return food[y][x]; }

void attack(int creatureId1, int creatureId2) {
	if (population[creatureId1].type == 3 && population[creatureId2].type == 2) {
		population[creatureId1].energy += population[creatureId2].energy;
		population[creatureId2].energy -= 100000;
	}
	else if (population[creatureId2].type == 3) {
		population[creatureId2].energy -= herbivorousDamage;
	}
}

void eat(int creationId, int foodX, int foodY) {
	if (population[creationId].type == 2) {
		population[creationId].energy += foodCost;
		food[foodY][foodX] = false;
		population[creationId].type = 2;
	}
}

void creatureAction() {
	for (int i = 0; i < popSize; ++i) {
		if (population[i].energy > 0 && population[i].age <= maxAge) {
			++population[i].age;
			int gene = population[i].genes[population[i].currentGen];
			if (population[i].energy >= divisionCost) { dividion(i); }
			if (gene <= 7) {
				population[i].energy -= virusDamage;
				++population[i].currentGen;
				population[i].currentGen %= 64;
			}
			if (gene >= 32 && gene <= 63) {
				population[i].currentGen += gene;
				population[i].currentGen %= 64;
			}
			if (gene >= 8 && gene <= 15) {
				switch (gene)
				{
				case 8:
					if (creatureSearch(population[i].x, population[i].y-1) == -1)
						--population[i].y;
					break;
				case 9:
					if (creatureSearch(population[i].x - 1, population[i].y - 1) == -1) {
						--population[i].x;
						--population[i].y;
					}
					break;
				case 10:
					if (creatureSearch(population[i].x - 1, population[i].y) == -1)
						--population[i].x;
					break;
				case 11:
					if (creatureSearch(population[i].x - 1, population[i].y + 1) == -1) {
						--population[i].x;
						++population[i].y;
					}
					break;
				case 12:
					if (creatureSearch(population[i].x, population[i].y + 1) == -1)
						++population[i].y;
					break;
				case 13:
					if (creatureSearch(population[i].x + 1, population[i].y + 1) == -1) {
						++population[i].x;
						++population[i].y;
					}
					break;
				case 14:
					if (creatureSearch(population[i].x + 1, population[i].y) == -1)
						++population[i].x;
					break;
				case 15:
					if (creatureSearch(population[i].x + 1, population[i].y - 1) == -1) {
						++population[i].x;
						--population[i].y;
					}
					break;
				}
				Coordinates a;
				a = qwerty(population[i].x, population[i].y);
				population[i].x = a.x;
				population[i].y = a.y;
				++population[i].currentGen;
				population[i].currentGen %= 64;
				--population[i].energy;
			}
			else if (gene >= 16 && gene <= 23) {
				int a = -1;
				Coordinates action;
				action = qwerty(population[i].x, population[i].y - 1);
				a = creatureSearch(action.x, action.y);
				if (a != -1) {
					attack(i, a);
				}
				else {
					action = qwerty(population[i].x - 1, population[i].y - 1);
					a = creatureSearch(action.x, action.y);
					if (a != -1)
						attack(i, a);
					else {
						action = qwerty(population[i].x - 1, population[i].y + 1);
						a = creatureSearch(action.x, action.y);
						if (a != -1)
							attack(i, a);
						else {
							action = qwerty(population[i].x, population[i].y + 1);
							a = creatureSearch(action.x, action.y);
							if (a != -1)
								attack(i, a);
							else {
								action = qwerty(population[i].x + 1, population[i].y + 1);
								a = creatureSearch(action.x, action.y);
								if (a != -1)
									attack(i, a);
								else {
									action = qwerty(population[i].x + 1, population[i].y);
									a = creatureSearch(action.x, action.y);
									if (a != -1)
										attack(i, a);
									else {
										action = qwerty(population[i].x + 1, population[i].y - 1);
										a = creatureSearch(action.x, action.y);
										if (a != -1)
											attack(i, a);
									}
								}
							}
						}
					}
				}
				++population[i].currentGen;
				population[i].currentGen %= 64;
				--population[i].energy;
			}
			else if (gene >= 24 && gene <= 31) {
				int a[2] = { -1, -1 };
				Coordinates action;
				switch (gene)
				{
				case 24:
					action = qwerty(population[i].x, population[i].y - 1);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x;
						a[1] = population[i].y - 1;
					}
					break;
				case 25:
					action = qwerty(population[i].x - 1, population[i].y - 1);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x - 1;
						a[1] = population[i].y - 1;
					}
					break;
				case 26:
					action = qwerty(population[i].x - 1, population[i].y);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x - 1;
						a[1] = population[i].y;
					}
					break;
				case 27:
					action = qwerty(population[i].x - 1, population[i].y + 1);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x - 1;
						a[1] = population[i].y + 1;
					}
					break;
				case 28:
					action = qwerty(population[i].x, population[i].y + 1);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x;
						a[1] = population[i].y + 1;
					}
					break;
				case 29:
					action = qwerty(population[i].x + 1, population[i].y + 1);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x + 1;
						a[1] = population[i].y + 1;
					}
					break;
				case 30:
					action = qwerty(population[i].x + 1, population[i].y);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x + 1;
						a[1] = population[i].y;
					}
					break;
				case 31:
					action = qwerty(population[i].x + 1, population[i].y - 1);
					if (foodSearch(action.x, action.y)) {
						a[0] = population[i].x + 1;
						a[1] = population[i].y - 1;
					}
					break;
				}
				if (a[0] != -1 && a[1] != -1)
					eat(i, action.x, action.y);
				++population[i].currentGen;
				population[i].currentGen %= 64;
				--population[i].energy;
			}
			field[population[i].y][population[i].x] = population[i].type;
		}
	}
}

void beginSimulation() {
	for (int i = 0; i < startPopulationNum; ++i) {
		for (int j = 0; j < 64; ++j) {
			population[i].genes[j] = rand() % 32;
		}
		population[i].x = rand() % width;
		population[i].y = rand() % height;
		if (rand() % 2 == 0)
			population[i].type = 3;
		else
			population[i].type = 2;
	}
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			food[i][j] = 0;
		}
	}
	for (int i = 0; i < startFoodNum; ++i) { food[rand() % height][rand() % width] = true; }
}

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}

void hidecursor()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void draw() {
	gotoxy(0, 0);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (field[i][j] == 2) {
				SetConsoleTextAttribute(hConsole, 10);
				cout << "X";
			}
			else if (field[i][j] == 3) {
				SetConsoleTextAttribute(hConsole, 12);
				cout << "X";
			}
			else if (foodSearch(j, i)) {
				SetConsoleTextAttribute(hConsole, 2);
				cout << "O";
			}
			else if (field[i][j] == 0) {
				cout << " ";
			}
		}
		SetConsoleTextAttribute(hConsole, 15);
		cout << "| ";
		switch (i)
		{
		case 0:
			cout << "Max age: " << maxAge;
			break;
		case 1:
			cout << "Virus damage: " << virusDamage;
			break;
		case 2:
			cout << "Food spawn speed: " << foodSpawnSpeed;
			break;
		case 3:
			cout << "Mutation speed: " << mutationSpeed;
			break;
		case 4:
			cout << "Division cost: " << divisionCost;
			break;
		case 5:
			cout << "Start Energy: " << startEnergy;
			break;
		case 7:
			cout << "died: " << died;
			break;
		case 8:
			cout << "burned: " << burned;
			break;
		case 10:
			cout << "Creatures: " << popSize << "       ";
			break;
		case 11:
			cout << "Step: " << step;
			break;
		}
		cout << endl;
	}
	SetConsoleTextAttribute(hConsole, 15);
	for (int i = 0; i <= width; ++i) { cout << '-'; }
	cout << endl;
}

void Simulation() {
	while (true) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) { field[i][j] = 0; }
		}
		for (int i = 0; i < foodSpawnSpeed; ++i){ 
			int a = rand() % height;
			int b = rand() % width;
			if (creatureSearch(b, a) == -1)
				food[a][b] = true;
		}
		creatureAction();
		for (int i = 0; i < popSize; ++i) {
			if (population[i].energy <= 0 || population[i].age > maxAge) {
				for (int j = i; j < popSize - 1; ++j) { population[j] = population[j + 1]; }
				--popSize;
				++died;
			}
			else{ field[population[i].y][population[i].x] == population[i].type; }
		}
		++step;
		if (step % speed == 0)
			draw();
	}
}

int main() {
	ios::sync_with_stdio(false);
	cout.tie(NULL);
	SetConsoleTextAttribute(hConsole, 15);
	hidecursor();
	food.resize(height);
	for (int i = 0; i < height; ++i) { food[i].resize(width); }
	beginSimulation();
	Simulation();
	return 0;
}