#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* All strings must be ASCII */

#define EXPECT(condition, ...) \
    do { \
        if (!(condition)) { \
            SDL_Log(__VA_ARGS__); \
            goto error; \
        } \
    }while (0)

#define WINDOW_WIDTH 250
#define WINDOW_HEIGHT 250

#define BUTTON_MAX 10
#define WORD_MAX 255
#define TRY_MAX 20

#define FONT_SIZE SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE

static const SDL_Color RIGHT_COLOR   = {255, 0, 0, 255};
static const SDL_Color PRESENT_COLOR = {255, 150, 0, 255};
static const SDL_Color EMPTY_COLOR   = {0, 0, 0, 255};

char* WORD_FILE = NULL;
int NUM_WORDS = 0;
char** WORDS = NULL;

int DAILY_WORD_INDEX = 0;

typedef struct Cell
{
    char letter;
    SDL_Color color;
}Cell;

typedef enum AppPhase
{
    APP_PHASE_MENU,
    APP_PHASE_GAME
}AppPhase;

typedef struct Button
{
    char text[WORD_MAX];
    int text_length;
    SDL_FRect rect;
    void* userdata;
    void (*callback)(void*);
}Button;

typedef struct MenuState
{
    Button buttons[BUTTON_MAX];
    int num_buttons;
}MenuState;

typedef enum GameType
{
    GAME_ENDLESS,
    GAME_DAILY
}GameType;

typedef struct GameState
{
    GameType type;
    
    char word[WORD_MAX];
    
    int word_length;
    int num_tries;
    
    int current_try;
    int current_letter;
    
    Cell grid[TRY_MAX][WORD_MAX];
    Uint64 solved_time;
    
    SDL_Texture* grid_texture;
    
    bool ended;
    bool won;
    
    Button end_button;
    Button back_button;
    
    int streak;
}GameState;

typedef struct AppState
{
    AppPhase phase;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    MenuState menu;
    GameState game;
}AppState;

void Retry(void* userdata);

void Quit(void* userdata);

void SwitchToDailyGame(void* userdata);

void SwitchToEndlessGame(void* userdata);

Button NewButton(char* text, SDL_FRect rect, void (*callback)(void*), void* userdata)
{
    Button button = {0};
    SDL_snprintf(button.text, WORD_MAX, "%s", text);
    button.text_length = SDL_strlen(button.text);
    button.rect = rect;
    button.callback = callback;
    button.userdata = userdata;
    
    return button;
}

void InitMenu(AppState* app)
{
    SDL_zero(app->menu);
    
    SDL_SetRenderLogicalPresentation(app->renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_FRect rect = {0.0f, 0.0f, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 10};
    rect.x = WINDOW_WIDTH / 2 - rect.w / 2;
    rect.y = WINDOW_WIDTH / 2 - rect.h / 2 + 5.0f;
    app->menu.buttons[0] = NewButton("Mot du jour", rect, SwitchToDailyGame, app);
    rect.y += 50.0f;
    app->menu.buttons[1] = NewButton("Infini", rect, SwitchToEndlessGame, app);
    app->menu.num_buttons = 2;
    
    app->phase = APP_PHASE_MENU;
}

void SetWord(AppState* app)
{
    if (app->game.type == GAME_DAILY)
        SDL_snprintf(app->game.word, WORD_MAX, "%s", WORDS[DAILY_WORD_INDEX]);
    else
        SDL_snprintf(app->game.word, WORD_MAX, "%s", WORDS[SDL_rand(NUM_WORDS)]);
}

void InitGame(AppState* app, GameType type, int streak)
{
    if (app->game.grid_texture) SDL_DestroyTexture(app->game.grid_texture);
    SDL_zero(app->game);
    
    app->game.streak = streak;
    app->game.type = type;
    SetWord(app);
    app->game.word_length = SDL_strlen(app->game.word);
    app->game.num_tries = 6;
    
    for (int i = 0; i < app->game.num_tries; i++)
    {
        for (int j = 0; j < app->game.word_length; j++)
        {
            app->game.grid[i][j].color = EMPTY_COLOR;
        }
    }
    
    app->game.grid[0][0] = (Cell){app->game.word[0], RIGHT_COLOR};
    app->game.current_letter++;
    
    app->game.grid_texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, app->game.word_length * (FONT_SIZE + 2) - 1, app->game.num_tries * (FONT_SIZE + 2) - 1);
    SDL_SetTextureScaleMode(app->game.grid_texture, SDL_SCALEMODE_NEAREST);
    
    SDL_FRect rect = {0.0f, 0.0f, 70.0f, 15.0f};
    rect.x = WINDOW_WIDTH / 2.0f - rect.w / 2.0f;
    rect.y = WINDOW_HEIGHT - rect.h - 6.0f;
    app->game.back_button = NewButton("<", (SDL_FRect){2.0f, 2.0f, 20.0f, 20.0f}, Quit, app);
    if (type == GAME_ENDLESS)
    {
        app->game.end_button = NewButton("Suivant", rect, Retry, app);
    }
    
    app->phase = APP_PHASE_GAME;
}

