#include "menu.h"

#include "word.h"
#include "button.h"
#include "checkbox.h"
#include "context.h"

typedef enum MenuPhase
{
    MENU_PHASE_HOMEPAGE,
    MENU_PHASE_ENDLESS_SETTINGS
}MenuPhase;

typedef struct MenuState
{
    MenuPhase phase; 
    Button* daily_button;
    Button* endless_button;

    Checkbox* seven_letters;
    Checkbox* eight_letters;
    Checkbox* nine_letters;

    Button* back_button;
    Button* confirm_button;
}MenuState;

bool AtLeastOneChecked(MenuState* menu)
{
    if (CheckboxChecked(menu->seven_letters)) return true;
    if (CheckboxChecked(menu->eight_letters)) return true;
    if (CheckboxChecked(menu->nine_letters)) return true;
    return false;
}

MenuState* CreateMenu()
{
    MenuState* menu = SDL_calloc(1, sizeof(MenuState));

    menu->phase = MENU_PHASE_HOMEPAGE;

    EnableSevenLetterWords(true);
    EnableEightLetterWords(true);
    EnableNineLetterWords(true);

    SDL_FRect rect = {0.0f, 0.0f, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 10};
    rect.x = WINDOW_WIDTH / 2 - rect.w / 2;
    rect.y = WINDOW_HEIGHT / 2 - rect.h / 2;
    menu->daily_button = CreateButton("Mot du jour", &rect, &BACKGROUND_COLOR);
    rect.y += 75.0f;
    menu->endless_button = CreateButton("Infini", &rect, &BACKGROUND_COLOR);

    SDL_FPoint position = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 20.0f};
    menu->seven_letters = CreateCheckbox("7 lettres", &position, true);
    position.y += 20.0f;
    menu->eight_letters = CreateCheckbox("8 lettres", &position, true);
    position.y += 20.0f;
    menu->nine_letters = CreateCheckbox("9 lettres", &position, true);

    menu->back_button = CreateButton("<", &(SDL_FRect){5.0f, 5.0f, 20.0f, 20.0f}, &BACKGROUND_COLOR);
    menu->confirm_button = CreateButton("Suivant", &rect, &BACKGROUND_COLOR);
    
    return menu;
}

void DestroyMenu(MenuState* menu)
{
    if (!menu) return;
    if (menu->daily_button) DestroyButton(menu->daily_button);
    if (menu->endless_button) DestroyButton(menu->endless_button);
    if (menu->back_button) DestroyButton(menu->back_button);
    if (menu->confirm_button) DestroyButton(menu->confirm_button);
    if (menu->seven_letters) DestroyCheckbox(menu->seven_letters);
    if (menu->eight_letters) DestroyCheckbox(menu->eight_letters);
    if (menu->nine_letters) DestroyCheckbox(menu->nine_letters);
    SDL_free(menu);
}

MenuResult MenuEvent(MenuState* menu, SDL_Event* event)
{
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        SDL_FPoint position = {event->button.x, event->button.y};

        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (menu->phase == MENU_PHASE_HOMEPAGE)
            {
                if (ButtonClicked(menu->daily_button, &position))
                    return MENU_TO_DAILY;

                if (ButtonClicked(menu->endless_button, &position))
                    menu->phase = MENU_PHASE_ENDLESS_SETTINGS;
            }
            else if (menu->phase == MENU_PHASE_ENDLESS_SETTINGS)
            {
                if (ButtonClicked(menu->back_button, &position))
                    menu->phase = MENU_PHASE_HOMEPAGE;

                if (ButtonClicked(menu->confirm_button, &position))
                {
                    if (AtLeastOneChecked(menu))
                        return MENU_TO_ENDLESS;
                }

                if (CheckboxClicked(menu->seven_letters, &position))
                    EnableSevenLetterWords(CheckboxChecked(menu->seven_letters));

                if (CheckboxClicked(menu->eight_letters, &position))
                    EnableEightLetterWords(CheckboxChecked(menu->eight_letters));

                if (CheckboxClicked(menu->nine_letters, &position))
                    EnableNineLetterWords(CheckboxChecked(menu->nine_letters));
            }
        }
    }
    else if ((event->type == SDL_EVENT_KEY_DOWN) && (!event->key.repeat))
    {
        if (event->key.scancode == SDL_SCANCODE_RETURN)
        {
            if (menu->phase == MENU_PHASE_ENDLESS_SETTINGS)
            {
                if (AtLeastOneChecked(menu))
                    return MENU_TO_ENDLESS;
            }
        }
        else if (event->key.scancode == SDL_SCANCODE_ESCAPE)
        {
            if (menu->phase == MENU_PHASE_ENDLESS_SETTINGS)
                menu->phase = MENU_PHASE_HOMEPAGE;
        }
    }

    return MENU_CONTINUE;
}

void RenderMenu(MenuState* menu)
{
    if (menu->phase == MENU_PHASE_HOMEPAGE)
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
    else if (menu->phase == MENU_PHASE_ENDLESS_SETTINGS)
    {
        RenderButton(menu->back_button);
        RenderButton(menu->confirm_button);
        RenderCheckbox(menu->seven_letters);
        RenderCheckbox(menu->eight_letters);
        RenderCheckbox(menu->nine_letters);
    }
}