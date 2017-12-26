//
// Created by alon on 26/12/17.
//

#ifndef EX4CLIENT_PRINTINGSHANDLER_H
#define EX4CLIENT_PRINTINGSHANDLER_H

#include <iostream>
using namespace std;
//enum menuKind {outerMenu, innerMenu};

class PrintingsHandler {
public:

    PrintingsHandler(/*menuKind outerOrInner*/);
    void PrintOuterMenu()const;
    void PrintInnerMenu()const;
    void DisplayPossibleCommands()const;
    void AskUserToInsertSpecificCommand()const;
    void PrintServerResponse(string response)const;
    void FailureMessage()const;

};


#endif //EX4CLIENT_PRINTINGSHANDLER_H
