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
protected:
    Card cards[2];
public:
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

    int checkComb(Card a, Card b)
    {
        int nums[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        int suits[4] = { 0, 0, 0, 0 };
        for (int i =0; i < cardNum; ++i)
        {
            ++nums[cards[i].num];
            ++suits[cards[i].suit];
        }
        ++nums[a.num];
        ++nums[b.num];
        ++suits[a.suit];
        ++suits[b.suit];
        int pairs, threes, flash;
        for (int i = 0; i < 13; ++i)
        {
            if (nums[i] == 2) ++pairs;
            else if (nums[i] == 3) ++threes;
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