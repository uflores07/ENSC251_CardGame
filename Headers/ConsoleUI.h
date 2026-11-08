#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include "Table.h"
#include <string>

// Handles all text-based input/output and prevents invalid data from crashing the game.
class ConsoleUI {
public:
    ConsoleUI();
    ~ConsoleUI();

    // Safely prompts the user for an integer within a specific range, reprompting on bad input
    int promptForInt(std::string prompt, int min, int max);
    
    // Safely prompts the user for a text string (e.g., player names)
    std::string promptForString(std::string prompt);
    
    // Prompts the user for a yes/no question and returns true for yes, false for no
    bool promptForYesNo(std::string prompt);
    
    // Presents the available gameplay actions (Hit, Stand, Double, Split) dynamically based on legality
    int promptForMove(std::string prompt, bool canDouble, bool canSplit);

    // Renders the entire table state, including all seats and active bets
    void displayTable(const Table& t);
    
    // Displays the cards and total of a specific hand, labeled with the owner's name
    void displayHand(const Hand& h, std::string ownerName);
    
    // Displays the dealer's hand. If hideHoleCard is true, the second card is hidden as [?]
    void displayDealer(const Dealer& d, bool hideHoleCard);
    
    // Lists all active players and their current bankroll balances
    void displayBankrolls(const std::vector<Player>& players);
    
    // Prints a generic formatted message or error to the console
    void displayMessage(std::string msg);
};

#endif