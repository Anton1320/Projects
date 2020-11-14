#include<iostream>
#include<string>
#include<math.h>
#include<vector>
using namespace std;

struct Card
{
    int num;
    string numstr;
    int suit;
    string suitstr;
};

struct Combination
{
    int combWeight = 0;
    string name;
    vector<int> combCards;
    vector<int> notCombCards;
};

class Deck
{
private:
    Card deck[52];
    int r = 52;

public:
    void init()
    {
        for (int i = 0; i < 52; ++i)
        {
            deck[i].num = i%13;
            if (deck[i].num < 9) deck[i].numstr = to_string(deck[i].num+2);
            else if (deck[i].num == 9) deck[i].numstr = 'J';
            else if (deck[i].num == 10) deck[i].numstr = 'Q';
            else if (deck[i].num == 11) deck[i].numstr = 'K';
            else if (deck[i].num == 12) deck[i].numstr = 'A';

            if (i < 13) { deck[i].suitstr = "Dimonds"; deck[i].suit = 0; }
            else if (i < 26) { deck[i].suitstr = "Clubs"; deck[i].suit = 1; }
            else if (i < 39) { deck[i].suitstr = "Hearts"; deck[i].suit = 2; }
            else if (i < 52) { deck[i].suitstr = "Spides"; deck[i].suit = 3; }
        }
    }

    void shuffle()
    {
        for (int i = 0; i < 52; ++i)
        {
            int w = rand()%(52-i)+i;
            Card q;
            q = deck[w];
            deck[w] = deck[i];
            deck[i] = q;
        }
        r = 52;
    }

    Card extract() { return deck[--r]; }

};

class Table
{
public:
    int bank = 0;
    int maxBet = 0;
    vector<Card> cards;

    void clear()
    {
        bank = 0;
        cards.resize(0);
    }
};

class Player
{
public:
    Card cards[2];
    Combination comb;
    int bet = 0;
    int cash = 200;
    bool isDead = false;
    bool isFolded = false;

