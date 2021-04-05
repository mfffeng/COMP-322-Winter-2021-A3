#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;



class Card{
    public:
        enum ranks{
            ACE = 1,
            TWO,
            THREE,
            FOUR,
            FIVE,
            SIX,
            SEVEN,
            EIGHT,
            NINE,
            TEN,
            JACK = 'J',
            QUEEN = 'Q',
            KING = 'K'
        };
        enum types{
            CLUBS = 'C',
            DIAMONDS = 'D',
            HEARTS = 'H',
            SPADES = 'S'
        };
        Card(ranks r, types t){
            this -> rank = r;
            this -> type = t;
        }
        int getValue() const{       // chars have int value larger than 10, so return 10 when encountering chars.
            return (rank <= 10)? rank: 10;
        }
        void displayCard(){     // Tried to use "(rank > 10)? char(rank): rank" here, but it didn't work for some unknown reason.
            if (rank > 10) cout << char(rank) << char(type) << " ";
            else cout << rank << char(type) << " ";
        }
    private:
        ranks rank;
        types type;
};



class Hand{
    private:
        vector<Card> handCards;
    public:
        void add(Card c){
            handCards.push_back(c);
        }
        void clear(){
            handCards.clear();
        }
        int getTotal() const{
            int total = 0;
            for (auto ptr = handCards.begin(); ptr != handCards.end(); ptr++){
                total += (*ptr).getValue();
                if ((*ptr).getValue() == 1 && total <= 11){     // If the current card is an Ace and there's space for a 11-point ace,
                    total += 10;                                // count this Ace as 11 points.
                }
            }
            return total;
        }
        vector<Card> getHandCards() const{
            return handCards;
        }
};



class Deck{
    private:
        vector<Card> deck;
    public:
        void populate(){
            deck.clear();
            Card::ranks values[] = {Card::ACE, Card::TWO, Card::THREE, Card::FOUR, Card::FIVE, Card::SIX, Card::SEVEN, Card::EIGHT, Card::NINE, Card::TEN, Card::JACK, Card::QUEEN, Card::KING};
            Card::types colors[] = {Card::CLUBS, Card::DIAMONDS, Card::HEARTS, Card::SPADES};
            for (int i = 0; i < 4; i++){
                for (int j = 0; j < 13; j++){
                    Card num(values[j], colors[i]);
                    deck.push_back(num);
                }
            }
        }
        void Shuffle(){
            // Some weird stuff for seeding the random engine with the current time in milliseconds to ensure a different shuffle scheme
            // every time Shuffle() is called.
            // Also I have to change the first letter to upper case, or it will collide with the shuffle() function I used below.
            mt19937 e{static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count())};
            shuffle(deck.begin(), deck.end(), e);
        }
        void deal(Hand& dealTo){    // C++ is pass by value by default!!!
            Card dealt = deck.at(0);
            dealTo.add(dealt);
            deck.erase(deck.begin());
        }
};



class AbstractPlayer: public Hand{
    public:
        virtual bool isDrawing() const = 0;
        bool isBusted(){
            return getTotal() > 21;
        }
        // Overload << to display a full line of needed information at once.
        friend ostream& operator<<(ostream& os, const AbstractPlayer& player){
            for (int i = 0; i < player.getHandCards().size(); i++){
                player.getHandCards()[i].displayCard();
            }
            cout << "[" << player.getTotal() << "]" << endl;
            return os;      // Is this line needed?
        }
};



class HumanPlayer: public AbstractPlayer{
    public:
        bool isDrawing() const{
            return getTotal() <= 21;
        }
        void announce(Hand casino){
            if (getTotal() > 21){
                cout << "Player busts.\nCasino wins." << endl;
            }
            else if (casino.getTotal() > 21){
                cout << "Casino busts.\nPlayer wins." << endl;
            }
            else if (casino.getTotal() > getTotal()){
                cout << "Casino wins." << endl;
            }
            else if (casino.getTotal() < getTotal()){
                cout << "Player wins." << endl;
            }
            else{
                cout << "Push: No one wins." << endl;
            }
        }
};



class ComputerPlayer: public AbstractPlayer{
    public:
        bool isDrawing() const{
            return getTotal() <= 16;
        }
};



class BlackJackGame{
    private:
        Deck m_deck;
        ComputerPlayer m_casino;

    public:
        BlackJackGame(){
            m_deck.populate();
            m_deck.Shuffle();
        }
        void play(){
            HumanPlayer h;
            m_deck.deal(h);
            m_deck.deal(h);
            m_deck.deal(m_casino);
            cout << "Player: " << h << endl;
            cout << "Casino: " << m_casino << endl;
            bool continueDraw = false;
            char ans;
            cout << "Do you want to draw? (y/n): ";
            cin >> ans;
            continueDraw = (ans == 'y')? true: false;
            while (h.isDrawing() && continueDraw && h.getTotal() != 21){
                m_deck.deal(h);
                cout << "Player: " << h << endl;
                if (h.isBusted()){          // If player busts, immediately terminate the current game.
                    h.announce(m_casino);
                    h.clear();
                    m_casino.clear();       // These three lines are used to empty both sides's hands and reset the deck.
                    m_deck.populate();
                    m_deck.Shuffle();
                    return;
                }
                if (h.getTotal() == 21){    // If the player's hand is exactly 21, stop offering to draw again and let computer start drawing.
                    break;
                }
                cout << "Do you want to draw? (y/n): ";
                cin >> ans;
                continueDraw = (ans == 'y')? true: false;
            }
            while (m_casino.isDrawing()){
                m_deck.deal(m_casino);
                cout << "Casino: " << m_casino << endl;
                if (m_casino.isBusted()){   // If casino busts, immediately terminate the current game.
                    h.announce(m_casino);
                    h.clear();
                    m_casino.clear();
                    m_deck.populate();
                    m_deck.Shuffle();
                    return;
                }
            }
            h.announce(m_casino);
            m_casino.clear();
            m_deck.populate();
            m_deck.Shuffle();
        }
};



int main(){
    cout << "\tWelcome to the Comp322 Blackjack game!" << endl << endl;
    BlackJackGame game;
    // The main loop of the game
    bool playAgain = true;
    char answer = 'y';
    while (playAgain){
        game.play();
        // Check whether the player would like to play another round
        cout << "Would you like another round? (y/n): ";
        cin >> answer;
        cout << endl << endl;
        playAgain = (answer == 'y' ? true : false);
    }
    cout <<"Gave over!";
    return 0;
}