void Retry(void* userdata)
{
    AppState* app = userdata;
    InitGame(app, app->game.type, app->game.won ? app->game.streak + 1 : 0);
}

void Quit(void* userdata)
{
    InitMenu(userdata);
}

void SwitchToDailyGame(void* userdata)
{
    InitGame(userdata, GAME_DAILY, 0);
}

void SwitchToEndlessGame(void* userdata)
{
    InitGame(userdata, GAME_ENDLESS, 0);
}

void SetDailyWord()
{
    SDL_Time time;
    SDL_DateTime date_time;
    SDL_GetCurrentTime(&time);
    SDL_TimeToDateTime(time, &date_time, true);
    SDL_srand(SDL_GetDayOfYear(date_time.year, date_time.month, date_time.day) + date_time.year + 1);
    
    for (int i = 0; i < 10; i++)
        SDL_rand(100);
    
    DAILY_WORD_INDEX = SDL_rand(NUM_WORDS);
}

void LoadWords(AppState* app)
{
    char* path = NULL;
    SDL_asprintf(&path, "%s/words.txt", SDL_GetBasePath());
    WORD_FILE = SDL_LoadFile(path, NULL);
    SDL_free(path);
    
    SDL_sscanf(WORD_FILE, "%d", &NUM_WORDS);
    WORDS = SDL_malloc(NUM_WORDS * sizeof(char*));
    char* start = SDL_strchr(WORD_FILE, '\n') + 1;
    char* end = SDL_strchr(start, '\n');
    
    for (int i = 0; i < NUM_WORDS; i++)
    {
        WORDS[i] = start;
        if (end)
        {
            *end = '\0';
            start = end + 1;
            end = SDL_strchr(start, '\n');
        }
    }
}

bool WordExists(const char* word)
{
    int left = 0;
    int right = NUM_WORDS - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        int cmp = SDL_strcmp(word, WORDS[mid]);

        if (cmp == 0)
            return true;
        else if (cmp < 0)
            right = mid - 1;
        else
            left = mid + 1;
    }

    return false;
}

void WordFromGridLine(AppState* app, int index, char word[WORD_MAX])
{
    for (int i = 0; i < app->game.word_length; i++)
    {
        word[i] = app->game.grid[index][i].letter;
        word[i + 1] = '\0';
    }
}

SDL_AppResult SDL_AppInit(void** userdata, int argc, char* argv[])
{
    AppState* app = SDL_calloc(1, sizeof(AppState));
    *userdata = app;
    
    EXPECT(SDL_Init(SDL_INIT_VIDEO), "%s", SDL_GetError());
    EXPECT(SDL_CreateWindowAndRenderer("Lotus", 800, 800, SDL_WINDOW_RESIZABLE, &app->window, &app->renderer), "%s", SDL_GetError());
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
    SDL_StartTextInput(app->window);
    
    LoadWords(app);
    
    SetDailyWord();
    
    SDL_srand(0);
    
    InitMenu(app);

    return SDL_APP_CONTINUE;

error:
    return SDL_APP_FAILURE;
}

bool Solved(AppState* app, const Cell* solution)
{
    for (int i = 0; i < app->game.word_length; i++)
    {
        if (solution[i].letter != app->game.word[i])
            return false;
    }
    return true;
}

