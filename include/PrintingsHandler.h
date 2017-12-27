//
// Created by alon on 26/12/17.
//

#ifndef EX5CLIENT_TEMPCLASS_H
#define EX5CLIENT_TEMPCLASS_H


#include <iostream>
using namespace std;

class PrintingsHandler {
public:
    static void PrintOuterMenu();
    static void PrintInnerMenu();
    static void DisplayPossibleCommands();
    static void AskUserToInsertSpecificCommand();
    static void PrintServerResponse(string response);
    static void FailureMessage();

};


#endif //EX5CLIENT_TEMPCLASS_H
