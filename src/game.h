#pragma once
#include <SDL3/SDL.h>

typedef enum GameType
{
    GAME_ENDLESS,
    GAME_DAILY
}GameType;

typedef struct GameState GameState;

typedef enum GameResult
{
    GAME_CONTINUE,
    GAME_TO_MENU,
    GAME_RESTART_WON,
    GAME_RESTART_LOST
}GameResult;

GameState* CreateGame(GameType type, int streak);

void DestroyGame(GameState* game);

GameResult GameEvent(GameState* game, SDL_Event* event);

void RenderGame(GameState* game);

int GetStreak(GameState* game);

GameType GetGameType(GameState* game);