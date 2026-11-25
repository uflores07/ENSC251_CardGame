#include "Card.h"

using namespace std;
    // Constructor initializes the card with a specific rank and suit
    Card::Card(Rank r, Suit s) : rank(r), suit(s) {};
    
    // Destructor: nothing to clean up
    Card::~Card() {}

    // Returns the integer Blackjack value of the card. 
    // Number cards return 2-10, face cards return 10, and Ace returns 11.
    int Card::getValue() const {
        if (rank == ACE)
            return 11;
        if (rank >= JACK)
            return 10;
        return rank;
    }
    
    // Converts the card to a concise text representation for the console (e.g., "KH" for King of Hearts, "10S" for Ten of Spades)
    std::string Card::toString() const {
        std::string text;

        switch (rank) {
            case ACE : text = "A"; break;
            case KING : text = "K"; break;
            case QUEEN : text = "Q"; break;
            case JACK : text = "J"; break;
            default: text = std::to_string(rank); break;
        }
        switch (suit) {
            case HEARTS : text = "H"; break;
            case DIAMONDS : text = "D"; break;
            case CLUBS : text = "C"; break;
            case SPADES : text = "S"; break;
        }
        return text;
    }