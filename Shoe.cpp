#include "Shoe.h"
#include <algorithm>

    // Constructor allows setting the number of decks (default 6) and a seed for reproducible testing
    Shoe::Shoe(int decks, unsigned int s) : numDecks(decks), seed(s), rng(s) {
        shuffle();
        }

    // Destructor
    Shoe::~Shoe() {}

    // Clears current cards, builds the standard decks, and shuffles them into a random order
    void Shoe::shuffle() {
        cards.clear();
        for (int i = 0; i < numDecks; ++i) {
            for (int j = 0; j < 4; ++j) {            // each suit
                for (int k = 2; k <= 14; ++k) {      // each rank, 2 to Ace (14)
                   cards.emplace_back(static_cast<Card::Rank>(k), static_cast<Card::Suit>(j));
                }
            }
        }
        std::shuffle(cards.begin(), cards.end(), rng);
    }
    
    // Removes and returns the top card from the shoe
    Card Shoe::dealCard() {
        Card topCard = cards.back(); // the back of the vector is the top of the shoe
        cards.pop_back();            // Remove the card from the shoe
        return topCard;
    }

    // Returns the exact number of cards currently left in the shoe
    int Shoe::cardsRemaining() const {
        return static_cast<int>(cards.size());
    }

    // Checks if the shoe has reached the penetration limit and needs reshuffling (e.g., fewer than 78 cards)
    bool Shoe::needsReshuffle() const {
        return cardsRemaining() < 78;
    }