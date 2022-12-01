#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>

class Game {
  public:
    Game();
    ~Game();

    void init(const char* title);
    void setup();
    void frameStart();
    void frameEnd();
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running();
    bool loadMedia();

  private:
    int counter;
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int FPS;
    Uint32 frameStartTimestamp;
    Uint32 frameEndTimestamp;
    float frameDuration;
    float dT; 
};

 #endif