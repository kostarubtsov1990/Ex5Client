/*
 * Name: Kosta Rubtsov
 * Id: 319206892
 * Name: Alon Barkan
 * Id: 200958296
 */

#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include "../include/MultiPlayerFlow.h"
#include "../include/GameClient.h"

#define BUF_SIZE 1024
#define START_NEW_GAME 1
#define JOIN_GAME 2
#define LIST_OF_AVAILABLE_GAMES 3

using namespace std;
/*
MultiPlayerFlow::MultiPlayerFlow() {
    
}
*/

MultiPlayerFlow::MultiPlayerFlow(Game* game, gameType type): type(type),
                                                             GameFlow::GameFlow(game){}

void MultiPlayerFlow::Run() {
    //This will run one of the possible game types of multi player.
    switch (type){
        case local:
            RunLocal();
        case remote:
            RunRemote();
    }
}

void MultiPlayerFlow::RunLocal() {
    GameLogic* logic = game->GetLogic();
    Board* board = game->GetBoard();

    cout << "current board:" << endl << endl;
    board->printBoard();

    //each player plays its turn till game's over.
    while (!logic->IsGameOver(board)) {
        RunCurrentTurnOfTheGame(xplayer, X);
        RunCurrentTurnOfTheGame(oplayer, O);
    }
    //declare the winner of the game (or draw)
    logic->DeclareWinner(board);
}

