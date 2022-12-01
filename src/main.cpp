#include "Game/Game.h"
#include "Game/Game.cpp"

Game *game = NULL;

int main( int argc, char* args[] )
{
    game = new Game();

    game->init("PONG ECS");
    game->setup();

    while (true && game->running())
    {
      game->frameStart();
      game->handleEvents();
      game->update();
      game->render();
      game->frameEnd();
    }

    game->clean();

    return 0;
}