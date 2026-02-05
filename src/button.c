#include "button.h"

typedef struct Button
{
    char* text;
    size_t text_length;
    SDL_Color color;

    SDL_FRect rect;
}Button;

Button* CreateButton(const char* text, const SDL_FRect* rect, const SDL_Color* color)
{
    Button* button = SDL_calloc(1, sizeof(Button));

    button->text = SDL_strdup(text);
    button->text_length = SDL_strlen(button->text);
    button->rect = *rect;
    button->color = *color;
    
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
    SDL_SetRenderDrawColor(renderer, button->color.r, button->color.g, button->color.b, button->color.a);
    SDL_RenderFillRect(renderer, &button->rect);
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

void SetButtonColor(Button* button, const SDL_Color* color)
{
    button->color = *color;
}

SDL_Color GetButtonColor(const Button* button)
{
    return button->color;
}