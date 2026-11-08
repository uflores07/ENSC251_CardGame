#ifndef TWENTYONEPLUSTHREE_H
#define TWENTYONEPLUSTHREE_H

#include "SideBet.h"

// Implements the "21+3" side bet.
// Wins if the player's first two cards and the dealer's upcard form a 3-card poker hand.
class TwentyOnePlusThree : public SideBet {
public:
    TwentyOnePlusThree(int minBet, int maxBet);
    ~TwentyOnePlusThree() override;

    // Evaluates all three cards to check for a Flush, Straight, Three of a Kind, 
    // Straight Flush, or Suited Triple, and returns the respective payout.
    int calculatePayout(Card playerCard1, Card playerCard2, Card dealerUpcard) override;
};

#endif