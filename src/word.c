#include "word.h"

static char* words_file = NULL;
static int num_words = 0;
static char** words = NULL;
static int daily_word_index = 0;

static int num_seven_letter_words = 0;
static char** seven_letter_words = NULL;

static int num_height_letter_words = 0;
static char** height_letter_words = NULL;

static int num_nine_letter_words = 0;
static char** nine_letter_words = NULL;

static bool seven_letters = true;
static bool height_letters = true;
static bool nine_letters = true;

void EnableSevenLetterWords(bool enable)
{
    seven_letters = enable;
}

void EnableHeightLetterWords(bool enable)
{
    height_letters = enable;
}

void EnableNineLetterWords(bool enable)
{
    nine_letters = enable;
}

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

    /* This takes a few mb at most but could be optimized */
    seven_letter_words = SDL_malloc(num_words * sizeof(char*));
    height_letter_words = SDL_malloc(num_words * sizeof(char*));
    nine_letter_words = SDL_malloc(num_words * sizeof(char*));

    num_seven_letter_words = 0;
    num_height_letter_words = 0;
    num_nine_letter_words = 0;

    for (int i = 0; i < num_words; i++)
    {
        words[i] = start;
        if (end)
        {
            *end = '\0';
            start = end + 1;
            end = SDL_strchr(start, '\n');
        }

        size_t length = SDL_strlen(words[i]);
        if (length == 7)
        {
            seven_letter_words[num_seven_letter_words] = words[i];
            num_seven_letter_words++;
        }
        else if (length == 8)
        {
            height_letter_words[num_height_letter_words] = words[i];
            num_height_letter_words++;
        }
        else if (length == 9)
        {
            nine_letter_words[num_nine_letter_words] = words[i];
            num_nine_letter_words++;
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
    int num_categories = 0;
    char** categories[3];
    int categories_length[3];

    if (seven_letters)
    {
        categories[num_categories] = seven_letter_words;
        categories_length[num_categories] = num_seven_letter_words;
        num_categories++;
    }
    if (height_letters)
    {
        categories[num_categories] = height_letter_words;
        categories_length[num_categories] = num_height_letter_words;
        num_categories++;
    }
    if (nine_letters)
    {
        categories[num_categories] = nine_letter_words;
        categories_length[num_categories] = num_nine_letter_words;
        num_categories++;
    }

    int random = SDL_rand(num_categories);
    return categories[random][SDL_rand(categories_length[random])];
}

void QuitWords()
{
    if (!words_file) return;

    if (seven_letter_words) SDL_free(seven_letter_words);
    if (height_letter_words) SDL_free(height_letter_words);
    if (nine_letter_words) SDL_free(nine_letter_words);
    if (words) SDL_free(words);

    SDL_free(words_file);
    words_file = NULL;
}