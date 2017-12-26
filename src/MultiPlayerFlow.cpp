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
#define LIST_OF_AVAILABLE_GAMES 2
#define JOIN_GAME 3


using namespace std;
/*
MultiPlayerFlow::MultiPlayerFlow() {
    
}
*/

MultiPlayerFlow::MultiPlayerFlow(Game* game, gameType type, PrintingsHandler handler): type(type), handler(handler),
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

    //Read the settings from file.
    const char* fileName = "settings.txt";
    string ip;
    string portString;
    ifstream myfile(fileName);
    getline(myfile, ip);
    getline(myfile, portString);

    int port = atoi(portString.c_str());


    while (true) {//client
        int option;
        string chosenCommand = "";
        //print the optional commands that the user can send to the server: start <name>, list_games, join <name>
        this->handler.DisplayPossibleCommands();
        cin >> option;

        cout << "Please enter the following command in the correct format: start <name>" << endl;

        cin >> chosenCommand;


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

        int var = write(gameClientSocket, chosenCommand.c_str(), strlen(chosenCommand.c_str()) + 1);

        if (var == -1) {
            cout << "Error reading arg1" << endl;
            return exit(-1);
        }
        if (var == 0) {
            cout << "Client disconnected" << endl;
            return exit(-1);
        }


        if (option == START_NEW_GAME) {
            player = xplayer;
            opponentPlayer = oplayer;
            playerSymbol = X;
            opponentPlayerSymbol = O;

            var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));

            if (var == -1) {
                cout << "Error reading arg1" << endl;
                return exit(-1);
            }
            if (var == 0) {
                cout << "Client disconnected" << endl;
                return exit(-1);
            }

            if (strcmp(answerBuffer,"game_created_successfully") == 0) {

                var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));

                if (strcmp(answerBuffer,"start_game") == 0) {
                    RunGame();
                }
            }
            else{
                close(gameClientSocket);
                continue;
            }
        }
        else if (option == LIST_OF_AVAILABLE_GAMES) {
            cout << "Please enter the following command in the correct format: list_games " << endl;
            cin >> chosenCommand;
            //send to the server the command entered by the user
            int var = write(gameClientSocket, chosenCommand.c_str(), strlen(chosenCommand.c_str()) + 1);
            //read the respone from the server
            var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
            close(gameClientSocket);

            cout << answerBuffer << endl;
            continue;
            
        }
        else // option == JOIN_GAME
        {
            cout << "Please enter the following command in the correct format: join <name>" << endl;
            cin >> chosenCommand;
            //send to the server the command entered by the user
            int var = write(gameClientSocket, chosenCommand.c_str(), strlen(chosenCommand.c_str()) + 1);
            //read the response from the server
            var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
            //handle game
            if (strcmp(answerBuffer,"joined_to_game") == 0) {
                player = oplayer;
                opponentPlayer = xplayer;
                playerSymbol = O;
                opponentPlayerSymbol = X;
                var = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));
                if (strcmp(answerBuffer,"start_game") == 0) {
                    RunGame();
                }
            }
            //server returned either "game_is_full" or "game_not_exist"
            else {
                close(gameClientSocket);
                continue;
            }

        }
    }
    /*
     *
     * SEND TO THE SERVER ONE OF THE POSSIBLE COMMANDS IN THE CORRECT FORMAT.
     * (MAKE SURE THAT THE CLIENT CAN SEND ONLY THESE FORMATS AND NOT UNRECOGNIZED STRINGS).
     *
     * IF THE USER CHOOSES GAME_LIST COMMAND, THEN PRINT THE AVAILABLE GAMES
     * SENT BY THE SERVER.
     * (anyway the client will print the response from the server. if no game available,
     * correct message will be printed).
     * PRINT CORRENT MESSAGE OF THE GAMES AVAILABLE IN THE SERVER SIDE
     * (TAKE INTO ACCOUNT THAT IN THIS CASE, CLIENT MIGHT NOT SENDS IMMEDIATELY A NEW_GAME COMMAND OR A JOIN COMMAND.)
     *
     * IF THE USER CHOOSES JOIN <NAME>, RESPONSES FROM THE SERVER MIGHT BE:
     * 'game_not_exist', 'game_is_full', 'joined_to_game'.
     * HANDLE EACH RESPONSE ACCORDINGLY.
     * concatenate to game_not_exist and game_is_full a message that ask the client to choose another name
     *
     * IF THE USER CHOOSES START <NAME>. -1 might be returned by the server, indicating that such game is already exist.
     * ask the user to enter another name.
     *
     *
     *
     *
     */
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
        //Client1 will enter this condition
        if (player == xplayer)
            cout << "Wait for second player to take a move..." << endl << endl;
            //Client2 will enter this else
        else
            cout << "Wait for first player to take a move..." << endl << endl;

        //current player reads from the server the step that its opponent has done.
        n = read(gameClientSocket, answerBuffer, sizeof(answerBuffer));

        //opponent player sent "END", current player (=current client process) declares winner and closes connection
        if (strcmp(answerBuffer, "END") == 0) {
            logic->DeclareWinner(board);
            close(gameClientSocket);
            return;
        }
            //if its opponent performed a step, then he updates its own board with the opponent step.
        else if (strcmp(answerBuffer, "no_moves")) {
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
        } else{
            cout << "Opponent player had no possible moves." << endl;
        }


    }
    //this code will be done by only one of the clients.
    string endMassage = "END";
    //send END message to the server who passes it to the other player that will declare winner and disconnect.
    int n = write(gameClientSocket, endMassage.c_str(), strlen(endMassage.c_str()) + 1);

    //declare the winner of the game (or draw)
    logic->DeclareWinner(board);
    //current player disconnects
    close(gameClientSocket);
}

MultiPlayerFlow::~MultiPlayerFlow() {
    delete game;
}
