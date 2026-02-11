#include "game.h"
#include "button.h"
#include "context.h"
#include "word.h"

#define NUM_TRIES 6

#define NUM_KEYBOARD_BUTTONS 28
static const char KEYBOARD_BUTTONS_CHAR[NUM_KEYBOARD_BUTTONS] = {
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z',
    'X', 'C', 'V', 'B', 'N', 'M', '<', '>'
};

typedef enum LetterType
{
    LETTER_PRESENT,
    LETTER_RIGHT,
    LETTER_WRONG
}LetterType;

const SDL_Color COLORS[3] = {{255, 150, 0, 255}, {255, 0, 0, 255}, {150, 150, 150, 255}};
const SDL_Color EMPTY_COLOR      = {0x2E, 0x00, 0x6C, 255};

const int KEYBOARD_INDICES[26] = {10, 23, 21, 12, 2, 13, 14, 15, 7, 16, 17, 18, 25, 24, 8, 9, 0, 3, 11, 4, 6, 22, 1, 20, 5, 19};

typedef struct Cell
{
    char letter;
    SDL_Color color;
}Cell;

typedef struct GameState
{
    GameType type;
    
    const char* (*get_word)(void);
    size_t word_length;

    int num_tries;
    
    Cell** grid;
    int current_try;
    int current_letter;
    
    SDL_Texture* grid_texture;
    
    bool ended;
    bool won;
    
    Button* end_button;
    Button* back_button;
    Button* forfeit_button;
    
    Button* buttons[NUM_KEYBOARD_BUTTONS];
    
    int streak;
}GameState;

char* WordFromGridLine(GameState* game, int index)
{
    char* word = SDL_malloc(game->word_length + 1);
    for (int i = 0; i < game->word_length; i++)
    {
        word[i] = game->grid[index][i].letter;
        word[i + 1] = '\0';
    }
    return word;
}

bool Solved(GameState* game, const Cell* solution)
{
    for (int i = 0; i < game->word_length; i++)
    {
        if (solution[i].letter != game->get_word()[i])
            return false;
    }
    return true;
}

bool ColorEquals(const SDL_Color* c1, const SDL_Color* c2)
{
    return (c1->r == c2->r) && (c1->g == c2->g) && (c1->b == c2->b) && (c1->a == c2->a);
}

void SetKeyColor(GameState* game, char letter, LetterType type)
{
    int index = letter - 'A';

    Button* button = game->buttons[KEYBOARD_INDICES[index]];
    SDL_Color color = GetButtonColor(button);

    if (ColorEquals(&color, &COLORS[LETTER_RIGHT])) return;
    if (((ColorEquals(&color, &COLORS[LETTER_RIGHT])) || (ColorEquals(&color, &COLORS[LETTER_PRESENT]))) && (type == LETTER_WRONG)) return;

    SetButtonColor(button, &COLORS[type]);
}

void Evaluate(GameState* game)
{
    int occurences[26] = {0};
    
    for (const char* it = game->get_word(); *it; it++)
    {
        occurences[(*it) - 'A']++;
    }
    
    for (int i = 0; i < game->word_length; i++)
    {
        char letter = game->grid[game->current_try][i].letter;
        int index = letter - 'A';
        
        if (occurences[index] > 0)
        {
            if (game->get_word()[i] == letter)
            {
                game->grid[game->current_try][i].color = COLORS[LETTER_RIGHT];
                SetKeyColor(game, letter, LETTER_RIGHT);
            }
            else
            {
                game->grid[game->current_try][i].color = COLORS[LETTER_PRESENT];
                SetKeyColor(game, letter, LETTER_PRESENT);
            }
                
            occurences[index]--;
        }
        else
        {
            SetKeyColor(game, letter, LETTER_WRONG);
        }
    }

    game->current_try++;
    game->current_letter = 0;
    
    if (Solved(game, game->grid[game->current_try - 1]))
    {
        game->ended = true;
        game->won = true;
    }
    else if (game->current_try == game->num_tries)
    {
        game->ended = true;
        game->won = false;
    }
    else
    {
        game->grid[game->current_try][0].letter = game->get_word()[0];
        game->grid[game->current_try][0].color = COLORS[LETTER_RIGHT];
        game->current_letter++;
    }
}

void AddLetter(GameState* game, char letter)
{
    if (game->ended) return;
    if (game->current_letter >= game->word_length) return;
    if ((game->current_letter == 1) && (game->grid[game->current_try][0].letter == letter)) return;
    
    game->grid[game->current_try][game->current_letter].letter = letter;
    game->current_letter++;
}

