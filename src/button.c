#include "button.h"

typedef struct Button
{
    char* text;
    size_t text_length;

    SDL_FRect rect;
}Button;

Button* CreateButton(const char* text, const SDL_FRect* rect)
{
    Button* button = SDL_calloc(1, sizeof(Button));

    button->text = SDL_strdup(text);
    button->text_length = SDL_strlen(button->text);
    button->rect = *rect;
    
    return button;
}

void DestroyButton(Button* button)
{
    if (!button) return;
    if (button->text) SDL_free(button->text);
    SDL_free(button);
}

bool ButtonClicked(const Button* button, const SDL_FPoint* position)
{
    return SDL_PointInRectFloat(position, &button->rect);
}

void RenderButton(const Button* button)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &button->rect);
    SDL_RenderDebugText(renderer,
                        button->rect.x + button->rect.w / 2 - button->text_length * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE / 2,
                        button->rect.y + button->rect.h / 2 - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE / 2,
                        button->text);
}

const char* GetButtonText(const Button* button)
{
    return button->text;
}