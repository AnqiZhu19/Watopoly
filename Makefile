CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

OBJS = main.o game.o board.o player.o \
       academicbuilding.o residence.o gym.o \
       ownablesquare.o nonownable.o \
       slc.o needleshall.o cardeffect.o \
       squarefactory.o auction.o savemanager.o

watopoly: $(OBJS)
	$(CXX) $(CXXFLAGS) -o watopoly $(OBJS)

main.o: main.cc game.h
	$(CXX) $(CXXFLAGS) -c main.cc

game.o: game.cc game.h board.h player.h square.h ownablesquare.h \
        academicbuilding.h residence.h gym.h nonownable.h auction.h savemanager.h
	$(CXX) $(CXXFLAGS) -c game.cc

board.o: board.cc board.h square.h player.h academicbuilding.h \
         residence.h gym.h nonownable.h slc.h needleshall.h
	$(CXX) $(CXXFLAGS) -c board.cc

player.o: player.cc player.h ownablesquare.h academicbuilding.h observer.h
	$(CXX) $(CXXFLAGS) -c player.cc

academicbuilding.o: academicbuilding.cc academicbuilding.h player.h board.h
	$(CXX) $(CXXFLAGS) -c academicbuilding.cc

residence.o: residence.cc residence.h player.h board.h
	$(CXX) $(CXXFLAGS) -c residence.cc

gym.o: gym.cc gym.h player.h board.h
	$(CXX) $(CXXFLAGS) -c gym.cc

ownablesquare.o: ownablesquare.cc ownablesquare.h player.h board.h auction.h
	$(CXX) $(CXXFLAGS) -c ownablesquare.cc

nonownable.o: nonownable.cc nonownable.h player.h board.h
	$(CXX) $(CXXFLAGS) -c nonownable.cc

slc.o: slc.cc slc.h player.h board.h cardeffect.h
	$(CXX) $(CXXFLAGS) -c slc.cc

needleshall.o: needleshall.cc needleshall.h player.h board.h cardeffect.h
	$(CXX) $(CXXFLAGS) -c needleshall.cc

cardeffect.o: cardeffect.cc cardeffect.h player.h board.h
	$(CXX) $(CXXFLAGS) -c cardeffect.cc

squarefactory.o: squarefactory.cc squarefactory.h
	$(CXX) $(CXXFLAGS) -c squarefactory.cc

auction.o: auction.cc auction.h ownablesquare.h player.h
	$(CXX) $(CXXFLAGS) -c auction.cc

savemanager.o: savemanager.cc savemanager.h board.h player.h \
               ownablesquare.h academicbuilding.h residence.h gym.h
	$(CXX) $(CXXFLAGS) -c savemanager.cc

clean:
	rm -f $(OBJS) watopoly

.PHONY: clean