void RemoveLetter(GameState* game)
{
    if (game->ended) return;
    if (game->current_letter <= 1) return;
    game->current_letter--;
    game->grid[game->current_try][game->current_letter].letter = '\0';
}

void ConfirmWord(GameState* game)
{    
    if (game->current_letter != game->word_length) return;

    char* word = WordFromGridLine(game, game->current_try);
    if (WordExists(word))
    {
        Evaluate(game);
    }
    SDL_free(word);
}

void ButtonKeyPress(GameState* game, char letter)
{
    if (letter == '<')
        RemoveLetter(game);
    else if (letter == '>')
        ConfirmWord(game);
    else
        AddLetter(game, letter);
}

GameState* CreateGame(GameType type, int streak)
{
    GameState* game = SDL_calloc(1, sizeof(GameState));
    
    game->streak = streak;
    game->type = type;
    
    if (type == GAME_DAILY)
    {
        game->get_word = GetDailyWord;
    }
    else
    {
        NextRandomWord();
        game->get_word = GetRandomWord;
    }
    game->word_length = SDL_strlen(game->get_word());
    
    game->num_tries = NUM_TRIES;
    
    game->grid = SDL_calloc(game->num_tries, sizeof(Cell*));
    for (int i = 0; i < game->num_tries; i++)
    {
        game->grid[i] = SDL_calloc(game->word_length, sizeof(Cell));
        for (int j = 0; j < game->word_length; j++)
        {
            game->grid[i][j].color = EMPTY_COLOR;
        }
    }
    
    game->grid[0][0] = (Cell){game->get_word()[0], COLORS[LETTER_RIGHT]};
    game->current_letter++;
    
    game->grid_texture = SDL_CreateTexture(renderer, 
                                           SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_TARGET,
                                           game->word_length * (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + 2) - 1,
                                           game->num_tries * (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + 2) - 1);

    SDL_SetTextureScaleMode(game->grid_texture, SDL_SCALEMODE_NEAREST);
    
    game->back_button = CreateButton("<", &(SDL_FRect){5.0f, 5.0f, 20.0f, 20.0f}, &BACKGROUND_COLOR);
    if (type == GAME_ENDLESS)
    {
        SDL_FRect rect = {0.0f, 0.0f, 75.0f, 25.0f};
        rect.x = WINDOW_WIDTH / 2.0f - rect.w / 2.0f;
        rect.y = WINDOW_HEIGHT - rect.h - 25.0f;
        game->end_button = CreateButton("Suivant", &rect, &BACKGROUND_COLOR);
    }
    
    float base_height = 260.0f;
    float key_size = 22.0f;
    for (int i = 0; i < 10; i++)
    {
        SDL_FRect rect = {2.0f + key_size * i, base_height, key_size, key_size};
        game->buttons[i] = CreateButton((char[]){KEYBOARD_BUTTONS_CHAR[i], '\0'}, &rect, &BACKGROUND_COLOR);
    }
    for (int i = 10; i < 20; i++)
    {
        SDL_FRect rect = {2.0f + key_size * (i - 10), base_height + key_size, key_size, key_size};
        game->buttons[i] = CreateButton((char[]){KEYBOARD_BUTTONS_CHAR[i], '\0'}, &rect, &BACKGROUND_COLOR);
    }
    for (int i = 20; i < 28; i++)
    {
        SDL_FRect rect = {2.0f + key_size * (i - 19), base_height + key_size * 2.0f, key_size, key_size};
        game->buttons[i] = CreateButton((char[]){KEYBOARD_BUTTONS_CHAR[i], '\0'}, &rect, &BACKGROUND_COLOR);
    }

    game->forfeit_button = CreateButton("FF", &(SDL_FRect){WINDOW_WIDTH - 25.0f, 5.0f, 20.0f, 20.0f}, &BACKGROUND_COLOR);
    
    return game;
}

void DestroyGame(GameState* game)
{
    if (!game) return;

    if (game->grid)
    {
        for (int i = 0; i < game->num_tries; i++)
        {
            if (game->grid[i]) SDL_free(game->grid[i]);
        }
        SDL_free(game->grid);
    }
    if (game->grid_texture) SDL_DestroyTexture(game->grid_texture);
    if (game->end_button) DestroyButton(game->end_button);
    if (game->back_button) DestroyButton(game->back_button);
    if (game->forfeit_button) DestroyButton(game->forfeit_button);
    for (int i = 0; i < NUM_KEYBOARD_BUTTONS; i++)
    {
        if (game->buttons[i]) DestroyButton(game->buttons[i]);
    }
    SDL_free(game);
}

