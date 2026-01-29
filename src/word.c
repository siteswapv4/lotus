#include "word.h"

static char* words_file = NULL;
static int num_words = 0;
static char** words = NULL;
static int daily_word_index = 0;

void LoadWords()
{
    char* path = NULL;
    SDL_asprintf(&path, "%s/data/words.txt", SDL_GetBasePath());
    words_file = SDL_LoadFile(path, NULL);
    SDL_free(path);
    
    SDL_sscanf(words_file, "%d", &num_words);
    words = SDL_malloc(num_words * sizeof(char*));
    char* start = SDL_strchr(words_file, '\n') + 1;
    char* end = SDL_strchr(start, '\n');

    for (int i = 0; i < num_words; i++)
    {
        words[i] = start;
        if (end)
        {
            *end = '\0';
            start = end + 1;
            end = SDL_strchr(start, '\n');
        }
    }
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
    
    daily_word_index = SDL_rand(num_words);
}

bool WordExists(const char* word)
{
    int left = 0;
    int right = num_words - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        int cmp = SDL_strcmp(word, words[mid]);

        if (cmp == 0)
            return true;
        else if (cmp < 0)
            right = mid - 1;
        else
            left = mid + 1;
    }

    return false;
}

void InitWords()
{
    if (words_file) return;

    LoadWords();
    SetDailyWord();
}

const char* GetDailyWord()
{
    return words[daily_word_index];
}

const char* GetRandomWord()
{
    return words[SDL_rand(num_words)];
}

void QuitWords()
{
    if (!words_file) return;

    SDL_free(words_file);
    words_file = NULL;
}