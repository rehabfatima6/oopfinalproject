// main.cpp - driver file: runs the GameEngine game loop.
#include <iostream>
#include "heist.h"
#include <cstdlib>
#include <ctime>
using namespace std;
int main() 
{
    srand((unsigned)time(0));                   // makes the culprit random in every game

    LeaderBoard board("leader/scoreBoard.txt"); // association: the engine uses it but does not own it
    GameEngine& engine = GameEngine::getInstance();
    engine.attachLeaderBoard(&board);
    engine.showWelcome();

    while (!engine.isFinished()) 
    {
        try {
            engine.step();
        }
        catch (const GameException& e) {        // all custom exceptions are caught here
            engine.handleException(e);
        }
    }

    GameEngine::destroyInstance();              // frees all characters
    cout << "\nGoodbye, Detective.\n";
    return 0;
}