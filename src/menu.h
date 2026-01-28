#pragma once
#include <SDL3/SDL.h>

typedef struct MenuState MenuState;

typedef enum MenuResult
{
    MENU_CONTINUE,
    MENU_TO_DAILY,
    MENU_TO_ENDLESS
}MenuResult;

MenuState* CreateMenu();

void DestroyMenu(MenuState* menu);

MenuResult MenuEvent(MenuState* menu, SDL_Event* event);

void RenderMenu(MenuState* menu);