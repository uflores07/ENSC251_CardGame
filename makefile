CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = blackjack

SRCS = main.cpp \
       Table.cpp \
       ConsoleUI.cpp \
       Seat.cpp \
       Player.cpp \
       Dealer.cpp \
       Hand.cpp \
       Rules.cpp \
       Payout.cpp \
       Shoe.cpp \
       Bankroll.cpp \
       Card.cpp \
       SideBet.cpp \
       PerfectPairs.cpp \
       TwentyOnePlusThree.cpp

OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)