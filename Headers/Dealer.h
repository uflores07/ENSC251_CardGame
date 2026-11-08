#ifndef DEALER_H
#define DEALER_H

#include "Hand.h"
#include "Card.h"

// Represents the house dealer and manages the dealer's specific rules and hand.
class Dealer {
public:
    Dealer();
    ~Dealer();

    // Adds a dealt card to the dealer's hand
    void addCard(Card c);
    
    // Clears the dealer's hand at the end of a round
    void clearHand();
    
    // Returns the dealer's first card (the only one visible to players initially)
    Card getUpcard() const;
    
    // Checks if the dealer's first two cards equal 21 (used during the peek)
    bool hasBlackjack() const;
    
    // Applies house rules to determine if the dealer must draw another card (Hits until 17, stands on soft 17)
    bool shouldHit() const;
    
    // Returns the dealer's full hand for payout comparisons
    Hand getHand() const;

private:
    Hand dealerHand; // The single hand belonging to the dealer
};

#endif