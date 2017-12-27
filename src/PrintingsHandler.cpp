//
// Created by kostarubtsov1990 on 26/12/17.
//


#include "../include/PrintingsHandler.h"

void PrintingsHandler::PrintOuterMenu(){
    cout<<"~Welcome to Reversi Game~"<<endl<<endl;
    cout<<"Please choose below options:"<<endl;
    cout<<"1.Single Player"<<endl;
    cout<<"2.Local Multi Player"<<endl;
    cout<<"3.Remote Multi Player"<<endl<<endl;
    cout<<"Press the number and enter to continue: ";
}

void PrintingsHandler::PrintInnerMenu(){
    cout<<endl<<"Please choose the game style:"<<endl;
    cout<<"1.Classic Game"<<endl;
    cout<<"2.Alternate Game"<<endl<<endl;
    cout<<"Press the number and enter to continue: ";
}

void PrintingsHandler::DisplayPossibleCommands(){
    cout << "Please choose one of the following options: " << endl;
    cout << "1. start new game" << endl;
    cout << "2. joining a game" << endl;
    cout << "3. get list of available games" << endl;

}

void PrintingsHandler::AskUserToInsertSpecificCommand(){
    cout << "Please enter one of the following commands in the correct format: " << endl;
    cout << "start <name>" << endl;
    cout << "list_games" << endl;
    cout << "join <name>" << endl;
}

void PrintingsHandler::PrintServerResponse(string response){
    cout << response << endl;
}

void PrintingsHandler::FailureMessage(){
    cout << "error" << endl;
}