#include "Hand.h"


// Initializes an empty hand with an optional starting bet
Hand::Hand(long long initialBet) : betAmount(initialBet), doubled(false), fromSplit(false), finished(false) {}
Hand::~Hand() {}

// Adds a dealt card to this hand
void Hand::addCard(Card c) {
    cards.push_back(c);            //Cards is a vector of a card
}
    
// Sets or updates the main bet riding on this specific hand
void Hand::setBet(long long amount) {
    betAmount = amount;
}

// Doubles the current bet amount (used when a player chooses to double down)
void Hand::doubleBet() {
    betAmount *= 2;
}

// Flags for tracking the state and origins of the hand
void Hand::setDoubled(bool d) {     // Marks the hand as having been doubled down
    doubled = d;
}

void Hand::setFromSplit(bool s) {   // Marks the hand as originating from a split pair
    fromSplit = s;
}

void Hand::setFinished(bool f) {    // Marks the hand as done playing (busted, stood, or 21)
    finished = f;
}

// Accessors for game logic
long long Hand::getBet() const {          // Returns the current bet amount
    return betAmount;
}

int Hand::getTotal() const {        // Calculates the best Blackjack total (handling Aces as 1 or 11)
    int total = 0;
    int aces = 0;
    for (const Card& c : cards) {
        total += c.getValue();
        if (c.getValue() == 11) { // If the card is an Ace
            aces++;
        }
    }
    // Adjust for Aces counted as 11
    while (total > 21 && aces > 0) {
        total -= 10; // Convert Ace from 11 to 1
        aces--;
    }
    return total;
}

bool Hand::isSoft() const {         // Returns true if the hand total includes an Ace counted as 11
    int total = 0;
    int aces = 0;
    for (const Card& c : cards) {
        total += c.getValue();
        if (c.getValue() == 11) // If the card is an Ace
            aces++;
    }
        while (total > 21 && aces > 0) {
            total -= 10; // Convert Ace from 11 to 1
            aces--;
        }
        return aces > 0; // If there's still an Ace counted as 11, it's a soft hand
}

bool Hand::isBlackjack() const {    // Returns true if the hand is exactly 2 cards totaling 21
    return cards.size() == 2 && getTotal() == 21 && !fromSplit;
}

bool Hand::isBust() const {         // Returns true if the hand total exceeds 21
    return getTotal() > 21;
}

bool Hand::canSplit() const {       // Returns true if the hand has exactly 2 cards of identical value
    return cards.size() == 2 && cards[0].getValue() == cards[1].getValue();
}

bool Hand::canDouble() const {      // Returns true if the hand has exactly 2 cards and can be doubled
    return cards.size() == 2;
}

bool Hand::isFinished() const {     // Returns true if no further actions can be taken on this hand
    return finished;
}

// Returns a copy of the cards currently in the hand for display purposes
std::vector<Card> Hand::getCards() const {
    return cards;
}

Card Hand::removeSecondCard() { // for splitting
    Card secondCard = cards[1];
    cards.erase(cards.begin() + 1);
    return secondCard;
}

void Hand::clear() { // to reset the hand at the end of a round
     cards.clear(); 
     betAmount = 0;
     doubled = false;
     fromSplit = false;
     finished = false;
    }  

bool Hand::isDoubled() const { // so other files can read those flags
     return doubled; 
    }

bool Hand::isFromSplit() const { // so other files can read those flags
     return fromSplit; 
    }