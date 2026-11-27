#include "Table.h"
#include "Rules.h"
#include "Payout.h"
#include <string>
#include <algorithm>

// ============================================================================
// Constructor: runs automatically once when a Table object is created.
// ": shoe(Rules::NUM_DECKS)" initializes the Shoe with 6 decks before the body runs.
// ============================================================================
Table::Table() : shoe(Rules::NUM_DECKS) {
    // Create the 6 physical seats at the table (numbered 1 through 6)
    for (int i = 1; i <= 6; ++i) {
        seats.push_back(Seat(i));
    }

    // Build and randomize the cards in the shoe so we're ready to deal
    shoe.shuffle();
}

// ============================================================================
// Destructor: runs automatically when the Table object is destroyed (game ends).
// Frees any dynamically allocated (created with `new`) SideBet objects still on seats.
// Note: If Seat::~Seat() already deletes SideBet*, remove this loop to avoid double-free.
// ============================================================================
Table::~Table() {
    for (Seat& seat : seats) {
        for (SideBet* sb : seat.getSideBets()) {
            delete sb;
        }
    }
}

// ============================================================================
// Main Game Loop: the "conductor" that runs Blackjack round by round
// until every player runs out of money or chooses to stop playing.
// ============================================================================
void Table::runGame() {
    // One-time setup: ask how many players (1-6) and get their names
    initializeGame();

    // Repeat one full round of Blackjack as long as at least one player remains
    while (!players.empty()) {
        // 1. Reshuffle the shoe if it is running low on cards
        if (shoe.needsReshuffle()) {
            ui.displayMessage("Reshuffling the shoe...");
            shoe.shuffle();
        }

        // 2. Let each player pick 1 (or more) open seats at the table
        assignSeats();

        // 3. Collect main bets and optional side bets for every occupied seat
        takeBets();

        // 4. Deal 2 starting cards to each occupied seat and to the dealer
        dealInitialCards();

        // 5. Side bets only depend on the first 2 player cards + dealer upcard,
        //    so they are settled immediately after the deal
        settleSideBets();

        // 6. Offer insurance if dealer shows an Ace, and peek for dealer Blackjack
        handleInsurance();

        // 7. Only let players and the dealer take turns if the dealer did NOT
        //    already get an instant Blackjack
        if (!dealer.hasBlackjack()) {
            playSeats();
            playDealer();
        }

        // 8. Compare each player's final hand(s) against the dealer and pay winners
        settleMainBets();

        // 9. Clear cards, bets, and seat assignments so the table is clean for next round
        dealer.clearHand();
        for (Seat& seat : seats) {
            seat.clearSeat();
        }
        for (Player& player : players) {
            player.clearSeats();
        }

        // 10. Remove any players who can no longer afford the table minimum bet
        removeBrokePlayers();

        // 11. If players are still left, ask if they want to play another round
        if (!players.empty()) {
            if (!ui.promptForYesNo("Play another round?")) {
                break; // Exit the while loop and end the game
            }
        }
    }

    ui.displayMessage("Thanks for playing!");
}

// ============================================================================
// Sets up the players once at the start of the game.
// ============================================================================
void Table::initializeGame() {
    // Safely ask for an integer between 1 and 6
    int numPlayers = ui.promptForInt("How many Players?", 1, 6);

    // Ask for each player's name, create a Player object, and store it in `players`.
    // std::to_string(i) converts the integer `i` into text so we can join it with "+".
    for (int i = 1; i <= numPlayers; ++i) {
        players.push_back(Player(ui.promptForString("Enter name for Player " + std::to_string(i) + ":")));
    }

    // Show everyone's starting bankroll
    ui.displayBankrolls(players);
}

