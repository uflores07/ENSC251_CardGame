/*
 * Implements the Dealer class declared in Dealer.h
 * The dealer's cards are kept in a Hand, so totals, blackjack and busts
 * all come from Hand.cpp
 * House rule: hit until the total is 17 or more, and stand on all 17s, including soft 17
 */
 
#include "Dealer.h"
 

// Creates a dealer with an empty hand
Dealer::Dealer() {}


// Destructor: nothing to clean up
Dealer::~Dealer() {}

 
// Adds a dealt card to the dealer's hand
// The first card is the upcard and the second is the hole card
void Dealer::addCard(Card c) {
    dealerHand.addCard(c);
}


// Empties the dealer's hand at the end of a round
void Dealer::clearHand() {
    dealerHand.clear();
}


// Returns the dealer's first card, the only one players can see at the start
// Only call after the deal, because an empty hand has no upcard
Card Dealer::getUpcard() const {
    return dealerHand.getCards().front();
}
 

// True if the dealer's first two cards total 21
// Table uses this for the peek when the upcard is an Ace or a 10-value card
bool Dealer::hasBlackjack() const {
    return dealerHand.isBlackjack();
}


// True if the dealer must take another card
// The dealer hits on 16 or less and stands on any 17 or more
// Soft 17 (for example A + 6) is still 17, so the dealer stands on it too
bool Dealer::shouldHit() const {
    return dealerHand.getTotal() < 17;
}


// Returns a copy of the dealer's hand, for display and for comparing hands in Payout
Hand Dealer::getHand() const {
    return dealerHand;
}