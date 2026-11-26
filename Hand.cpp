/*
 * Implements the Hand class declared in Hand.h: adding cards, tracking
 * the bet and flags, and working out the Blackjack total, soft hands,
 * blackjack, busts, splits and doubles.
 *
 * Money is always in cents as long long
 */
 
#include "Hand.h"
 

// Creates an empty hand with the given starting bet. All flags start false
Hand::Hand(long long initialBet)
    : betAmount(initialBet), doubled(false), fromSplit(false), finished(false) {}
 

    // Destructor: nothing to clean up, the vector frees its own memory
Hand::~Hand() {}
 

// Adds a dealt card to the end of the hand
void Hand::addCard(Card c) {
    cards.push_back(c);    // cards is a vector of Card
}


// Sets or replaces the bet on this hand (in cents)
void Hand::setBet(long long amount) {
    betAmount = amount;
}


// Doubles the bet when the player doubles down
void Hand::doubleBet() {
    betAmount *= 2;
}
 

// Removes the second card and returns it, leaving one card in this hand.
// Used for splitting. The caller must check canSplit() first, because
// this reads cards[1] and would fail on a hand with fewer than two cards.
Card Hand::removeSecondCard() {
    Card secondCard = cards[1];         // copy it before removing it
    cards.erase(cards.begin() + 1);
    return secondCard;
}


// Empties the hand and resets the bet and every flag for the next round
void Hand::clear() {
    cards.clear();
    betAmount = 0;
    doubled = false;
    fromSplit = false;
    finished = false;
}


// Marks the hand as doubled down
void Hand::setDoubled(bool d) {
    doubled = d;
}
 

// Marks the hand as created by splitting a pair
void Hand::setFromSplit(bool s) {
    fromSplit = s;
}


// Marks the hand as done (stood, busted or reached 21)
void Hand::setFinished(bool f) {
    finished = f;
}


// True if the player doubled down on this hand
bool Hand::isDoubled() const {
    return doubled;
}


// True if this hand came from a split
bool Hand::isFromSplit() const {
    return fromSplit;
}
 

// True if no more moves can be made on this hand
bool Hand::isFinished() const {
    return finished;
}
 
// Reading the hand 
// Returns the current bet in cents
long long Hand::getBet() const {
    return betAmount;
}
 
// Returns the best Blackjack total
// Every Ace starts as 11; while the total is over 21, one Ace at a time
// drops to 1
int Hand::getTotal() const {
    int total = 0;
    int aces = 0;
 
    for (const Card& c : cards) {
        total += c.getValue();
        if (c.getValue() == 11) {   // only an Ace is worth 11
            aces++;
        }
    }
 
    while (total > 21 && aces > 0) {
        total -= 10;                // count one Ace as 1 instead of 11
        aces--;
    }
 
    return total;
}
 

// True if an Ace is still counted as 11 after adjusting
// Uses the same steps as getTotal(), then checks whether any Ace is left at 11
bool Hand::isSoft() const {
    int total = 0;
    int aces = 0;
 
    for (const Card& c : cards) {
        total += c.getValue();
        if (c.getValue() == 11) {
            aces++;
        }
    }
 
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }
 
    return aces > 0;
}


// True for exactly two cards totalling 21, not from a split
bool Hand::isBlackjack() const {
    return cards.size() == 2 && getTotal() == 21 && !fromSplit;
}
 

// True if the total is over 21
bool Hand::isBust() const {
    return getTotal() > 21;
}
 

// True if the hand has exactly two cards of the same value (e.g. 8+8 or K+10)
// Rules.cpp checks the money and the four-hand limit.
bool Hand::canSplit() const {
    return cards.size() == 2 && cards[0].getValue() == cards[1].getValue();
}
 

// True if the hand has exactly two cards. Rules.cpp checks the money
bool Hand::canDouble() const {
    return cards.size() == 2;
}
 

// Returns a copy of the cards in the hand, for display and checks
std::vector<Card> Hand::getCards() const {
    return cards;
}