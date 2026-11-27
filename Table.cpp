#include "Table.h"
#include "Rules.h"
#include "Payout.h"
#include <string>
#include <algorithm>

Table::Table() : shoe(Rules::NUM_DECKS) {
    for (int i = 1; i <= 6; ++i) {
        seats.push_back(Seat(i));
    }
    shoe.shuffle();
}

Table::~Table() {
    for (Seat& seat : seats) {
        for (SideBet* sb : seat.getSideBets()) {
            delete sb;
        }
    }
}

void Table::runGame() {
        initializeGame();
    while (!players.empty()) {
        // 1. Check if shoe needs reshuffling
        if (shoe.needsReshuffle()) {
            ui.displayMessage("Reshuffling the shoe...");
            shoe.shuffle();
        }
        // 2-6. Setup, deal, and immediate checks
        assignSeats();
        takeBets();
        dealInitialCards();
        settleSideBets();
        handleInsurance();
        // 7. Play out seats and dealer only if dealer does not have Blackjack
        if (!dealer.hasBlackjack()) {
            playSeats();
            playDealer();
        }
        // 8. Compare hands and pay winners
        settleMainBets();
        // 9. Reset round state
        dealer.clearHand();
        for (Seat& seat : seats) {
            seat.clearSeat();
        }
        for (Player& player : players) {
            player.clearSeats();
        }
        // 10. Evict broke players
        removeBrokePlayers();
        // 11. Ask to play another round
        if (!players.empty()) {
            if (!ui.promptForYesNo("Play another round?")) {
                break;
            }
        }
    }
    ui.displayMessage("Thanks for playing!");
}
void Table::initializeGame() {
    int numPlayers = ui.promptForInt("How many Players?", 1, 6);
    for (int i = 1; i <= numPlayers; ++i) {
        players.push_back(Player(ui.promptForString("Enter name for Player " + std::to_string(i) + ":")));
    }
    ui.displayBankrolls(players);

    // External functions used:
    //   ConsoleUI::promptForInt(prompt, min, max)
    //   ConsoleUI::promptForString(prompt)
    //   ConsoleUI::displayBankrolls(players)
    //   Player::Player(std::string playerName)

    // TODO: Prompt for number of players (1-6) using ui.promptForInt(...)
    // TODO: For each player, prompt for their name using ui.promptForString(...)
    //       and push_back(Player(name)) into `players`
    // TODO: Display initial bankrolls using ui.displayBankrolls(players)
}

