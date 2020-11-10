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
    int cash = 1000;
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

    void callCheck(Table &T)
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
                bet += cash
                cash = 0;
            }
        }
    }

    void raise(Table &T, int sum)
    {
        if (cash > sum)
        {
            cash -= sum;
            bet += sum;
            T.bank += sum;
        }
        else
        {
            T.bank += cash;
            bet += cash
            cash = 0;
        }
        if (bet > T.maxBet) T.maxBet = bet;
    }

    void fold() { isFolded = true; }
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

Deck deck;
Player player[5];
//Computer bot;
Table table;

int main()
{
    //bot.init();
    deck.init();
    for (int k = 0; k < 1000000; ++k)
    {
        deck.shuffle();
        player.cards[0] = deck.extract();
        player.cards[1] = deck.extract();
        for (int i = 0; i < 5; ++i)
        {
            table.cards.push_back(deck.extract());
        }
        player.combInit(table);
        if (player.comb.combWeight == 8)
        {
            cout << "--> " << player.cards[0].numstr << " of " << player.cards[0].suitstr << endl;
            cout << "--> " << player.cards[1].numstr << " of " << player.cards[1].suitstr << endl;
            for (int i = 0; i < table.cards.size(); ++i)
            {
                cout << table.cards[i].numstr << " of " << table.cards[i].suitstr << endl;
            }
            cout << player.comb.name << endl;
            cout << endl;
        }
        table.clear();
    }
    return 0;
}