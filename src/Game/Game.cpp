#include <iostream>
#include <cmath>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "entt/entt.hpp"
#include "Game.h"

const int SCREEN_WIDTH = 850;
const int SCREEN_HEIGHT = 480;

int screen_width;
int screen_height;

entt::registry reg;

struct BallComponent{

};

struct PlayerComponent{
  int playerNum;
};

struct PositionComponent {
  int x, y;
};

struct VelocityComponent {
  int x, y;
};

struct CubeComponent {
  int w, h;
};

PositionComponent ballSpawnPosition = {20, 20};
VelocityComponent ballSpawnVelocity = {200, 200};
CubeComponent ballRect = {20, 20};

CubeComponent paddle1Rect = {100, 20};
PositionComponent paddle1Position = {SCREEN_WIDTH/2 - paddle1Rect.w/2, 0};
VelocityComponent paddle1Velocity = {0, 0};

CubeComponent paddle2Rect = {100, 20};
PositionComponent paddle2Position = {SCREEN_WIDTH/2 - paddle2Rect.w/2, SCREEN_HEIGHT - 20};
VelocityComponent paddle2Velocity = {0, 0};

void createBallEntity()
{
  const entt::entity e = reg.create();
  reg.emplace<PositionComponent>(e, ballSpawnPosition);
  reg.emplace<VelocityComponent>(e, ballSpawnVelocity);
  reg.emplace<CubeComponent>(e, ballRect);   
  reg.emplace<BallComponent>(e);
}

void createPlayerEntity(int num){
  const entt::entity e = reg.create();
  if(num == 1){
    reg.emplace<PositionComponent>(e, paddle1Position);
    reg.emplace<VelocityComponent>(e, paddle1Velocity);
    reg.emplace<CubeComponent>(e, paddle1Rect);   
  } else {
    reg.emplace<PositionComponent>(e, paddle2Position);
    reg.emplace<VelocityComponent>(e, paddle2Velocity);
    reg.emplace<CubeComponent>(e, paddle2Rect);  
  }
  reg.emplace<PlayerComponent>(e, num);
 
}

void cubeRenderSystem(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255 ,255, 1);

  const auto view = reg.view<PositionComponent, CubeComponent>();
  for (const entt::entity e : view) {
    const PositionComponent position = view.get<PositionComponent>(e);
    const CubeComponent cube = view.get<CubeComponent>(e);

    SDL_Rect rect = { position.x, position.y, cube.w, cube.h };    
    SDL_RenderFillRect(renderer, &rect);
  }
}

void bounceSystem(double dT){
  auto view = reg.view<PositionComponent, VelocityComponent, BallComponent, CubeComponent>();
  for (const entt::entity e: view){
     PositionComponent& pos = view.get<PositionComponent>(e);
     VelocityComponent& vel = view.get<VelocityComponent>(e);
     CubeComponent& cub = view.get<CubeComponent>(e);
    if (pos.x <= 0)
    {
      vel.x *= -1;
    }

    if (pos.x + cub.w >= SCREEN_WIDTH)
    {
      vel.x *= -1;
    }

    if (pos.y <= 0)
    {
      vel.y *= -1;
    }

    if (pos.y + cub.h >= SCREEN_HEIGHT)
    {
      vel.y *= -1;
    }
  }
}

void movementSystem(double dT) {
  auto view = reg.view<PositionComponent, VelocityComponent>();
  for (const entt::entity e : view) {
    PositionComponent& pos = view.get<PositionComponent>(e);
    VelocityComponent& vel = view.get<VelocityComponent>(e);

        
        pos.x += vel.x * dT;
        pos.y += vel.y * dT;
  }
}