// ============================================================================
// Lets each human player claim one or more of the 6 physical seats for this round.
// ============================================================================
void Table::assignSeats() {
    int occupiedCount = 0; // Tracks how many of the 6 table seats are taken so far

    // Use `Player&` (reference) so changes via player.addSeat() modify the real Player,
    // not a temporary copy.
    for (Player& player : players) {
        bool pickingSeats = true;

        // Keep asking this player for seats until they say no or all 6 seats are full
        while (occupiedCount < 6 && pickingSeats) {
            int seatNum = ui.promptForInt(
                player.getName() + ", choose any open seat from (1-6):", 1, 6
            );

            // Seats are numbered 1-6 for the user, but stored at indexes 0-5 in the vector
            if (seats[seatNum - 1].isOccupied()) {
                ui.displayMessage("Seat " + std::to_string(seatNum) + " is already taken. Try again.");
                continue; // Skip the rest of this turn and ask again
            }

            // Link the seat and the player together.
            // `&player` passes the memory address of the real Player to the Seat.
            seats[seatNum - 1].assignPlayer(&player);
            player.addSeat(seatNum);
            ++occupiedCount;

            // Check if the player has enough money to cover MIN_BET on one more seat
            int nextSeatsCount = player.getSeats().size() + 1;
            bool canAffordAnother = player.getBankroll()->canAfford(Rules::MIN_BET * nextSeatsCount);

            if (occupiedCount < 6 && canAffordAnother) {
                pickingSeats = ui.promptForYesNo(player.getName() + ", would you like another seat?");
            } else {
                pickingSeats = false;
            }
        }
    }
}

// ============================================================================
// Walks through all 6 seats and collects main bets + optional side bets
// from every seat that has a player sitting in it.
// ============================================================================
void Table::takeBets() {
    // Loop by index `i` (0..5) so we can display the seat number as `i + 1`
    for (size_t i = 0; i < seats.size(); ++i) {
        // `Seat&` is a reference to the real seat in `seats[i]` (not a copy)
        Seat& seat = seats[i];

        // If nobody is sitting at this seat, skip it and move to the next seat
        if (!seat.isOccupied()) {
            continue;
        }

        // Get pointers to the player sitting here and their bankroll (`->` accesses pointer members)
        Player* p = seat.getPlayer();
        Bankroll* br = p->getBankroll();

        // Player cannot bet more than the table max OR more than they currently have
        int maxAllowed = std::min(Rules::MAX_BET, br->getBalanceCents());

        // Ask for the main bet, deduct it from the player's bankroll, and create their starting Hand
        int mainBet = ui.promptForInt(
            p->getName() + " (Seat " + std::to_string(i + 1) + ") enter main bet:",
            Rules::MIN_BET,
            maxAllowed
        );
        br->deduct(mainBet);
        seat.addHand(Hand(mainBet));

        // Optional Perfect Pairs side bet (only offered if player can still afford MIN_BET)
        if (br->canAfford(Rules::MIN_BET) &&
            ui.promptForYesNo(p->getName() + ", place a Perfect Pairs side bet?")) {
            int maxSide = std::min(Rules::MAX_BET, br->getBalanceCents());
            int sideWager = ui.promptForInt("Enter Perfect Pairs wager:", Rules::MIN_BET, maxSide);

            // Allocate on the heap with `new` because Seat stores base-class SideBet* pointers
            SideBet* sb = new PerfectPairs(Rules::MIN_BET, Rules::MAX_BET);
            sb->placeBet(sideWager);
            br->deduct(sideWager);
            seat.addSideBet(sb);
        }

        // Optional 21+3 side bet
        if (br->canAfford(Rules::MIN_BET) &&
            ui.promptForYesNo(p->getName() + ", place a 21+3 side bet?")) {
            int maxSide = std::min(Rules::MAX_BET, br->getBalanceCents());
            int sideWager = ui.promptForInt("Enter 21+3 wager:", Rules::MIN_BET, maxSide);

            SideBet* sb = new TwentyOnePlusThree(Rules::MIN_BET, Rules::MAX_BET);
            sb->placeBet(sideWager);
            br->deduct(sideWager);
            seat.addSideBet(sb);
        }
    }
}

// ============================================================================
// Deals 2 initial cards to every occupied seat and the dealer in casino order:
// 1st card to each seat then dealer, then 2nd card to each seat then dealer.
// ============================================================================
void Table::dealInitialCards() {
    for (int pass = 0; pass < 2; ++pass) {
        for (Seat& seat : seats) {
            if (seat.isOccupied()) {
                // getHand(0) gets the seat's first (and currently only) hand
                seat.getHand(0).addCard(shoe.dealCard());
            }
        }
        dealer.addCard(shoe.dealCard());
    }

    // Show the table and the dealer's hand (`true` hides the dealer's 2nd "hole" card)
    ui.displayTable(*this);
    ui.displayDealer(dealer, true);
}

