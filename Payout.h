#ifndef PAYOUT_H
#define PAYOUT_H

#include "Hand.h"

// A stateless utility class responsible for comparing hands and calculating monetary winnings.
class Payout {
public:
    // Enumeration of all possible outcomes for a player's hand against the dealer
    enum Result { BLACKJACK, WIN, PUSH, LOSS };

    // Evaluates both totals and returns the win/loss/tie status of the player's hand
    static Result determineResult(const Hand& playerHand, const Hand& dealerHand);
    
    // Applies casino payout ratios to determine the return amount. 
    // (e.g., standard win pays 1:1, blackjack pays 3:2)
    static int calculateWinnings(Result r, int betAmount);
    
    // Calculates the return on a winning insurance bet (always pays 2:1)
    static int calculateInsurancePayout(int insuranceBetAmount);
};

#endif