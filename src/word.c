#include "word.h"

static char* WORD_FILE = NULL;

int NUM_WORDS = 0;
char** WORDS = NULL;
int DAILY_WORD_INDEX = 0;

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

void LoadWords()
{
    char* path = NULL;
    SDL_asprintf(&path, "%s/data/words.txt", SDL_GetBasePath());
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

void DestroyWords()
{
    if (WORD_FILE) SDL_free(WORD_FILE);
    WORD_FILE = NULL;
}