void Evaluate(AppState* app)
{
    int occurences[26] = {0};
    
    for (char* it = app->game.word; *it; it++)
    {
        occurences[(*it) - 'A']++;
    }
    
    for (int i = 0; i < app->game.word_length; i++)
    {
        char letter = app->game.grid[app->game.current_try][i].letter;
        int index = letter - 'A';
        
        if (occurences[index] > 0)
        {
            if (app->game.word[i] == letter)
                app->game.grid[app->game.current_try][i].color = RIGHT_COLOR;
            else
                app->game.grid[app->game.current_try][i].color = PRESENT_COLOR;
                
            occurences[index]--;
        }
    }

    app->game.current_try++;
    app->game.current_letter = 0;
    
    if (Solved(app, app->game.grid[app->game.current_try - 1]))
    {
        app->game.ended = true;
        app->game.won = true;
    }
    else if (app->game.current_try == app->game.num_tries)
    {
        app->game.ended = true;
        app->game.won = false;
    }
    else
    {
        app->game.grid[app->game.current_try][0].letter = app->game.word[0];
        app->game.grid[app->game.current_try][0].color = RIGHT_COLOR;
        app->game.current_letter++;
    }
}

void HandleButtonClick(AppState* app, const Button* button, SDL_FPoint position)
{
    if (SDL_PointInRectFloat(&position, &button->rect))
    {
        if (button->callback)
        {
            button->callback(button->userdata);
        }
    }
}

SDL_AppResult SDL_AppEvent(void* userdata, SDL_Event* event)
{
    AppState* app = userdata;
    SDL_ConvertEventToRenderCoordinates(app->renderer, event);
    
    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    
    if (event->type == SDL_EVENT_TEXT_INPUT)
    {
        if (app->phase == APP_PHASE_GAME && !app->game.ended)
        {
            if (app->game.current_letter < app->game.word_length)
            {
                char letter = event->text.text[0];
                if (((letter >= 'A') && (letter <= 'Z')) || ((letter >= 'a') && (letter <= 'z')))
                {
                    if ((letter >= 'a') && (letter <= 'z'))
                        letter -= 'a' - 'A';
                        
                    if ((app->game.current_letter != 1) || (app->game.grid[app->game.current_try][0].letter != letter))
                    {
                        app->game.grid[app->game.current_try][app->game.current_letter].letter = letter;
                        app->game.current_letter++;
                    }
                }
            }
        }
    }
    else if ((event->type == SDL_EVENT_KEY_DOWN) && (!event->key.repeat))
    {
        if (event->key.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if (app->phase == APP_PHASE_GAME && !app->game.ended)
            {
                if (app->game.current_letter > 1)
                {
                    app->game.current_letter--;
                    app->game.grid[app->game.current_try][app->game.current_letter].letter = '\0';
                }
            }
        }
        else if (event->key.scancode == SDL_SCANCODE_RETURN)
        {
            if ((app->phase == APP_PHASE_GAME) && !app->game.ended)
            {
                if (app->game.current_letter == app->game.word_length)
                {
                    char word[WORD_MAX];
                    WordFromGridLine(app, app->game.current_try, word);
                    if (WordExists(word))
                    {
                        Evaluate(app);
                    }
                }
            }
        }
        else if (event->key.scancode == SDL_SCANCODE_F11)
        {
            SDL_SetWindowFullscreen(app->window, !(SDL_GetWindowFlags(app->window) & SDL_WINDOW_FULLSCREEN));
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        SDL_FPoint position = {event->button.x, event->button.y};
        
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (app->phase == APP_PHASE_MENU)
            {
                for (int i = 0; i < app->menu.num_buttons; i++)
                {
                    HandleButtonClick(app, &app->menu.buttons[i], position);
                }
            }
            else if (app->phase == APP_PHASE_GAME)
            {
                HandleButtonClick(app, &app->game.back_button, position);
                if (app->game.ended && app->game.type == GAME_ENDLESS)
                    HandleButtonClick(app, &app->game.end_button, position);
            }
        }
    }
    
    return SDL_APP_CONTINUE;
}