// ============================================================================
// Evaluates and pays out Perfect Pairs / 21+3 side bets right after the deal.
// ============================================================================
void Table::settleSideBets() {
    Card upcard = dealer.getUpcard(); // Dealer's visible card (needed for 21+3)

    for (Seat& seat : seats) {
        if (!seat.isOccupied()) {
            continue;
        }

        // Safety check: make sure the hand actually has 2 cards before reading cards[0] and cards[1]
        std::vector<Card> cards = seat.getHand(0).getCards();
        if (cards.size() < 2) {
            continue;
        }

        Player* p = seat.getPlayer();

        // Check every side bet placed on this seat (polymorphism calls the right calculatePayout)
        for (SideBet* sb : seat.getSideBets()) {
            int payout = sb->calculatePayout(cards[0], cards[1], upcard);
            if (payout > 0) {
                p->getBankroll()->add(payout);
                ui.displayMessage(
                    p->getName() + " won " + std::to_string(payout) + " on " + sb->getName() + "!"
                );
            } else {
                ui.displayMessage(p->getName() + " lost their " + sb->getName() + " side bet.");
            }
        }
    }
}

// ============================================================================
// Step 1: If dealer shows an Ace (11), offer insurance (costs half of main bet).
// Step 2: If dealer shows an Ace (11) or 10-value card, peek for dealer Blackjack.
//         If dealer has Blackjack, reveal the hole card and pay winning insurance bets.
// ============================================================================
void Table::handleInsurance() {
    int upcardVal = dealer.getUpcard().getValue();

    // Step 1: Offer insurance only when dealer's face-up card is an Ace (value 11)
    if (upcardVal == 11) {
        for (Seat& seat : seats) {
            if (!seat.isOccupied()) {
                continue;
            }
            if (!Rules::isLegalInsurance(seat.getHand(0))) {
                continue;
            }

            Player* p = seat.getPlayer();
            Bankroll* br = p->getBankroll();
            int maxIns = seat.getHand(0).getBet() / 2; // Insurance costs half of the main bet

            // Only offer insurance if the player has enough remaining funds to cover it
            if (maxIns > 0 && br->canAfford(maxIns) &&
                ui.promptForYesNo(p->getName() + ", buy insurance for " + std::to_string(maxIns) + "?")) {
                br->deduct(maxIns);
                seat.setInsuranceBet(maxIns);
            }
        }
    }

    // Step 2: Dealer always peeks when upcard is an Ace (11) or a 10/J/Q/K (10)
    if (upcardVal == 11 || upcardVal == 10) {
        if (dealer.hasBlackjack()) {
            ui.displayMessage("Dealer has Blackjack!");
            ui.displayDealer(dealer, false); // `false` reveals the dealer's hidden hole card

            // Pay 2:1 to every seat that bought insurance
            for (Seat& seat : seats) {
                if (!seat.isOccupied()) {
                    continue;
                }
                int insBet = seat.getInsuranceBet();
                if (insBet > 0) {
                    int pay = Payout::calculateInsurancePayout(insBet);
                    seat.getPlayer()->getBankroll()->add(pay);
                    ui.displayMessage(
                        seat.getPlayer()->getName() + " won " + std::to_string(pay) + " on insurance!"
                    );
                }
            }
        }
    }
}

