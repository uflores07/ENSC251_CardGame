#ifndef TABLE_H
#define TABLE_H

#include "Shoe.h"
#include "Dealer.h"
#include "Seat.h"
#include "Player.h"
#include "ConsoleUI.h"
#include <vector>
#include "PerfectPairs.h"
#include "TwentyOnePlusThree.h"
// The central game controller that owns all entities and orchestrates the flow of the game.
class Table {
public:
    Table();
    ~Table();

    // The primary public method that starts the application loop until all players leave
    void runGame();

private:
    // Core game components owned by the table
    Shoe shoe;
    Dealer dealer;
    std::vector<Seat> seats;     // Fixed array of 6 seats
    std::vector<Player> players; // List of active human players
    ConsoleUI ui;                // The interface controller

    // Internal helper methods representing the chronological steps of a single round
    void initializeGame();       // Gets player count, names, and sets up initial bankrolls
    void assignSeats();          // Maps players to physical table seats before betting starts
    void takeBets();             // Iterates through occupied seats to collect main bets
    void dealInitialCards();     // Distributes two cards to each active seat and the dealer
    void settleSideBets();       // Evaluates and pays out side bets immediately after the deal
    void handleInsurance();      // Offers insurance if dealer upcard is an Ace, runs dealer peek
    void playSeats();            // Prompts players for hit/stand/double/split actions seat by seat
    void playDealer();           // Reveals the hole card and draws until 17
    void settleMainBets();       // Compares final hands, pays winners, and collects losing bets
    void removeBrokePlayers();   // Evicts players whose bankrolls fall below the table minimum
};

#endif