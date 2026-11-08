#ifndef PLAYER_H
#define PLAYER_H

#include "Bankroll.h"
#include <string>
#include <vector>

// Represents a real person playing at the table. A player can control multiple seats.
class Player {
public:
    // Initializes the player with a custom display name
    Player(std::string playerName);
    ~Player();

    // Returns the player's display name
    std::string getName() const;
    
    // Returns a pointer to the player's bankroll so bets can be validated and deducted
    Bankroll* getBankroll();
    
    // Assigns a table seat number to this player for the current round
    void addSeat(int seatNumber);
    
    // Clears all assigned seats at the end of a round
    void clearSeats();
    
    // Returns a list of all seat numbers currently controlled by this player
    std::vector<int> getSeats() const;

private:
    std::string name;             // The player's chosen name
    Bankroll bankroll;            // The player's single money pool, shared across all their seats
    std::vector<int> activeSeats; // The seat indices (1-6) this player is currently betting on
};

#endif