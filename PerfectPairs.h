#ifndef PERFECTPAIRS_H
#define PERFECTPAIRS_H

#include "SideBet.h"

// Implements the "Perfect Pairs" side bet.
// Wins if the player's first two cards form a pair (Mixed Pair, Color Pair, or Perfect Pair).
class PerfectPairs : public SideBet {
public:
    PerfectPairs(int minBet, int maxBet);
    ~PerfectPairs() override;

    // Evaluates the player's first two cards. The dealerUpcard is ignored for this bet.
    // Returns the calculated payout (e.g., 5:1, 12:1, or 25:1) based on the pair type.
    int calculatePayout(Card playerCard1, Card playerCard2, Card dealerUpcard) override;
};

#endif