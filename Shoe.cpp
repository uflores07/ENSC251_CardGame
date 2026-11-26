/*
 * Implements the Shoe class declared in Shoe.h
 * Builds the decks, shuffles them with the seeded std::mt19937, deals from the top,
 * and reports when fewer than 78 cards are left so Table knows to reshuffle
 */

#include "Shoe.h"
#include <algorithm>   // std::shuffle



// Sets up the random engine and deck count, then builds and shuffles the shoe
// The list follows the order the members are declared in Shoe.h, or -Wall warns
Shoe::Shoe(int decks, unsigned int s) : rng(s), numDecks(decks), seed(s) {
    shuffle();
}

// Destructor
Shoe::~Shoe() {}


// Throws away the remaining cards, rebuilds every deck and shuffles them
// Each deck is every suit (0-3) times every rank (2-14), so 6 decks give 312 cards
void Shoe::shuffle() {
    cards.clear();

    for (int i = 0; i < numDecks; ++i) {             // each deck
        for (int j = 0; j < 4; ++j) {                // each suit
            for (int k = 2; k <= 14; ++k) {          // each rank, 2 to Ace (14)
                cards.emplace_back(static_cast<Card::Rank>(k), static_cast<Card::Suit>(j));
            }
        }
    }

    // std:: is needed here, or this would call Shoe::shuffle() again
    std::shuffle(cards.begin(), cards.end(), rng);
}


// Removes the top card from the shoe and returns it
// The back of the vector is the top of the shoe
Card Shoe::dealCard() {
    Card topCard = cards.back();   // copy the card before removing it
    cards.pop_back();
    return topCard;
}


// Reading the shoe
// Returns how many cards are left to deal
int Shoe::cardsRemaining() const {
    return static_cast<int>(cards.size());
}


// True when fewer than 78 cards are left
// Only reports when to call shuffle()
bool Shoe::needsReshuffle() const {
    return cardsRemaining() < 78;
}