    void combInit(Table T)
    {
        vector<Card> allCards;
        for (int i = 0; i < T.cards.size(); ++i) allCards.push_back(T.cards[i]);
        allCards.push_back(cards[0]);
        allCards.push_back(cards[1]);

        comb.combWeight = 0;

        int cardNums[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        vector<int> pair, three;
        int square = -1, row = 0, straight = -2;
        for (int i = 0; i < allCards.size(); ++i) ++cardNums[allCards[i].num];
        for (int i = 0; i < allCards.size(); ++i)
        {
            for (int j = 0; j + 1 < allCards.size(); ++j)
            {
                if (allCards[j].num > allCards[j+1].num)
                {
                    Card q = allCards[j];
                    allCards[j] = allCards[j+1];
                    allCards[j+1] = q;
                }
            }
        }
        if (cardNums[0] > 0 && cardNums[1] > 0 && cardNums[2] > 0 && cardNums[3] > 0 && cardNums[12] > 0)
        {
            straight = 3;
        }
        for (int i = 12; i >= 0; --i)
        {
            if (cardNums[i] >= 1) ++row;
            else if (row < 5) row = 0;
            else { straight = i; row = -1000000; }
 
            if (cardNums[i] == 2) pair.push_back(i);
            else if (cardNums[i] == 3) three.push_back(i);
            else if (cardNums[i] == 4) square = i;
        }

        if (pair.size() == 0)
        {
            comb.combCards.resize(0);
            comb.name = "high card";
            for (int i = allCards.size()-1; i >= 0 && comb.notCombCards.size() < 5; --i)
                comb.notCombCards.push_back(allCards[i].num);
        }
        else if (pair.size() == 1)
        {
            comb.combCards.resize(0);
            comb.combWeight = 1;
            comb.name = "pair";
            comb.combCards.push_back(pair[0]);
            for (int i = allCards.size()-1; i >= 0 && comb.notCombCards.size() < 3; --i)
            {
                if (allCards[i].num != comb.combCards[0])
                    comb.notCombCards.push_back(allCards[i].num);
            }
        }
        else if (pair.size() == 2)
        {
            comb.combCards.resize(0);
            comb.combWeight = 2;
            comb.name = "two pair";
            comb.combCards.push_back(pair[0]);
            comb.combCards.push_back(pair[1]);
            for (int i = allCards.size()-1; i >= 0; --i)
            {
                if (allCards[i].num != comb.combCards[0] && allCards[i].num != comb.combCards[1])
                    comb.notCombCards.push_back(allCards[i].num);
            }
        }

        if (three.size() > 0)
        {
            comb.combCards.resize(0);
            comb.combWeight = 3;
            comb.name = "three of a kind";
            comb.combCards.push_back(three[0]);
            for (int i = allCards.size()-1; i >= 0; --i)
            {
                if (allCards[i].num != comb.combCards[0] && comb.notCombCards.size() < 2)
                    comb.notCombCards.push_back(allCards[i].num);
            }
        }

        if (straight != -2)
        {
            comb.combCards.resize(0);
            comb.combWeight = 4;
            comb.name = "straight";
            for (int i = straight; i >= (straight-5)%13; --i)
            {
                comb.combCards.push_back(i);
            }
        }

        if (three.size() > 0 && pair.size() > 0)
        {
            comb.combWeight = 6;
            comb.name = "full house";
            comb.combCards.resize(0);
            comb.combCards.push_back(three[0]);
            comb.combCards.push_back(pair[0]);
        }

        if (square > 0)
        {
            comb.combWeight = 7;
            comb.name = "four of a kind";
            comb.combCards.resize(0);
            comb.combCards.push_back(square);
            comb.notCombCards.resize(0);
        }

        for (int i = 0; i < allCards.size(); ++i)
        {
            for (int j = 0; j + 1 < allCards.size(); ++j)
            {
                if (allCards[j].suit > allCards[j+1].suit)
                {
                    Card q = allCards[j];
                    allCards[j] = allCards[j+1];
                    allCards[j+1] = q;
                }
            }
        }

        for (int i = 0; i + 4 < allCards.size(); ++i)
        {
            bool q = true;
            for (int j = i; j < i + 4; ++j)
            {
                if (allCards[j].suit != allCards[j+1].suit)
                {
                    q = false;
                    break;
                }
            }
            if (q)
            {
                if (allCards[i].num + 1 == allCards[i+1].num &&
                allCards[i+1].num + 1 == allCards[i+2].num &&
                allCards[i+2].num + 1 == allCards[i+3].num &&
                allCards[i+3].num + 1 == allCards[i+4].num)
                {
                    comb.combWeight = 8;
                    comb.name = "straight flush";
                }
                else
                {
                    comb.combWeight = 5;
                    comb.name = "flush";
                }
                for (int j = i + 4; j >= i; --j)
                {
                    comb.combCards.resize(0);
                    comb.combCards.push_back(allCards[j].num);
                }
            }
        }
    }

    void callOrCheck(Table &T)
    {
        if (bet < T.maxBet) // если колл
        {
            int sum = T.maxBet - bet;
            if (cash >= sum)
            {
                cash -= sum;
                bet += sum;
                T.bank += sum;
            }
            else
            {
                T.bank += cash;
                bet += cash;
                cash = 0;
            }
        }
    }

    void raise(Table &T, int sum)
    {
        sum += T.maxBet - bet;
        if (cash > sum)
        {
            cash -= sum;
            bet += sum;
            T.bank += sum;
        }
        else
        {
            T.bank += cash;
            bet += cash;
            cash = 0;
        }
        if (bet > T.maxBet) T.maxBet = bet;
    }

    void fold() { isFolded = true; }
};

class Computer: public Player
{
private:
    vector<int> layers = {12, 10, 5}; // не знаю
    vector<vector<double>> NN;
    vector<vector<vector<double>>> W;
    int quality = 0;

public:
    void init()
    {
        NN.resize(layers.size());
        for (int i = 0; i < layers.size(); ++i) NN[i].resize(layers[i]);
        W.resize(layers.size() - 1);
        for (unsigned int i = 0; i < W.size(); ++i)
        {
            W[i].resize(layers[i]);
            for (unsigned int j = 0; j < W[i].size(); ++j)
            {
                W[i][j].resize(layers[i + 1]);
                for (unsigned int k = 0; k < W[i][j].size(); ++k) { 
                    W[i][j][k] = (rand() % 100);
                    W[i][j][k] /= 1000;
                }
            }
        }
    }