GameResult GameEvent(GameState* game, SDL_Event* event)
{
    if (event->type == SDL_EVENT_TEXT_INPUT)
    {
        char letter = event->text.text[0];
        if (((letter >= 'A') && (letter <= 'Z')) || ((letter >= 'a') && (letter <= 'z')))
        {
            if ((letter >= 'a') && (letter <= 'z'))
                letter -= 'a' - 'A';
                
            AddLetter(game, letter);
        }
    }
    else if ((event->type == SDL_EVENT_KEY_DOWN) && (!event->key.repeat))
    {
        if (event->key.scancode == SDL_SCANCODE_BACKSPACE)
        {
            RemoveLetter(game);
        }
        else if (event->key.scancode == SDL_SCANCODE_RETURN)
        {
            if (game->ended)
            {
                if (game->type == GAME_ENDLESS)
                    return game->won ? GAME_RESTART_WON : GAME_RESTART_LOST;
                else
                    return GAME_TO_MENU;
            }
            ConfirmWord(game);
        }
        else if ((event->key.scancode == SDL_SCANCODE_ESCAPE) || (event->key.scancode == SDL_SCANCODE_AC_BACK))
        {
            return GAME_TO_MENU;
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        SDL_FPoint position = {event->button.x, event->button.y};
        
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (ButtonClicked(game->back_button, &position))
                return GAME_TO_MENU;

            if (game->ended && (game->type == GAME_ENDLESS))
            {
                if (ButtonClicked(game->end_button, &position))
                    return game->won ? GAME_RESTART_WON : GAME_RESTART_LOST;
            }
            for (int i = 0; i < NUM_KEYBOARD_BUTTONS; i++)
            {
                if (ButtonClicked(game->buttons[i], &position))
                    ButtonKeyPress(game, GetButtonText(game->buttons[i])[0]);
            }
            if (ButtonClicked(game->forfeit_button, &position))
            {
                game->ended = true;
                game->won = false;
            }
        }
    }

    return GAME_CONTINUE;
}

void RenderGrid(GameState* game)
{
    float ratio = game->grid_texture->w / (float)game->grid_texture->h;

    float max_w = (float)WINDOW_WIDTH;
    float max_h = (float)WINDOW_HEIGHT;

    float w = max_w;
    float h = w / ratio;

    if (h > max_h) {
        h = max_h;
        w = h * ratio;
    }

    SDL_FRect rect;
    rect.w = w;
    rect.h = h;
    rect.x = (WINDOW_WIDTH - w) / 2.0f;
    rect.y = 50.0f;

    SDL_RenderTexture(renderer, game->grid_texture, NULL, &rect);
}

void RenderGame(GameState* game)
{
    SDL_SetRenderTarget(renderer, game->grid_texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, NULL);
    for (int i = 0; i < game->num_tries; i++)
    {
        for (int j = 0; j < game->word_length; j++)
        {
            SDL_FRect rect = {j * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + (2 * j),
                              i * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + (2 * i),
                              SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + 1,
                              SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + 1};

            SetRenderDrawColor(game->grid[i][j].color);
            SDL_RenderFillRect(renderer, &rect);
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDebugText(renderer,
                                j * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + (2 * j + 1),
                                i * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + (2 * i + 1),
                                (char[]){game->grid[i][j].letter, '\0'});
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
    RenderGrid(game);
    
    RenderButton(game->back_button);

    for (int i = 0; i < NUM_KEYBOARD_BUTTONS; i++)
        RenderButton(game->buttons[i]);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    if (game->type == GAME_ENDLESS)
        SDL_RenderDebugTextFormat(renderer,
                                  0.0f,
                                  WINDOW_HEIGHT - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE,
                                  "streak %d", game->streak);
    
    if (game->ended)
    {
        if (game->type == GAME_ENDLESS)
        {
            RenderButton(game->end_button);
        }
        if (!game->won)
        {
            float scale = 2.0f;
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
            SDL_SetRenderScale(renderer, scale, scale);
            SDL_RenderDebugText(renderer,
                                ((WINDOW_WIDTH / 2.0f) - (game->word_length * (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * scale) / 2.0f)) / scale,
                                (25.0f - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * scale) / 2.0f) / scale,
                                game->get_word());
            SDL_SetRenderScale(renderer, 1.0f, 1.0f);
        }
    }
    else
    {
        RenderButton(game->forfeit_button);
    }
}

int GetStreak(GameState* game)
{
    return game->streak;
}

GameType GetGameType(GameState* game)
{
    return game->type;
}