bool paddleEventSystem(SDL_Event event) {
  auto view = reg.view< VelocityComponent, PlayerComponent, PositionComponent, CubeComponent>();
  for (const entt::entity e : view){
    VelocityComponent& vel = view.get<VelocityComponent>(e);
    const PlayerComponent player = view.get<PlayerComponent>(e);
    const PositionComponent pos = view.get<PositionComponent>(e);
    const CubeComponent cube = view.get<CubeComponent>(e);
    int id = player.playerNum;
    switch( event.type ){
          // Inputs 
          case SDL_KEYDOWN:
              switch( event.key.keysym.sym ){
                  case SDLK_LEFT:
                    if (id == 1){
                      if(pos.x >= 0){
                        vel.x = -200;
                      } else {
                        vel.x = 0;
                      }
                    }
                    break;
                  case SDLK_RIGHT:
                    if (id == 1){
                      if(pos.x + 100  <= SCREEN_WIDTH){
                        vel.x = 200;  
                      } else{
                        vel.x = 0;
                      }
                    }
                    break;
                  case SDLK_a:
                    if (id == 2){
                      if(pos.x >= 0){
                        vel.x = -200;
                      } else {
                        vel.x = 0;
                      }
                    }
                    break;
                  case SDLK_d:
                    if(id == 2){
                      if(pos.x + 100  <= SCREEN_WIDTH){
                        vel.x = 200;  
                      } else{
                        vel.x = 0;
                      }
                    }
                    break;
              }
              break;
          case SDL_KEYUP:
              switch( event.key.keysym.sym ){
                  case SDLK_LEFT:
                      if( vel.x < 0 )
                          vel.x = 0;
                      break;
                  case SDLK_RIGHT:
                      if( vel.x > 0 )
                          vel.x = 0;
                      break;
                  case SDLK_a:
                    if (vel.x < 0){
                      vel.x = 0;
                    }
                    break;
                  case SDLK_d:
                    if(vel.x > 0){
                      vel.x = 0;
                    }
                    break;
                  default:
                      break;
              }
              break;
          default:
              break;
      }
  }
}

Game::Game()
{
  FPS = 60;
  frameDuration = (1.0f / FPS) * 1000.0f;
  std::cout << "Game Object Constructed!" << std::endl;

  SDL_RenderSetLogicalSize(renderer, 160, 144);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
}

Game::~Game()
{
  std::cout << "Game Object Destroyed!" << std::endl;
}

void Game::init(const char* title)
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
  {
    window = SDL_CreateWindow(title, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    IMG_Init(IMG_INIT_PNG);
    std::cout << "Game Start!" << std::endl;

    isRunning = true;
  } else {
    isRunning = false;
  }
  counter = 0;
}

void Game::setup()
{
  createBallEntity();
  createPlayerEntity(1);
  createPlayerEntity(2);
}

void Game::frameStart()
{
  std::cout << "---- Frame: " << counter << " ----" << std::endl;
  frameStartTimestamp = SDL_GetTicks();
  if (frameEndTimestamp)
  {
    dT = (frameStartTimestamp - frameEndTimestamp) / 1000.0f;
  }
  else
  {
    dT = 0;
  }
}

void Game::frameEnd()
{
  frameEndTimestamp = SDL_GetTicks();

  float actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

  if (actualFrameDuration < frameDuration)
  {
    SDL_Delay(frameDuration - actualFrameDuration);
  }

  counter++;
  std::cout << " " << std::endl;
}

void Game::handleEvents()
{
  std::cout << "Game Handling events..." << std::endl;
  std::cout << screen_width << std::endl;
  std::cout << screen_height << std::endl;
  

  SDL_Event event;
  while (SDL_PollEvent(&event) != 0)
  {
    if (event.type == SDL_QUIT)
    {
      isRunning = false;
    }
      paddleEventSystem(event);
       
  }
}

void Game::update()
{
  std::cout << "Game Updating.." << std::endl;
  bounceSystem(dT);
  movementSystem(dT);

}

void Game::render()
{
  std::cout << "Game Rendering..." << std::endl;

  SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 1);

  SDL_RenderClear(renderer);

  cubeRenderSystem(renderer);

  SDL_RenderPresent(renderer);
}

void Game::clean()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
	IMG_Quit();
  SDL_Quit();
  std::cout << "Game Over." << std::endl;
}

bool Game::running()
{
  return isRunning;
}