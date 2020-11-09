#include<iostream>
#include<string>
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
    int combCardsR = 0;
    Card combCards[5];
    vector<Card> notCombCards;
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
            else if (i < 52) { deck[i].suitstr = "Spides"; deck[i].suit = 2; }
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
    }

    Card extract() { return deck[--r]; }

};

class Player
{
public:
    Card cards[2];
    Combination comb;
    int cash;
};

class Computer: public Player
{
private:
    const vector<int> layers = {9, 10, 5}; // не знаю
    vector<vector<double>> NN;
    vector<vector<vector<double>>> W;
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

    void action();
};

class Table
{
public:
    int bank = 0;
    int cardNum = 0;
    Card cards[5];

    int сomb(Player P)
    {
        vector<Card> allCards;
        for (int i = 0; i < cardNum; ++i) allCards.push_back(cards[i]);
        allCards.push_back(P[0].cards[0]);
        allCards.push_back(P[0].cards[1]);

        int cardNums[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        vector<int> pair, three;
        int square = -1, row, street;
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
            street = 3;
        }
        for (int i = allCards.size()-1; i >= 0; --i)
        {
            if (cardNums[i] >= 1) ++row;
            else if (row < 5) row = 0;
            else { street = i; row = -1000000; }

            if (cardNums[i] == 2) pair.push_back(i);
            else if (cardNums[i] == 3) three.push_back(i);
            else if (cardNums[i] == 4) square = i;
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
    }
};

Deck deck;
Computer comp;
Table table;

int main()
{
    deck.init();
    deck.shuffle();
    comp.init();
    return 0;
}