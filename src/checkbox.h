#pragma once
#include <SDL3/SDL.h>

#include "context.h"

typedef struct Checkbox Checkbox;

Checkbox* CreateCheckbox(const char* text, const SDL_FPoint* position, bool checked);

void DestroyCheckbox(Checkbox* checkbox);

bool CheckboxClicked(Checkbox* checkbox, const SDL_FPoint* position);

bool CheckboxChecked(const Checkbox* checkbox);

void RenderCheckbox(const Checkbox* checkbox);