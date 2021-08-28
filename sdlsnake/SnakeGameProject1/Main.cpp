#include <iostream>
#include "Game.h"

using namespace std;

int main(int argc, char* argv[])
{
    Game game = Game();
    Game().Initialize();
    cout << "Game has terminated successfully." << endl;
    return 0;
}