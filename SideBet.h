#ifndef SIDEBET_H
#define SIDEBET_H

#include "Card.h"
#include <string>

// Abstract base class for all optional side bets placed before the deal.
class SideBet {
public:
    // Initializes base side bet properties including limits
    SideBet(std::string betName, int minBet, int maxBet);
    virtual ~SideBet();

    // Sets the amount wagered on this specific side bet
    void placeBet(int amount);
    
    // Returns the current wager amount
    int getWager() const;
    
    // Returns the display name of the side bet (e.g., "Perfect Pairs")
    std::string getName() const;

    // Pure virtual function. Derived classes (SideBetA, SideBetB) must implement this to 
    // evaluate the specific win condition and return the calculated payout.
    virtual int calculatePayout(Card playerCard1, Card playerCard2, Card dealerUpcard) = 0;

protected:
    std::string name; // Name of the side bet
    int minimumBet;   // Minimum allowed wager for this side bet
    int maximumBet;   // Maximum allowed wager for this side bet
    int wager;        // The actual amount the player has bet
};

#endif