// ============================================================================
// Lets each occupied seat play out all of its hands (Hit, Stand, Double, Split).
// ============================================================================
void Table::playSeats() {
    for (Seat& seat : seats) {
        if (!seat.isOccupied()) {
            continue;
        }

        Player* p = seat.getPlayer();
        Bankroll* br = p->getBankroll();

        // Use an index loop (instead of range-for) because splitting adds a new hand
        // to the seat, increasing seat.getNumHands() while we are looping!
        for (int h = 0; h < seat.getNumHands(); ++h) {
            // Keep prompting until this hand stands, busts, doubles, or hits 21
            while (!seat.getHand(h).isFinished()) {
                Hand& hand = seat.getHand(h);

                // Auto-finish if the player already has Blackjack or 21
                if (hand.isBlackjack() || hand.getTotal() == 21) {
                    hand.setFinished(true);
                    break;
                }

                // Show current hand and dealer's upcard (hole card still hidden)
                ui.displayHand(hand, p->getName());
                ui.displayDealer(dealer, true);

                // Check which special moves are both legal by Blackjack rules AND affordable
                bool canDouble = Rules::isLegalDouble(hand) && br->canAfford(hand.getBet());
                bool canSplit  = Rules::isLegalSplit(hand, seat.getNumHands()) && br->canAfford(hand.getBet());

                // Assumes ConsoleUI::promptForMove returns: 1=Hit, 2=Stand, 3=Double, 4=Split
                int move = ui.promptForMove(p->getName() + ", choose your move:", canDouble, canSplit);

                if (move == 1) { // Hit: draw 1 card
                    hand.addCard(shoe.dealCard());
                    if (hand.isBust()) {
                        ui.displayHand(hand, p->getName());
                        ui.displayMessage(p->getName() + " busts!");
                        hand.setFinished(true);
                    } else if (hand.getTotal() == 21) {
                        hand.setFinished(true);
                    }
                } else if (move == 2) { // Stand: keep current total and end turn on this hand
                    hand.setFinished(true);
                } else if (move == 3 && canDouble) { // Double Down: double bet, draw 1 card, and finish
                    br->deduct(hand.getBet());
                    hand.doubleBet();
                    hand.setDoubled(true);
                    hand.addCard(shoe.dealCard());
                    ui.displayHand(hand, p->getName());
                    if (hand.isBust()) {
                        ui.displayMessage(p->getName() + " busts!");
                    }
                    hand.setFinished(true);
                } else if (move == 4 && canSplit) { // Split: pay matching bet, split into 2 hands, deal 1 card each
                    br->deduct(hand.getBet());
                    seat.splitHand(h);
                    // Re-fetch via seat.getHand(...) instead of using `hand` because
                    // splitHand() may resize the hands vector in memory
                    seat.getHand(h).addCard(shoe.dealCard());
                    seat.getHand(seat.getNumHands() - 1).addCard(shoe.dealCard());
                }
            }
        }
    }
}

// ============================================================================
// Plays out the dealer's turn automatically according to house rules
// (reveal hole card, then keep hitting while dealer.shouldHit() is true).
// ============================================================================
void Table::playDealer() {
    ui.displayMessage("Dealer's turn:");
    ui.displayDealer(dealer, false); // Reveal hole card

    while (dealer.shouldHit()) {
        dealer.addCard(shoe.dealCard());
        ui.displayDealer(dealer, false);
    }
}

// ============================================================================
// Compares every hand at every occupied seat against the dealer's final hand,
// adds any winnings to the player's bankroll, and prints updated balances.
// ============================================================================
void Table::settleMainBets() {
    Hand dealerHand = dealer.getHand();

    for (Seat& seat : seats) {
        if (!seat.isOccupied()) {
            continue;
        }

        Player* p = seat.getPlayer();
        Bankroll* br = p->getBankroll();

        // Check every hand at this seat (1 normally, or up to 4 if the player split)
        for (int h = 0; h < seat.getNumHands(); ++h) {
            Hand& hand = seat.getHand(h);
            Payout::Result res = Payout::determineResult(hand, dealerHand);
            int winnings = Payout::calculateWinnings(res, hand.getBet());

            // Pay winnings (for WIN, BLACKJACK, or returned bet on PUSH)
            if (winnings > 0) {
                br->add(winnings);
            }

            if (res == Payout::BLACKJACK) {
                ui.displayMessage(p->getName() + " got BLACKJACK! Won " + std::to_string(winnings));
            } else if (res == Payout::WIN) {
                ui.displayMessage(p->getName() + " WON " + std::to_string(winnings));
            } else if (res == Payout::PUSH) {
                ui.displayMessage(p->getName() + " PUSHED (bet returned).");
            } else {
                ui.displayMessage(p->getName() + " LOST.");
            }
        }
    }

    // Show updated bankrolls for all players at the end of the round
    ui.displayBankrolls(players);
}

// ============================================================================
// Removes any players who no longer have enough money to place the minimum bet.
// Uses an iterator loop because erasing inside a range-for loop is unsafe.
// ============================================================================
void Table::removeBrokePlayers() {
    for (auto it = players.begin(); it != players.end(); ) {
        if (!it->getBankroll()->canAfford(Rules::MIN_BET)) {
            ui.displayMessage(it->getName() + " is out of funds and leaves the table.");
            // erase() removes the player and returns a valid iterator to the next player
            it = players.erase(it);
        } else {
            ++it; // Only advance manually when we did NOT erase
        }
    }
}
