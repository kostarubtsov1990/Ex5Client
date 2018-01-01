/*
 * Name: Kosta Rubtsov
 * Id: 319206892
 * Name: Alon Barkan
 * Id: 200958296
 */

#include <cstdio>
#include <cstdlib>
#include "../include/ReversiGameMenu.h"



void ReversiGameMenu::Start() {
    PrintingsHandler::PrintOuterMenu();

    string option;

    cin>>option;
    int gameOption = atoi(option.c_str());


    while  (gameOption != 3 && gameOption != 2 && gameOption != 1) {
        cout << "Wrong number please choose one of the possible numbers: 1, 2, 3" << endl;
        cin >> option;
        gameOption = atoi(option.c_str());
    }

    PrintingsHandler::PrintInnerMenu();


    cin>>option;
    int gameStyleOption = atoi(option.c_str());
    getchar();


    while  (gameStyleOption != 2 && gameStyleOption != 1) {
        cout << "Wrong number please choose one of the possible numbers: 1, 2" << endl;
        cin >> option;
        gameStyleOption = atoi(option.c_str());
    }

    GameLogic* logic;
    Game* game;
    Board* board;


    switch (gameStyleOption) {
        case 1:
            logic = new DefaultLogic();
            game = new ReversiGame(logic);
            board = new Board();
            game->SetBoard(board);

            switch (gameOption) {
                case 1:
                    //To add SinglePlayerFlow in the future.
                    userChoiceFlow = new SinglePlayerFlow(game);
                    break;
                case 2:
                    userChoiceFlow = new MultiPlayerFlow(game,local);
                    break;
                case 3:

                    userChoiceFlow = new MultiPlayerFlow(game,remote);
                    break;
            }
            break;

        //Add alternate game style in the future.
        case 2:
            break;
    }
}

GameFlow* ReversiGameMenu::GetGameFlow() {
    return userChoiceFlow;
}

ReversiGameMenu::~ReversiGameMenu() {
    delete userChoiceFlow;
}