void MultiPlayerFlow::RunRemote() {
    GameLogic* logic = game->GetLogic();
    Board* board = game->GetBoard();

    //contains player's move
    char answerBuffer [BUF_SIZE];
    bzero(&answerBuffer,sizeof(answerBuffer));



    while (true) {//client
        string option = "";
        string chosenCommand = "";
        //print the optional commands that the user can send to the server: start <name>, list_games, join <name>
        PrintingsHandler::DisplayPossibleCommands();
        cin >> option;

        int numericOption = atoi(option.c_str());

        while  (numericOption != 3 && numericOption != 2 && numericOption != 1) {
            cout << "Wrong number please choose one of the possible numbers: 1, 2, 3" << endl;
            cin >> option;
            numericOption = atoi(option.c_str());
        }


        //Client is about to start a new game
        if (numericOption == START_NEW_GAME) {
            cout << "Please enter the following command in the correct format: start <name>" << endl;
            getline(cin, chosenCommand);
            getline(cin, chosenCommand);

            while (chosenCommand.find("start") == string::npos) {
                cout << "Wrong command" << endl;
                cout << "Please enter the following command in the correct format: start <name>" << endl;
                getline(cin, chosenCommand);
            }

            ConnectToServer();
            //send to the server string of the format start <name>
            int var = write(gameClientSocket, chosenCommand.c_str(), strlen(chosenCommand.c_str()) + 1);
            //handle errors
            if (var == -1) {
                cout << "Error reading arg1" << endl;
            }
            if (var == 0) {
                cout << "Client disconnected" << endl;
            }
            //the client that sends start new game message, is the X player
            player = xplayer;
            //therefore, its opponent is the O player
            opponentPlayer = oplayer;
            playerSymbol = X;
            opponentPlayerSymbol = O;
            //read the server's response (either "game_created_successfully" or "-1" if game already exists
            var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
            //handle errors
            if (var == -1) {
                cout << "Error reading arg1" << endl;
            }
            if (var == 0) {
                cout << "Client disconnected" << endl;
            }
            if (strcmp(answerBuffer,"game_created_successfully") == 0) {
                cout << "game created successfully! Please wait till your opponent join your game..." << endl << endl;
                var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
                if (var == -1) {
                    cout << "Error reading arg1" << endl;
                }
                if (var == 0) {
                    cout << "Client disconnected" << endl;
                }
                if (strcmp(answerBuffer,"start_game") == 0) {
                    RunGame();
                }
            }
            else{
                //server sent "-1" to client
                cout << "the name you entered already exists. please choose another name" << endl << endl;
                if (close(gameClientSocket)) {
                    cout << "Error: unable to close client socket" << endl;
                }
                //go back to the menu. new client will connect and choose command
                continue;
            }
        }
        //Client is about to ask for avaiable games on the server side
        else if (numericOption == LIST_OF_AVAILABLE_GAMES) {
            cout << "Please enter the following command in the correct format: list_games" << endl;
            cin >> chosenCommand;

            while (chosenCommand.find("list_games") == string::npos) {
                cout << "Wrong command" << endl;
                cout << "Please enter the following command in the correct format: list_games" << endl;
                cin >> chosenCommand;
            }

            ConnectToServer();
            //send to the server the command entered by the user
            int var = write(gameClientSocket, chosenCommand.c_str(), strlen(chosenCommand.c_str()) + 1);

            if (var == -1) {
                cout << "Error reading arg1" << endl;
                return exit(-1);
            }
            if (var == 0) {
                cout << "Client disconnected" << endl;
                return exit(-1);
            }
            /*read the respone from the server.
             *server's response may be "No games are available" or a string with available games.
             */
            var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));

            if (var == -1) {
                cout << "Error reading arg1" << endl;
                return exit(-1);
            }
            if (var == 0) {
                cout << "Client disconnected" << endl;
                return exit(-1);
            }
            //in any case, connection is closed and the control will go back to the start of the loop
            if (close(gameClientSocket)) {
                cout << "Error: unable to close client socket" << endl;
            }

            cout << answerBuffer << endl;
            continue;
            
        }
        //Client is about to join a game
        else // option == JOIN_GAME
        {
            cout << "Please enter the following command in the correct format: join <name>" << endl;
            getline(cin, chosenCommand);
            getline(cin, chosenCommand);

            while (chosenCommand.find("join") == string::npos) {
                cout << "Wrong command" << endl;
                cout << "Please enter the following command in the correct format: join <name>" << endl;
                getline(cin, chosenCommand);
            }

            ConnectToServer();
            //send to the server join <name> command
            int var = write(gameClientSocket, chosenCommand.c_str(), strlen(chosenCommand.c_str()) + 1);
            if (var == 0) {
                cout << "Error: No Connection with Server side" << endl;
            }
            if (var == -1) {
                cout << "Error: unable to execute write" << endl;
            }
            //read the response from the server
            var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
            if (var == 0) {
                cout << "Error: No Connection with Server side" << endl;
            }
            if (var == -1) {
                cout << "Error: unable to execute read" << endl;
            }

            //Client joined to the requested game.
            if (strcmp(answerBuffer,"joined_to_game") == 0) {
                //the client that joins, is the O player
                player = oplayer;
                //its opponent is the X player (the one the sent start <name>)
                opponentPlayer = xplayer;
                playerSymbol = O;
                opponentPlayerSymbol = X;
                /*Client reads from the server "start_game" message.
                 *this message is sent to both players (the one that started the game, and
                 * one who joined it). It is sent from GameHandler method (ReversiGameManager.cpp)
                 */
                var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
                if (var == 0) {
                    cout << "Error: No Connection with Server side" << endl;
                }
                if (var == -1) {
                    cout << "Error: unable to execute read" << endl;
                }
                if (strcmp(answerBuffer,"start_game") == 0) {
                    RunGame();
                }
            }
            //server returned either "game_is_full" or "game_not_exist"
            else {
                cout << answerBuffer << endl;
                if (close(gameClientSocket)) {
                    cout << "Error: unable to close socket" << endl;
                }
                //go back to the menu. new client will connect and choose command
                continue;
            }
        }
    }
}

