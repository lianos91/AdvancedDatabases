CC = g++
CFLAGS = -std=c++11 -Wall -fexceptions
LDFLAGS = -pthread -lpqxx -lpq
INC = -Iinclude
OBJ_AUCT = obj/Auctioneer/Auctioneer.o obj/Auctioneer/RegTable.o obj/Auctioneer/DBlayer.o obj/Auctioneer/main.o obj/AbstractServer/AbstractServer.o obj/CommunicationService/CommunicationService.o obj/MiscLib/MiscLib.o obj/logger/logger.o
OBJ_SYNCH = obj/SynchrServer/SynchrServer.o obj/SynchrServer/AbstractServer.o obj/SynchrServer/CommunicationService.o obj/SynchrServer/MiscLib.o obj/SynchrServer/logger.o
OBJ_BIDDER = obj/Bidder/bidder.o obj/Bidder/main.o obj/logger/logger.o obj/AbstractServer/AbstractServer.o obj/MiscLib/MiscLib.o obj/CommunicationService/CommunicationService.o 

all: bin/SyncrServer bin/BiddingPlatform bin/bidder

# Bidding Platform

bin/BiddingPlatform: $(OBJ_AUCT)
	$(CC) -o bin/BiddingPlatform $(OBJ_AUCT) $(LDFLAGS)

obj/MiscLib/MiscLib.o: src/MiscLib/MiscLib.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/MiscLib/MiscLib.cpp -o obj/MiscLib/MiscLib.o

obj/logger/logger.o: src/logger/logger.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/logger/logger.cpp -o obj/logger/logger.o

obj/AbstractServer/AbstractServer.o: src/AbstractServer/AbstractServer.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/AbstractServer/AbstractServer.cpp -o obj/AbstractServer/AbstractServer.o

obj/Auctioneer/RegTable.o: src/Auctioneer/RegTable.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/Auctioneer/RegTable.cpp -o obj/Auctioneer/RegTable.o

obj/Auctioneer/DBlayer.o: src/Auctioneer/DBlayer.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/Auctioneer/DBlayer.cpp -o obj/Auctioneer/DBlayer.o

obj/Auctioneer/main.o: src/Auctioneer/main.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/Auctioneer/main.cpp -o obj/Auctioneer/main.o

obj/Auctioneer/Auctioneer.o: src/Auctioneer/Auctioneer.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/Auctioneer/Auctioneer.cpp -o obj/Auctioneer/Auctioneer.o

obj/CommunicationService/CommunicationService.o: src/CommunicationService/CommunicationService.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/CommunicationService/CommunicationService.cpp -o obj/CommunicationService/CommunicationService.o

# Synchronization Server

bin/SyncrServer: $(OBJ_SYNCH)
	$(CC) -o bin/SyncrServer $(OBJ_SYNCH) $(LDFLAGS) -s

obj/SynchrServer/SynchrServer.o: src/SynchrServer/SynchrServer.cpp
	$(CC) $(CFLAGS) -O2 $(INC) -c src/SynchrServer/SynchrServer.cpp -o obj/SynchrServer/SynchrServer.o

obj/SynchrServer/MiscLib.o: src/MiscLib/MiscLib.cpp
	$(CC) $(CFLAGS) -O2 $(INC) -c src/MiscLib/MiscLib.cpp -o obj/SynchrServer/MiscLib.o

obj/SynchrServer/CommunicationService.o: src/CommunicationService/CommunicationService.cpp
	$(CC) $(CFLAGS) -O2 $(INC) -c src/CommunicationService/CommunicationService.cpp -o obj/SynchrServer/CommunicationService.o

obj/SynchrServer/AbstractServer.o: src/AbstractServer/AbstractServer.cpp
	$(CC) $(CFLAGS) -O2 $(INC) -c src/AbstractServer/AbstractServer.cpp -o obj/SynchrServer/AbstractServer.o

obj/SynchrServer/logger.o: src/logger/logger.cpp
	$(CC) $(CFLAGS) -O2 $(INC) -c src/logger/logger.cpp -o obj/SynchrServer/logger.o

# Bidder

bin/bidder: $(OBJ_BIDDER)
	$(CC) -o bin/bidder $(OBJ_BIDDER)

obj/Bidder/bidder.o: src/Bidder/bidder.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/Bidder/bidder.cpp -o obj/Bidder/bidder.o

obj/Bidder/main.o: src/Bidder/main.cpp
	$(CC) $(CFLAGS) -g $(INC) -c src/Bidder/main.cpp -o obj/Bidder/main.o

clean:
	rm obj/Auctioneer/*.o
	rm obj/Bidder/*.o
	rm obj/SynchrServer/*.o
	rm obj/logger/*.o
	rm obj/CommunicationService/*.o
	rm obj/AbstractServer/*.o
	rm obj/MiscLib/*.o
	rm bin/bidder
	rm bin/BiddingPlatform
	rm bin/SyncrServer
