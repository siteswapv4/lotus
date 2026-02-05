#pragma once
#include <SDL3/SDL.h>

#define EXPECT(condition, ...) \
    do { \
        if (!(condition)) { \
            SDL_Log(__VA_ARGS__); \
            goto error; \
        } \
    }while (0)

#define WINDOW_WIDTH 225
#define WINDOW_HEIGHT 400

extern const SDL_Color BACKGROUND_COLOR;

/* Valid for the lifetime of the app */
extern SDL_Window* window;
extern SDL_Renderer* renderer;

bool InitContext();

void SetRenderDrawColor(SDL_Color color);

void QuitContext();