string MultiPlayerFlow::RunCurrentTurnOfTheGame(playerIdentifier id,
                                               boardContent symbol) {
    GameLogic* logic = game->GetLogic();
    Board* board = game->GetBoard();
    //check possible moves of current player
    logic->CheckPossibleMoves(board,id);

    PrintHandler(id,logic->GetMoves());
    //if no possible moves, pass turn to the second player
    if (logic->GetMoves().empty()) {
        return "no_moves";
    }
    //chosen cell of current player
    Cell chosenCell = this->InputHandler();
    //if cellisnt validask again the playerto choose cell from the possiblelist
    while(!logic->IsLocationValid(chosenCell)) {//
        PrintHandler(id, logic->GetMoves());
        chosenCell = InputHandler();
    }
    //update the board and print it to the screen
    logic->UpdateBoard(board,chosenCell.getX(),chosenCell.getY(),symbol);
    cout << "current board:" << endl << endl;
    board->printBoard();

    //needed to pass the move to the server (and the server will move it to the other client.
    ostringstream ss;
    ss << chosenCell.getX() << "," << chosenCell.getY();
    return ss.str();
}

void MultiPlayerFlow::PrintHandler(playerIdentifier id,
                                    const vector<Cell>& possibleLocations) const {
    //xplayer
    if (id == xplayer) {
        cout << "\nX: " << "It's your move." << endl;
    }
    //oplayer
    else {
        cout << "\nO: " << "It's your move." << endl;
    }
    //current player has no possible moves
    if (possibleLocations.empty()) {
        cout << "No possible moves. play passes back to the other player." << endl;
    }
    //print the possible moves to the screen
    else {
        cout << "Your possible moves: " << endl;
        possibleLocations[0].printFormatOfCell();
        for (int i = 1; i < possibleLocations.size(); i++) {
            cout << ", ";
            possibleLocations[i].printFormatOfCell();
        }
        cout << endl;
    }
}

Cell MultiPlayerFlow::InputHandler() const {
    cout << "Please enter your move row,col:";
    string input = "";
    getline(cin, input);
    //input is valid
    if (!((input.size() == 3) && (atoi(&input[0]) >= 1 && atoi(&input[0]) <= 8)
          &&(input[1] == ',')&&(atoi(&input[2]) >= 1 && atoi(&input[2]) <= 8))){
        return Cell(-1,-1);
    }
    //input is invalid
    else {
        int row, col;
        char comma;
        row = atoi(&input[0]);
        comma = input[1];
        col = atoi(&input[2]);

        row--;
        col--;
        return Cell(row, col);
    }
}