void Table::assignSeats() {
    int occupiedCount = 0;
    for(Player& player : players){
        bool pickingSeats = true;

        while ((occupiedCount<6)&&pickingSeats){
            int seatNum = ui.promptForInt(player.getName() + ", choose any open seat from (1-6)", 1, 6
            );
            if (seats[seatNum-1].isOccupied()){
             ui.displayMessage("Seat " + std::to_string(seatNum) + " is already taken. Try again.");
                continue;
            }   

            seats[seatNum-1].assignPlayer(&player);
            player.addSeat(seatNum);
            ++occupiedCount;

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

void Table::takeBets() {
    for (size_t i = 0; i < seats.size(); ++i) {
        Seat& seat = seats[i];
        if (!seat.isOccupied()) {
            continue;
        }
    Player* p = seat.getPlayer();
    Bankroll* br = p->getBankroll();
    int maxAllowed = std::min(Rules::MAX_BET, br->getBalanceCents());
    int mainBet = ui.promptForInt(
            p->getName() + " (Seat " + std::to_string(i + 1) + ") enter main bet:",
            Rules::MIN_BET, 
            maxAllowed
        );
        br->deduct(mainBet);
        seat.addHand(Hand(mainBet));
        if (br->canAfford(Rules::MIN_BET) &&
            ui.promptForYesNo(p->getName() + ", place a Perfect Pairs side bet?")) {
            int maxSide = std::min(Rules::MAX_BET, br->getBalanceCents());
            int sideWager = ui.promptForInt("Enter Perfect Pairs wager:", Rules::MIN_BET, maxSide);
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
            }}}
    // External functions used:
    //   Seat::isOccupied(), Seat::getPlayer(), Seat::addHand(Hand), Seat::addSideBet(SideBet*)
    //   Player::getName(), Player::getBankroll()
    //   Bankroll::getBalanceCents(), Bankroll::canAfford(int), Bankroll::deduct(int)
    //   Rules::MIN_BET, Rules::MAX_BET
    //   Hand::Hand(int initialBet)
    //   PerfectPairs::PerfectPairs(int minBet, int maxBet)
    //   TwentyOnePlusThree::TwentyOnePlusThree(int minBet, int maxBet)
    //   SideBet::placeBet(int amount)
    //   ConsoleUI::promptForInt(prompt, min, max), ConsoleUI::promptForYesNo(prompt)

    // TODO: For each occupied seat in `seats`:
    //   - Get Player* p = seat.getPlayer() and Bankroll* br = p->getBankroll()
    //   - Prompt for main bet between Rules::MIN_BET and std::min(Rules::MAX_BET, balance)
    //   - Call br->deduct(bet) and seat.addHand(Hand(bet))
    //   - Offer PerfectPairs and TwentyOnePlusThree side bets via ui.promptForYesNo(...):
    //       if yes & br->canAfford(...):
    //         allocate with `new PerfectPairs(...)` / `new TwentyOnePlusThree(...)`
    //         call sb->placeBet(sideWager), br->deduct(sideWager), seat.addSideBet(sb)

void Table::dealInitialCards() {
    for (int pass = 0; pass < 2; ++pass) {
        for (Seat& seat : seats) {
            if (seat.isOccupied()) {
                seat.getHand(0).addCard(shoe.dealCard());
            }
        }
        dealer.addCard(shoe.dealCard());
    }
    ui.displayTable(*this);
    ui.displayDealer(dealer, true);
    // External functions used:
    //   Shoe::dealCard()
    //   Seat::isOccupied(), Seat::getHand(int index)
    //   Hand::addCard(Card)
    //   Dealer::addCard(Card)
    //   ConsoleUI::displayTable(const Table&), ConsoleUI::displayDealer(const Dealer&, bool)

    // TODO: Deal in two passes (1st card to each occupied seat's getHand(0), then dealer;
    //       2nd card to each occupied seat's getHand(0), then dealer) using shoe.dealCard()
    // TODO: Display the table with ui.displayTable(*this) and/or ui.displayDealer(dealer, true)
}

void Table::settleSideBets() {
     Card upcard = dealer.getUpcard();
      for (Seat& seat : seats) {
        if (!seat.isOccupied()) {
            continue;
        }
    std::vector<Card> cards = seat.getHand(0).getCards();
        if (cards.size() < 2) {
            continue;
        }
         Player* p = seat.getPlayer();
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
    // External functions used:
    //   Dealer::getUpcard()
    //   Seat::isOccupied(), Seat::getHand(int), Seat::getSideBets(), Seat::getPlayer()
    //   Hand::getCards()
    //   SideBet::calculatePayout(Card, Card, Card), SideBet::getName(), SideBet::getWager()
    //   Player::getName(), Player::getBankroll()
    //   Bankroll::add(int)
    //   ConsoleUI::displayMessage(msg)

    // TODO: Card upcard = dealer.getUpcard()
    // TODO: For each occupied seat:
    //   - Get cards = seat.getHand(0).getCards() (use cards[0] and cards[1])
    //   - For each SideBet* sb in seat.getSideBets():
    //       int payout = sb->calculatePayout(cards[0], cards[1], upcard)
    //       if payout > 0:
    //         seat.getPlayer()->getBankroll()->add(payout)
    //         ui.displayMessage(...) with sb->getName() and payout
}



void Table::handleInsurance() {
    int upcardVal = dealer.getUpcard().getValue();
    if (dealer.getUpcard().getValue() == 11){
        for (Seat& seat : seats) {
        if (!seat.isOccupied()) {
            continue;
        }
        if (!Rules::isLegalInsurance(seat.getHand(0))) {
                continue;
            }
        Player* p = seat.getPlayer();
        Bankroll* br = p->getBankroll();
        int maxIns = seat.getHand(0).getBet() / 2;
        if (maxIns > 0 && br->canAfford(maxIns) &&
                ui.promptForYesNo(p->getName() + ", buy insurance for " + std::to_string(maxIns) + "?")) {
                br->deduct(maxIns);
                seat.setInsuranceBet(maxIns);
            }
        }
    }
   if (upcardVal == 11 || upcardVal == 10) {
        if (dealer.hasBlackjack()) {
            ui.displayMessage("Dealer has Blackjack!");
            ui.displayDealer(dealer, false);
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

    // External functions used:
    //   Dealer::getUpcard(), Dealer::hasBlackjack()
    //   Card::getValue()
    //   Seat::isOccupied(), Seat::getHand(int), Seat::getPlayer(),
    //     Seat::setInsuranceBet(int), Seat::getInsuranceBet()
    //   Hand::getBet()
    //   Rules::isLegalInsurance(const Hand&)
    //   Player::getName(), Player::getBankroll()
    //   Bankroll::canAfford(int), Bankroll::deduct(int), Bankroll::add(int)
    //   Payout::calculateInsurancePayout(int)
    //   ConsoleUI::promptForYesNo(prompt), ConsoleUI::displayDealer(const Dealer&, bool),
    //     ConsoleUI::displayMessage(msg)

    // TODO: If dealer.getUpcard().getValue() == 11 (Ace):
    //   - For each occupied seat where Rules::isLegalInsurance(seat.getHand(0)):
    //       int maxIns = seat.getHand(0).getBet() / 2
    //       if player->getBankroll()->canAfford(maxIns) and ui.promptForYesNo(...):
    //         br->deduct(maxIns)
    //         seat.setInsuranceBet(maxIns)
    // TODO: If dealer upcard value is 11 or 10, peek with dealer.hasBlackjack():
    //   - If true:
    //       ui.displayDealer(dealer, false)
    //       for each occupied seat with seat.getInsuranceBet() > 0:
    //         int pay = Payout::calculateInsurancePayout(seat.getInsuranceBet())
    //         seat.getPlayer()->getBankroll()->add(pay)
    //         ui.displayMessage(...)
}

void Table::playSeats() {
     for (Seat& seat : seats) {
        if (!seat.isOccupied()) {
            continue;
        }
        Player* p = seat.getPlayer();
        Bankroll* br = p->getBankroll();
        for (int h = 0; h < seat.getNumHands(); ++h) {
            while(!seat.getHand(h).isFinished()){
                Hand& hand = seat.getHand(h);
                if (hand.isBlackjack() || hand.getTotal() == 21) {
                    hand.setFinished(true);
                    break;
                }
                ui.displayHand(hand, p->getName());
                ui.displayDealer(dealer, true);
                                bool canDouble = Rules::isLegalDouble(hand) && br->canAfford(hand.getBet());
                bool canSplit  = Rules::isLegalSplit(hand, seat.getNumHands()) && br->canAfford(hand.getBet());
                // Assumes ConsoleUI::promptForMove returns: 1=Hit, 2=Stand, 3=Double, 4=Split
                int move = ui.promptForMove(p->getName() + ", choose your move:", canDouble, canSplit);
                if (move == 1) { // Hit
                    hand.addCard(shoe.dealCard());
                    if (hand.isBust()) {
                        ui.displayHand(hand, p->getName());
                        ui.displayMessage(p->getName() + " busts!");
                        hand.setFinished(true);
                    } else if (hand.getTotal() == 21) {
                        hand.setFinished(true);
                    }
                } else if (move == 2) { // Stand
                    hand.setFinished(true);
                } else if (move == 3 && canDouble) { // Double
                    br->deduct(hand.getBet());
                    hand.doubleBet();
                    hand.setDoubled(true);
                    hand.addCard(shoe.dealCard());
                    ui.displayHand(hand, p->getName());
                    if (hand.isBust()) {
                        ui.displayMessage(p->getName() + " busts!");
                    }
                    hand.setFinished(true);
                } else if (move == 4 && canSplit) { // Split
                    br->deduct(hand.getBet());
                    seat.splitHand(h);
                    // Re-fetch via getHand() because splitHand() may reallocate the vector
                    seat.getHand(h).addCard(shoe.dealCard());
                    seat.getHand(seat.getNumHands() - 1).addCard(shoe.dealCard());
                }
            }
        }
    }
        
    // External functions used:
    //   Seat::isOccupied(), Seat::getPlayer(), Seat::getNumHands(),
    //     Seat::getHand(int), Seat::splitHand(int)
    //   Player::getName(), Player::getBankroll()
    //   Bankroll::canAfford(int), Bankroll::deduct(int)
    //   Hand::isFinished(), Hand::isBlackjack(), Hand::getTotal(), Hand::isBust(),
    //     Hand::getBet(), Hand::addCard(Card), Hand::doubleBet(),
    //     Hand::setDoubled(bool), Hand::setFinished(bool)
    //   Rules::isLegalHit(const Hand&), Rules::isLegalStand(const Hand&),
    //     Rules::isLegalDouble(const Hand&), Rules::isLegalSplit(const Hand&, int)
    //   Shoe::dealCard()
    //   ConsoleUI::displayHand(const Hand&, std::string),
    //     ConsoleUI::displayDealer(const Dealer&, bool),
    //     ConsoleUI::promptForMove(std::string, bool, bool),
    //     ConsoleUI::displayMessage(msg)

    // TODO: For each occupied seat:
    //   - Loop by index: for (int h = 0; h < seat.getNumHands(); ++h)
    //     - While !seat.getHand(h).isFinished():
    //       - If hand.isBlackjack() || hand.getTotal() == 21 -> hand.setFinished(true); break;
    //       - ui.displayHand(hand, player->getName()) and ui.displayDealer(dealer, true)
    //       - bool canDouble = Rules::isLegalDouble(hand) && br->canAfford(hand.getBet())
    //       - bool canSplit  = Rules::isLegalSplit(hand, seat.getNumHands()) && br->canAfford(hand.getBet())
    //       - int move = ui.promptForMove(..., canDouble, canSplit)
    //       - Hit:    hand.addCard(shoe.dealCard()); if (hand.isBust() || hand.getTotal() == 21) hand.setFinished(true);
    //       - Stand:  hand.setFinished(true);
    //       - Double: br->deduct(hand.getBet()); hand.doubleBet(); hand.setDoubled(true);
    //                 hand.addCard(shoe.dealCard()); hand.setFinished(true);
    //       - Split:  br->deduct(hand.getBet()); seat.splitHand(h);
    //                 deal 1 card to seat.getHand(h) and 1 card to the newly created hand
}

void Table::playDealer() {
    // External functions used:
    //   Dealer::shouldHit(), Dealer::addCard(Card)
    //   Shoe::dealCard()
    //   ConsoleUI::displayDealer(const Dealer&, bool)

    // TODO: Reveal dealer hole card via ui.displayDealer(dealer, false)
    // TODO: While dealer.shouldHit():
    //   - dealer.addCard(shoe.dealCard())
    //   - ui.displayDealer(dealer, false)
    ui.displayMessage("Dealer's turn:");
    ui.displayDealer(dealer, false);
    while (dealer.shouldHit()) {
        dealer.addCard(shoe.dealCard());
        ui.displayDealer(dealer, false);
    }
}

void Table::settleMainBets() {
     Hand dealerHand = dealer.getHand();
    for (Seat& seat : seats) {
        if (!seat.isOccupied()) {
            continue;
        }
        Player* p = seat.getPlayer();
        Bankroll* br = p->getBankroll();
        for (int h = 0; h < seat.getNumHands(); ++h) {
            Hand& hand = seat.getHand(h);
            Payout::Result res = Payout::determineResult(hand, dealerHand);
            int winnings = Payout::calculateWinnings(res, hand.getBet());
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
    ui.displayBankrolls(players);
}
    // External functions used:
    //   Dealer::getHand()
    //   Seat::isOccupied(), Seat::getPlayer(), Seat::getNumHands(), Seat::getHand(int)
    //   Hand::getBet()
    //   Payout::Result (BLACKJACK, WIN, PUSH, LOSS)
    //   Payout::determineResult(const Hand&, const Hand&)
    //   Payout::calculateWinnings(Payout::Result, int)
    //   Player::getName(), Player::getBankroll()
    //   Bankroll::add(int)
    //   ConsoleUI::displayMessage(msg), ConsoleUI::displayBankrolls(players)

    // TODO: Hand dealerHand = dealer.getHand()
    // TODO: For each occupied seat and each hand h in 0 .. seat.getNumHands() - 1:
    //   - Payout::Result res = Payout::determineResult(hand, dealerHand)
    //   - int winnings = Payout::calculateWinnings(res, hand.getBet())
    //   - If winnings > 0 -> seat.getPlayer()->getBankroll()->add(winnings)
    //   - Announce BLACKJACK / WIN / PUSH / LOSS via ui.displayMessage(...)
    // TODO: Display updated balances via ui.displayBankrolls(players)


void Table::removeBrokePlayers() {
    // External functions used:
    //   Player::getName(), Player::getBankroll()
    //   Bankroll::canAfford(int)
    //   Rules::MIN_BET
    //   ConsoleUI::displayMessage(msg)
    // TODO: Iterate through `players` with an iterator:
    //   - If !it->getBankroll()->canAfford(minBet):
    //       ui.displayMessage(it->getName() + " is out of funds and leaves the table.")
    //       it = players.erase(it)
    //   - Else:
    //       ++it
    for (auto it = players.begin(); it != players.end(); ) {
        if (!it->getBankroll()->canAfford(Rules::MIN_BET)) {
            ui.displayMessage(it->getName() + " is out of funds and leaves the table.");
            it = players.erase(it);
        } else {
            ++it;
        }
    }
}