void SetRenderColor(SDL_Renderer* renderer, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void RenderButton(AppState* app, const Button* button)
{
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderRect(app->renderer, &button->rect);
    SDL_RenderDebugText(app->renderer, button->rect.x + button->rect.w / 2 - button->text_length * FONT_SIZE / 2, button->rect.y + button->rect.h / 2 - FONT_SIZE / 2, button->text);
}

void RenderMenu(AppState* app)
{
    SDL_SetRenderScale(app->renderer, 2.0f, 2.0f);
    SDL_SetRenderDrawColor(app->renderer, 255, 50, 50, 255);
    SDL_RenderDebugText(app->renderer, WINDOW_WIDTH / 4.0 - 5 * FONT_SIZE / 2, 30.0f, "LOTUS");
    SDL_SetRenderScale(app->renderer, 1.0f, 1.0f);
    for (int i = 0; i < app->menu.num_buttons; i++)
    {
        RenderButton(app, &app->menu.buttons[i]);
    }
}

void RenderGrid(AppState* app)
{
    float ratio = app->game.grid_texture->w / (float)app->game.grid_texture->h;

    // Maximum allowed width and height
    float max_w = (float)WINDOW_WIDTH;
    float max_h = (float)WINDOW_HEIGHT - 60.0f;

    // Start with width = max_w, compute height from aspect ratio
    float w = max_w;
    float h = w / ratio;

    // If height exceeds max_h, scale down to max_h
    if (h > max_h) {
        h = max_h;
        w = h * ratio; // adjust width to maintain aspect ratio
    }

    // Center the rect
    SDL_FRect rect;
    rect.w = w;
    rect.h = h;
    rect.x = (WINDOW_WIDTH - w) / 2.0f;
    rect.y = (WINDOW_HEIGHT - h) / 2.0f;

    // Render
    SDL_RenderTexture(app->renderer, app->game.grid_texture, NULL, &rect);
}

void RenderGame(AppState* app)
{
    SDL_SetRenderTarget(app->renderer, app->game.grid_texture);
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(app->renderer, NULL);
    
    for (int i = 0; i < app->game.num_tries; i++)
    {
        for (int j = 0; j < app->game.word_length; j++)
        {
            SDL_FRect rect = {j * FONT_SIZE + (2 * j), i * FONT_SIZE + (2 * i), FONT_SIZE + 1, FONT_SIZE + 1};
            SetRenderColor(app->renderer, app->game.grid[i][j].color);
            SDL_RenderFillRect(app->renderer, &rect);
            
            SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
            SDL_RenderDebugText(app->renderer, j * FONT_SIZE + (2 * j + 1), i * FONT_SIZE + (2 * i + 1), (char[]){app->game.grid[i][j].letter, '\0'});
        }
    }
    
    SDL_SetRenderTarget(app->renderer, NULL);
    
    RenderGrid(app);
    
    RenderButton(app, &app->game.back_button);
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    if (app->game.type == GAME_ENDLESS)
        SDL_RenderDebugTextFormat(app->renderer, 0.0f, WINDOW_HEIGHT - FONT_SIZE, "streak %d", app->game.streak);
    
    if (app->game.ended)
    {
        if (app->game.type == GAME_ENDLESS)
        {
            RenderButton(app, &app->game.end_button);
        }
        if (!app->game.won)
        {
            SDL_SetRenderDrawColor(app->renderer, 255, 50, 50, 255);
            SDL_RenderDebugText(app->renderer, WINDOW_WIDTH / 2.0f - app->game.word_length * FONT_SIZE / 2.0f, 15.0f - FONT_SIZE / 2.0f, app->game.word);
        }
    }
}

SDL_AppResult SDL_AppIterate(void* userdata)
{
    AppState* app = userdata;
    
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    
    if (app->phase == APP_PHASE_MENU)
        RenderMenu(app);
    else if (app->phase == APP_PHASE_GAME)
        RenderGame(app);
    
    SDL_RenderPresent(app->renderer);
    
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* userdata, SDL_AppResult result)
{
    if (WORD_FILE) SDL_free(WORD_FILE);
    if (WORDS) SDL_free(WORDS);
    if (userdata) SDL_free(userdata);
}
