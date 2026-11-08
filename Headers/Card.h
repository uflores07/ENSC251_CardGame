#ifndef CARD_H
#define CARD_H

#include <string>

// Represents a single standard playing card with a suit and a rank.
class Card {
public:
    // Enumeration for the four standard card suits
    enum Suit { HEARTS, DIAMONDS, CLUBS, SPADES };
    
    // Enumeration for card ranks, matching their base integer values where possible
    enum Rank { TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, 
                JACK, QUEEN, KING, ACE };

    // Constructor initializes the card with a specific rank and suit
    Card(Rank r, Suit s);
    
    // Destructor
    ~Card();

    // Returns the integer Blackjack value of the card. 
    // Number cards return 2-10, face cards return 10, and Ace returns 11.
    int getValue() const;
    
    // Converts the card to a concise text representation for the console (e.g., "KH" for King of Hearts, "10S" for Ten of Spades)
    std::string toString() const;

private:
    Rank rank; // The rank of this card (2 through Ace)
    Suit suit; // The suit of this card
};

#endif