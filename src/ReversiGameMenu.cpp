/*
 * Name: Kosta Rubtsov
 * Id: 319206892
 * Name: Alon Barkan
 * Id: 200958296
 */

#include <cstdio>
#include "../include/ReversiGameMenu.h"



void ReversiGameMenu::Start() {
    PrintingsHandler::PrintOuterMenu();

    int gameOption;
    cin>>gameOption;

    PrintingsHandler::PrintInnerMenu();

    int gameStyleOption;
    cin>>gameStyleOption;
    getchar();

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