#pragma once
#include <SDL3/SDL.h>

#include "context.h"

typedef struct Button Button;

Button* CreateButton(const char* text, const SDL_FRect* rect);

void DestroyButton(Button* button);

bool ButtonClicked(const Button* button, const SDL_FPoint* position);

void RenderButton(const Button* button);

const char* GetButtonText(const Button* button);