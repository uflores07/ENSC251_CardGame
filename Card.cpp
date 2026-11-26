/*
 * Implements the Card class declared in Card.h
 * Gives each card its Blackjack value and its short console text, such as "KH" or "10S"
 */
 
#include "Card.h"
 

// Creates a card with the given rank and suit
Card::Card(Rank r, Suit s) : rank(r), suit(s) {}
 

// Destructor: nothing to clean up
Card::~Card() {}


// Returns the Blackjack value of the card
// Number cards return 2-10, Jack, Queen and King return 10, and an Ace returns 11
// Hand.cpp lowers an Ace to 1 when 11 would bust the hand
int Card::getValue() const {
    if (rank == ACE)
        return 11;
    if (rank >= JACK)      // Jack, Queen or King
        return 10;
    return rank;           // TWO to TEN already equal 2 to 10
}


// Returns short text for the console: the rank followed by the suit letter
// Examples: "KH" for King of Hearts, "10S" for Ten of Spades, "AD" for Ace of Diamonds
std::string Card::toString() const {
    std::string text;
 
    // Rank part: a letter for Ace and face cards, the number for 2-10
    switch (rank) {
        case ACE:   text = "A"; break;
        case KING:  text = "K"; break;
        case QUEEN: text = "Q"; break;
        case JACK:  text = "J"; break;
        default:    text = std::to_string(rank); break;
    }
 
    // Suit part: added to the end of the rank with +=
    switch (suit) {
        case HEARTS:   text += "H"; break;
        case DIAMONDS: text += "D"; break;
        case CLUBS:    text += "C"; break;
        case SPADES:   text += "S"; break;
    }
 
    return text;
}