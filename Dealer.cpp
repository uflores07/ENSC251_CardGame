#include "Dealer.h"

    Dealer::Dealer() {}
    Dealer::~Dealer() {}

    // Adds a dealt card to the dealer's hand
    void Dealer::addCard(Card c) {
        dealerHand.addCard(c);
    }

    // Clears the dealer's hand at the end of a round
    void Dealer::clearHand() {
        dealerHand.clear();
    }

    // Returns the dealer's first card (the only one visible to players initially)
    Card Dealer::getUpcard() const {
        return dealerHand.getCards().front();
    }

    // Checks if the dealer's first two cards equal 21 (used during the peek)
    bool Dealer::hasBlackjack() const {
        return dealerHand.isBlackjack();
    }

    // Applies house rules to determine if the dealer must draw another card (Hits until 17, stands on soft 17)
    bool Dealer::shouldHit() const {
        // Implementation for house rules
    }

    // Returns the dealer's full hand for payout comparisons
    Hand Dealer::getHand() const {
        return dealerHand;
    }