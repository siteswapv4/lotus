#include "checkbox.h"

typedef struct Checkbox
{
    bool checked;
    char* text;
    size_t text_length;
    SDL_FPoint position;
    SDL_FRect rect;
}Checkbox;

Checkbox* CreateCheckbox(const char* text, const SDL_FPoint* position, bool checked)
{
    Checkbox* checkbox = SDL_calloc(1, sizeof(Checkbox));

    checkbox->text = SDL_strdup(text);
    checkbox->text_length = SDL_strlen(text);
    checkbox->checked = checked;
    checkbox->position = (SDL_FPoint){position->x - (checkbox->text_length + 1) * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE / 2 - 2.5f,
                                      position->y - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE / 2.0f};

    checkbox->rect = (SDL_FRect){checkbox->position.x + checkbox->text_length * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + 5.0f,
                                 checkbox->position.y,
                                 SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE, 
                                 SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE};

    return checkbox;
}

void DestroyCheckbox(Checkbox* checkbox)
{
    if (!checkbox) return;

    if (checkbox->text) SDL_free(checkbox->text);
    SDL_free(checkbox);
}

bool CheckboxClicked(Checkbox* checkbox, const SDL_FPoint* position)
{
    if (SDL_PointInRectFloat(position, &checkbox->rect))
    {
        checkbox->checked = !checkbox->checked;
        return true;
    }
    return false;
}

bool CheckboxChecked(const Checkbox* checkbox)
{
    return checkbox->checked;
}

void RenderCheckbox(const Checkbox* checkbox)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, checkbox->position.x, checkbox->position.y, checkbox->text);
    SDL_RenderRect(renderer, &checkbox->rect);
    if (checkbox->checked)
    {
        SDL_RenderLine(renderer, checkbox->rect.x, checkbox->rect.y, checkbox->rect.x + checkbox->rect.w - 1, checkbox->rect.y + checkbox->rect.h - 1);
        SDL_RenderLine(renderer, checkbox->rect.x + checkbox->rect.w - 1, checkbox->rect.y, checkbox->rect.x, checkbox->rect.y + checkbox->rect.h - 1);
    }
}