    int action(vector<long double> X)
    {
        vector<vector<long double>> NN;
        vector<long double> Y;
        NN.push_back(X);
        for (int i = 0; i < W.size(); ++i)
        {
            Y.resize(W[i][0].size());
            for (unsigned int j = 0; j < W[i].size(); ++j)
            {
                for (unsigned int k = 0; k < Y.size(); ++k)
                {
                    Y[k] += X[j] * W[i][j][k];
                }
            }
            X.resize(Y.size());
            for (unsigned int j = 0; j < Y.size(); ++j)
            {
                Y[j] = activation(Y[j]);
                X[j] = Y[j];
            }
            NN.push_back(Y);
        }
        int max = 0;
        for (int i = 0; i < NN[NN.size()-1].size(); ++i)
            if (NN[NN.size()-1][i] > NN[NN.size()-1][max]) max = i;
        return max;
    }
};



int compareComb(Combination a, Combination b) // 0 - a < b; 1 - a > b; 2 - a == b
{
    if (a.combWeight > b.combWeight) return 1;
    else if (a.combWeight == b.combWeight)
    {
        int counter = 0;
        for (int i = 0; i < a.combCards.size(); ++i)
        {
            if (a.combCards[i] > b.combCards[i]) return 1;
            else if (a.combCards[i] < b.combCards[i]) return 0;
        }
        for (int i = 0; i < a.notCombCards.size(); ++i)
        {
            if (a.notCombCards[i] > b.notCombCards[i]) return 1;
            else if (a.notCombCards[i] < b.notCombCards[i]) return 0;
        }
        return 2;
    }
    else return 0;
}

const int playerNum = 6;
const int blindCost = 50;

Deck deck;
Computer players[playerNum];
Table table;


void betsTime()
{
    int currentPlayer = 0;
    bool turn = false;
    int loopCounter = 0;
    int alive = 0;

    for (int i = 0; i < playerNum; ++i)
        if (!players[i].isFolded && !players[i].isDead)
            ++alive;
    
    do
    {
        turn = false;
        
        currentPlayer %= playerNum;
        

        if (!(players[currentPlayer].isDead || players[currentPlayer].isFolded) &&
            players[currentPlayer].cash != 0 && alive > 1)
        {
            cout << currentPlayer+1 << " player's turn\n\n";
            cout << players[currentPlayer].cards[0].numstr << " of " <<
                    players[currentPlayer].cards[0].suitstr << endl;
            cout << players[currentPlayer].cards[1].numstr << " of " <<
                    players[currentPlayer].cards[1].suitstr << "\n\n";
            if (table.cards.size() > 0)
            {
                cout << "Cards on table: \n";
                for (int i = 0; i < table.cards.size(); ++i)
                {
                    cout << table.cards[i].numstr << " of " <<
                            table.cards[i].suitstr << endl;
                }
            }
            cout << "Bank: " << table.bank << '\n';
            cout << "Max bet is " << table.maxBet << "\n";
            cout << "Your bet is " << players[currentPlayer].bet << "\n";
            cout << "Your cash is " << players[currentPlayer].cash << "\n\n";
            cout << "0 - fold\n1 - call or check\n2 - raise to <num>\n";
            int action;
            cin >> action;
            //cout << '\n';
            if (action == 0) { players[currentPlayer].fold(); --alive; }
            else if (action == 1) players[currentPlayer].callOrCheck(table);
            else if (action == 2)
            {
                int num;
                cout << "Your bet is ";
                cin >> num;
                players[currentPlayer].raise(table, num);
            }
            cout << "_________________________________\n\n";
        }
        cout << "";
        for (int i = 0; i < playerNum; ++i)
        {
            if (!players[i].isFolded && !players[i].isDead)
            {
                if (players[i].bet != table.maxBet && players[i].cash != 0) turn = true;
            }
        } 
        ++currentPlayer;
        if (currentPlayer == playerNum) ++loopCounter;
    } while ((turn || loopCounter == 0) && alive > 1);
    for (int i = 0; i < playerNum; ++i) players[i].bet = 0;
    table.maxBet = 0;
}

void reward()
{
    int best = -1;
    vector<int> bests;
    for (int i = 0; i < playerNum; ++i)
    {
        if (!(players[i].isDead || players[i].isFolded))
        {
            players[i].combInit(table);
            best = i;
        }
    }

    for (int i = 0; i < playerNum; ++i)
    {
        if (!(players[i].isDead || players[i].isFolded) && compareComb(players[i].comb, players[best].comb) == 1)
            best = i;
    }

    for (int i = 0; i < playerNum; ++i)
    {
        if (!(players[i].isDead || players[i].isFolded) && compareComb(players[i].comb, players[best].comb) == 2)
        {
            bests.push_back(i);
        }
    }

    for (int i = 0; i < bests.size(); ++i)
    {
        players[bests[i]].cash += table.bank/bests.size();
        cout << "Winner: " << bests[i]+1 << " player\n";
        cout << players[bests[i]].comb.name << '\n';
    }
}

class Learning
{
private:
    int popNum = 100;
    vector<Computer> population;
    vector<Computer> newPop;
public:
    void init()
    {
        population.resize(popNum);
        newPop.resize(popNum);
    }
    void game();
    void selection();
    void mutation();
    void hybridization();
};

int main()
{

    for (int i = 0; i < playerNum; ++i)
    {
        players[i].init();
    }

    deck.init();
    for (int i = 0; i < 10; i++)
    {
        rand();
    }
    
    int alive = playerNum;
    int blind = 0, smallBlind = 1;
    while (alive > 1)
    {
        deck.shuffle();
        cout << "New loop\n\n";
        for (int i = 0; i < playerNum; ++i)
        {
            players[i].bet = 0;
            for (int j = 0; j < 2; ++j)
            {
                players[i].cards[j] = deck.extract();
            }
        }
        
        blind %= playerNum;
        smallBlind %= playerNum;

        if (players[blind].cash >= blindCost)
        {
            players[blind].bet = blindCost;
            players[blind].cash -= blindCost;
            table.bank += blindCost;
            table.maxBet = blindCost;
        }
        else 
        {
            table.bank += players[blind].cash;
            table.maxBet = players[blind].cash;
            players[blind].bet = players[blind].cash;
            players[blind].cash = 0;
        }

        if (players[smallBlind].cash >= blindCost/2)
        {
            players[smallBlind].bet = blindCost/2;
            players[smallBlind].cash -= blindCost/2;
            table.bank += blindCost/2;
        }
        else 
        {
            players[smallBlind].bet = players[smallBlind].cash;
            table.bank += players[blind].cash;
            players[blind].cash = 0;
        }
        if (blindCost/2 > table.maxBet)
                table.maxBet = blindCost/2;

        betsTime();

        deck.extract();
        for (int i = 0; i < 3; ++i)
            table.cards.push_back(deck.extract());
        betsTime();

        deck.extract();
        table.cards.push_back(deck.extract());
        betsTime();

        deck.extract();
        table.cards.push_back(deck.extract());
        betsTime();

        reward();

        for (int i = 0; i < playerNum; ++i)
        {
            players[i].isFolded = false;
            if (!players[i].isDead && players[i].cash == 0)
            {
                players[i].isDead = true;
                --alive;
            }
        }
        ++blind;
        ++smallBlind;
        table.cards.resize(0);
        table.bank = 0;
    }
    return 0;
}