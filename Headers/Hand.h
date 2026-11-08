#ifndef HAND_H
#define HAND_H

#include "Card.h"
#include <vector>

// Represents a single playable hand of cards and the bet associated with it.
class Hand {
public:
    // Initializes an empty hand with an optional starting bet
    Hand(int initialBet = 0);
    ~Hand();

    // Adds a dealt card to this hand
    void addCard(Card c);
    
    // Sets or updates the main bet riding on this specific hand
    void setBet(int amount);
    
    // Doubles the current bet amount (used when a player chooses to double down)
    void doubleBet();
    
    // Flags for tracking the state and origins of the hand
    void setDoubled(bool d);     // Marks the hand as having been doubled down
    void setFromSplit(bool s);   // Marks the hand as originating from a split pair
    void setFinished(bool f);    // Marks the hand as done playing (busted, stood, or 21)

    // Accessors for game logic
    int getBet() const;          // Returns the current bet amount
    int getTotal() const;        // Calculates the best Blackjack total (handling Aces as 1 or 11)
    bool isSoft() const;         // Returns true if the hand total includes an Ace counted as 11
    bool isBlackjack() const;    // Returns true if the hand is exactly 2 cards totaling 21
    bool isBust() const;         // Returns true if the hand total exceeds 21
    bool canSplit() const;       // Returns true if the hand has exactly 2 cards of identical value
    bool canDouble() const;      // Returns true if the hand has exactly 2 cards and can be doubled
    bool isFinished() const;     // Returns true if no further actions can be taken on this hand

    // Returns a copy of the cards currently in the hand for display purposes
    std::vector<Card> getCards() const;

private:
    std::vector<Card> cards; // The cards currently held in this hand
    int betAmount;           // The money wagered on this specific hand
    bool doubled;            // True if the player doubled down
    bool fromSplit;          // True if this hand was created by splitting a pair
    bool finished;           // True if the player has stood, busted, or reached 21
};

#endif