#include "simulator.h"

using namespace std;

int main()
{
    Game game = Game("game_config.txt");
    game.simulate();
    game.print_strategy_map("strategies.txt");
    game.print_nfg_format("game.nfg");

    return 0;
}