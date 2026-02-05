#include "menu.h"

#include "button.h"
#include "context.h"

typedef struct MenuState
{
    Button* daily_button;
    Button* endless_button;
}MenuState;

MenuState* CreateMenu()
{
    MenuState* menu = SDL_calloc(1, sizeof(MenuState));

    SDL_FRect rect = {0.0f, 0.0f, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 10};
    rect.x = WINDOW_WIDTH / 2 - rect.w / 2;
    rect.y = WINDOW_HEIGHT / 2 - rect.h / 2;
    menu->daily_button = CreateButton("Mot du jour", &rect, &BACKGROUND_COLOR);
    rect.y += 75.0f;
    menu->endless_button = CreateButton("Infini", &rect, &BACKGROUND_COLOR);  
    
    return menu;
}

void DestroyMenu(MenuState* menu)
{
    if (!menu) return;
    if (menu->daily_button) DestroyButton(menu->daily_button);
    if (menu->endless_button) DestroyButton(menu->endless_button);
    SDL_free(menu);
}

MenuResult MenuEvent(MenuState* menu, SDL_Event* event)
{
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        SDL_FPoint position = {event->button.x, event->button.y};

        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (ButtonClicked(menu->daily_button, &position))
                return MENU_TO_DAILY;

            if (ButtonClicked(menu->endless_button, &position))
                return MENU_TO_ENDLESS;
        }
    }

    return MENU_CONTINUE;
}

void RenderMenu(MenuState* menu)
{
    SDL_SetRenderScale(renderer, 2.0f, 2.0f);
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    SDL_RenderDebugText(renderer, WINDOW_WIDTH / 4.0 - 5 * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE / 2, 50.0f, "LOTUS");
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

    RenderButton(menu->daily_button);
    RenderButton(menu->endless_button);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 0.0f, WINDOW_HEIGHT - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE, "siteswapv4");
}