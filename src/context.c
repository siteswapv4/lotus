#include "context.h"
#include "word.h"

const SDL_Color BACKGROUND_COLOR = {0x2E, 0x00, 0x6C, 255};

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool InitContext()
{
    if (window) return false;

    EXPECT(SDL_Init(SDL_INIT_VIDEO), "%s", SDL_GetError());
    EXPECT(SDL_CreateWindowAndRenderer("Lotus", 450, 800, SDL_WINDOW_RESIZABLE, &window, &renderer), "%s", SDL_GetError());
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
#if defined(SDL_PLATFORM_ANDROID)
    SDL_SetWindowFullscreen(window, true);
#endif

    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "0");
    SDL_StartTextInput(window);
    
    InitWords();
    SDL_srand(0);

    return true;

error:
    return false;
}

void SetRenderDrawColor(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void QuitContext()
{
    if (!window) return;

    QuitWords();
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
}