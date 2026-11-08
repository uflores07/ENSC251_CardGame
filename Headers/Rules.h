#ifndef RULES_H
#define RULES_H

#include "Hand.h"

// A stateless utility class that enforces the mathematical and casino rules of the game.
class Rules {
public:
    // Global static constants for table configuration
    static const int MIN_BET = 10;
    static const int MAX_BET = 500;
    static const int MAX_HANDS_PER_SEAT = 4; // Maximum allowed hands after splitting
    static const int NUM_DECKS = 6;
    
    // Checks if taking a hit is mathematically and legally allowed for the hand
    static bool isLegalHit(const Hand& h);
    
    // Checks if standing is allowed
    static bool isLegalStand(const Hand& h);
    
    // Checks if doubling down is allowed (usually only on the first two cards)
    static bool isLegalDouble(const Hand& h);
    
    // Checks if splitting is allowed, factoring in card values and the seat's current hand count
    static bool isLegalSplit(const Hand& h, int currentHandsAtSeat);
    
    // Checks if an insurance bet is legal based on the hand's state
    static bool isLegalInsurance(const Hand& h);
};

#endif