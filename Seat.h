#ifndef SEAT_H
#define SEAT_H

#include "Hand.h"
#include "Player.h"
#include "SideBet.h"
#include <vector>

// Represents one of the physical six positions at the Blackjack table.
class Seat {
public:
    // Initializes the seat with its table position number (1-6)
    Seat(int number);
    ~Seat();

    // Assigns a player to this seat for the round
    void assignPlayer(Player* p);
    
    // Empties the seat for the next round
    void clearSeat();
    
    // Returns true if a player is currently sitting here
    bool isOccupied() const;
    
    // Returns a pointer to the player who owns this seat
    Player* getPlayer() const;

    // Adds a new hand to this seat (used during initial deal and after splits)
    void addHand(Hand h);
    
    // Returns a reference to a specific hand for gameplay modifications
    Hand& getHand(int index);
    
    // Returns the current number of hands at this seat (1 normally, up to 4 if split)
    int getNumHands() const;
    
    // Processes a split: takes the hand at the given index, splits its cards, and adds the new hand
    void splitHand(int handIndex);

    // Insurance bet management
    void setInsuranceBet(int amount);
    int getInsuranceBet() const;
    
    // Registers an active side bet for this seat
    void addSideBet(SideBet* sb);
    
    // Returns all active side bets on this seat for settlement
    std::vector<SideBet*> getSideBets() const;

private:
    int seatNumber;                 // The table position (1-6)
    Player* owner;                  // Pointer to the player currently playing this seat (nullptr if empty)
    std::vector<Hand> hands;        // The main hands being played at this seat
    int insuranceBet;               // The amount wagered on insurance (if the dealer shows an Ace)
    std::vector<SideBet*> sideBets; // Pointers to any side bets placed at this seat
};

#endif