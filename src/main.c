#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "context.h"
#include "menu.h"
#include "game.h"

/* All strings must be ASCII */

const SDL_Color BACKGROUND_COLOR = {0x2E, 0x00, 0x6C, 255};

typedef enum AppPhase
{
    APP_PHASE_MENU,
    APP_PHASE_GAME
}AppPhase;

typedef struct AppState
{
    AppPhase phase;
    
    MenuState* menu;
    GameState* game;
}AppState;

SDL_AppResult SDL_AppInit(void** userdata, int argc, char* argv[])
{
    AppState* app = SDL_calloc(1, sizeof(AppState));
    *userdata = app;

    EXPECT(InitContext(), "Failed to init context");
    app->menu = CreateMenu();
    app->phase = APP_PHASE_MENU;

    return SDL_APP_CONTINUE;

error:
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void* userdata, SDL_Event* event)
{
    AppState* app = userdata;
    SDL_ConvertEventToRenderCoordinates(renderer, event);
    
    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

    if ((event->type == SDL_EVENT_KEY_DOWN) && (!event->key.repeat))
    {
        if (event->key.scancode == SDL_SCANCODE_F11)
        {
            SDL_SetWindowFullscreen(window, !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
        }
    }

    if (app->phase == APP_PHASE_MENU)
    {
        MenuResult result = MenuEvent(app->menu, event);
        if (result != MENU_CONTINUE)
        {
            DestroyMenu(app->menu);
            app->menu = NULL;
            app->game = CreateGame(result == MENU_TO_DAILY ? GAME_DAILY : GAME_ENDLESS, 0);
            app->phase = APP_PHASE_GAME;
        }
    }
    else if (app->phase == APP_PHASE_GAME)
    {
        GameResult result = GameEvent(app->game, event);
        if (result == GAME_TO_MENU)
        {
            DestroyGame(app->game);
            app->game = NULL;
            app->menu = CreateMenu();
            app->phase = APP_PHASE_MENU;
        }
        else if (result != GAME_CONTINUE)
        {
            GameType type = GetGameType(app->game);
            int streak = GetStreak(app->game);
            DestroyGame(app->game);
            app->game = CreateGame(type, (result == GAME_RESTART_WON ? streak + 1 : 0));
        }
    }
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* userdata)
{
    AppState* app = userdata;
    
    SetRenderDrawColor(BACKGROUND_COLOR);
    SDL_RenderClear(renderer);
    
    switch (app->phase)
    {
        case APP_PHASE_MENU:
            RenderMenu(app->menu);
            break;

        case APP_PHASE_GAME:
            RenderGame(app->game);
            break;

        default:
            break;
    }
    
    SDL_RenderPresent(renderer);
    
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* userdata, SDL_AppResult result)
{
    AppState* app = userdata;

    if (!app) return;
    if (app->menu) DestroyMenu(app->menu);
    if (app->game) DestroyGame(app->game);

    DestroyContext();

    SDL_free(app);
}