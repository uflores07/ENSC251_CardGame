#ifndef BANKROLL_H
#define BANKROLL_H

// Manages a player's money balance, avoiding floating-point errors by using cents.
class Bankroll {
public:
    // Initializes the bankroll. Defaults to 100,000 cents ($1000.00).
    Bankroll(int initialCents = 100000); 
    ~Bankroll();

    // Checks whether the player has enough balance to cover a specific bet amount (in cents)
    bool canAfford(int cents) const;
    
    // Safely deducts the specified amount in cents from the balance when a bet is placed
    void deduct(int cents);
    
    // Adds the specified amount in cents to the balance when winnings are paid out
    void add(int cents);
    
    // Returns the current total balance in cents
    int getBalanceCents() const;

private:
    int balanceCents; // The internal money balance, stored strictly in whole cents
};

#endif