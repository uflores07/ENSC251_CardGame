#ifndef SHOE_H
#define SHOE_H

#include "Card.h"
#include <vector>
#include <random>

// Manages the multi-deck card supply used for dealing.
class Shoe {
public:
    // Constructor allows setting the number of decks (default 6) and a seed for reproducible testing
    Shoe(int numDecks = 6, unsigned int seed = 0);
    
    // Destructor
    ~Shoe();

    // Clears current cards, builds the standard decks, and shuffles them into a random order
    void shuffle();
    
    // Removes and returns the top card from the shoe
    Card dealCard();
    
    // Returns the exact number of cards currently left in the shoe
    int cardsRemaining() const;
    
    // Checks if the shoe has reached the penetration limit and needs reshuffling (e.g., fewer than 78 cards)
    bool needsReshuffle() const;

private:
    std::vector<Card> cards; // The internal container holding the undealt cards
    int currentCardIndex;    // Tracks the position of the next card to be dealt
    int totalDecks;          // The total number of standard 52-card decks in this shoe
    std::mt19937 rng;        // Standard Mersenne Twister engine for randomizing card order
};

#endif