void MultiPlayerFlow::RunGame() {
    GameLogic* logic = game->GetLogic();
    Board* board = game->GetBoard();
    char answerBuffer [BUF_SIZE];
    //sec player (the process of Client2 will enter this condition
    if (player == oplayer) {
        cout << "Wait for first move..." << endl << endl;
        //second player reads the FIRST move that is done by client1 (=first player/Xplayer)
        int n = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
        if (n == 0) {
            cout << "Error: No Connection with Server side" << endl;
        }
        if (n == -1) {
            cout << "Error: unable to execute read" << endl;
        }

        if (strcmp(answerBuffer, "server_shutdown") == 0){
            cout << "Server shut down"<< endl << endl;
            if (close(gameClientSocket)) {
                cout << "Error: unable to close socket" << endl;
            }
            return;
        }
        //second player updates its own board with the step player1 has done.
        logic->CheckPossibleMoves(board, opponentPlayer);
        logic->UpdateBoard(board, atoi(&answerBuffer[0]), atoi(&answerBuffer[2]), opponentPlayerSymbol);
        //board->printBoard();
        //cout << endl << endl;
        //n == -1 , n == 0..clients disconnected
    }
    //both clients print their board (Client1's board will be the initial board.
    //Client2's board will be the board updated with Client1's step.
    cout << "current board:" << endl << endl;
    board->printBoard();

    if (opponentPlayer == xplayer)
        cout << "X played (" << ++answerBuffer[0] << "," << ++answerBuffer[2] << ")" << endl;

    //each player plays its turn till game's over.
    while (!logic->IsGameOver(board)) {
        //current player performs a step (chooses a cell and updates its own board). result contains its step.
        string result = RunCurrentTurnOfTheGame(player, playerSymbol);
        //To add failure condition.
        //current player sends to the server its step (=(x,y)).
        int n = write(gameClientSocket, result.c_str(), strlen(result.c_str()) + 1);

        if (n == 0) {
            cout << "Server shut down"<< endl << endl;
            close(gameClientSocket);
            return;
        }
        if (n == -1) {
            cout << "Error: unable to execute write" << endl;
        }
        //Client1 will enter this condition
        if (player == xplayer)
            cout << "Wait for second player to take a move..." << endl << endl;
            //Client2 will enter this else
        else
            cout << "Wait for first player to take a move..." << endl << endl;

        //current player reads from the server the step that its opponent has done.
        n = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
        if (n == 0) {
            cout << "Error: no connection with server";
            return;
        }
        if (n == -1) {
            cout << "Error: unable to execute read" << endl;
            return;
        }

        //opponent player sent "END", current player (=current client process) declares winner and closes connection
        if (strcmp(answerBuffer, "END") == 0) {
            logic->DeclareWinner(board);
            if (close(gameClientSocket)) {
                cout << "Error: unable to close client socket" << endl;
            }
            return;
        }
            //if its opponent performed a step, then he updates its own board with the opponent step.
        else if (strcmp(answerBuffer, "no_moves") && strcmp(answerBuffer, "server_shutdown")) {
            logic->CheckPossibleMoves(board, opponentPlayer);
            logic->UpdateBoard(board, atoi(&answerBuffer[0]), atoi(&answerBuffer[2]), opponentPlayerSymbol);

            //current client prints its updated board (updated with its opponent step)
            cout << "current board:" << endl << endl;
            board->printBoard();

            if (opponentPlayer == xplayer)
                cout << "X played (" << ++answerBuffer[0] << "," << ++answerBuffer[2] << ")" << endl;
            else
                cout << "O played (" << ++answerBuffer[0] << "," << ++answerBuffer[2] << ")" << endl;
            //answerbuffer is "no moves"
        } else if (strcmp(answerBuffer, "server_shutdown") == 0){
            cout << "Server shut down" << endl << endl;
            close(gameClientSocket);
            return;
        }
        else{
            cout << "Opponent player had no possible moves." << endl;
        }
    }
    //this code will be done by only one of the clients.
    string endMassage = "END";
    //send END message to the server who passes it to the other player that will declare winner and disconnect.
    int n = write(gameClientSocket, endMassage.c_str(), strlen(endMassage.c_str()) + 1);
    if (n == -1) {
        cout << "Error: unable to execute write" << endl;
    }
    if (n == 0) {
        cout << "Server shut down"<< endl << endl;
        if (close(gameClientSocket)) {
            cout << "Error: unable to execute close" << endl;
        }
        return;
    }
    //declare the winner of the game (or draw)
    logic->DeclareWinner(board);
    //current player disconnects
    if (close(gameClientSocket)) {
        cout << "Error: unable to execute close" << endl;
    }
}

void MultiPlayerFlow::ConnectToServer() {
    //Read the settings from file.
    const char* fileName = "settings.txt";
    string ip;
    string portString;
    ifstream myfile(fileName);
    getline(myfile, ip);
    getline(myfile, portString);

    int port = atoi(portString.c_str());

    //first argument is the IP of the computer which the server runs on.
    //second argument is the port of the server
    GameClient gameClient(ip.c_str(), port);
    try {
        //sort of file descriptor. after this line, client is connected to server
        gameClientSocket = gameClient.connectToServer();
    }catch (const char* msg) {
        cout << "Failed to connect to server. Reason: " << msg << endl;
        exit(-1);
    }
}

MultiPlayerFlow::~MultiPlayerFlow() {
    delete game;
}