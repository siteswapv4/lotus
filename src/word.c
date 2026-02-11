#include "word.h"

#include "stb_ds.h"

static char* words_file = NULL;
static const char** words[NUM_LETTER_SPAN] = {0};
static const char** all_words = NULL;
static bool enabled_categories[NUM_LETTER_SPAN] = {0};

int daily_word_category = 0;
int daily_word_index = 0;

int random_word_category = 0;
int random_word_index = 0;


void SetRandomWord(bool enabled[NUM_LETTER_SPAN], int* category, int* index)
{
    int num_usable = 0;
    int usable[NUM_LETTER_SPAN] = {0};

    for (int i = 0; i < NUM_LETTER_SPAN; i++)
    {
        if (enabled[i] && words[i] && (arrlen(words[i])))
        {
            usable[num_usable] = i;
            num_usable++;
        }
    }

    if (num_usable <= 0)
        return;

    *category = usable[SDL_rand(num_usable)];
    *index = SDL_rand(arrlen(words[*category]));
}

void LoadWords()
{
    char* path = NULL;
#if defined(SDL_PLATFORM_ANDROID)
    SDL_asprintf(&path, "words.txt");
#else
    SDL_asprintf(&path, "%s/data/words.txt", SDL_GetBasePath());
#endif
    words_file = SDL_LoadFile(path, NULL);
    SDL_free(path);
    
    int num_words = 0;
    SDL_sscanf(words_file, "%d", &num_words);
    char* start = SDL_strchr(words_file, '\n') + 1;
    char* end = SDL_strchr(start, '\n');

    for (int i = 0; i < num_words; i++)
    {
        arrput(all_words, start);

        if (end)
        {
            *end = '\0';
            start = end + 1;
            end = SDL_strchr(start, '\n');
        }

        size_t length = SDL_strlen(all_words[i]);
        if ((length >= MIN_NUM_LETTERS) && (length <= MAX_NUM_LETTERS))
        {
            arrput(words[length - MIN_NUM_LETTERS], all_words[i]);
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
    
    bool all_enabled[NUM_LETTER_SPAN];
    SDL_memset(all_enabled, 1, NUM_LETTER_SPAN * sizeof(bool));

    SetRandomWord(all_enabled, &daily_word_category, &daily_word_index);
}

bool WordExists(const char* word)
{
    int left = 0;
    int right = arrlen(all_words) - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        int cmp = SDL_strcmp(word, all_words[mid]);

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

    SDL_memset(enabled_categories, 1, NUM_LETTER_SPAN * sizeof(bool));

    LoadWords();
    SetDailyWord();
}

const char* GetRandomWord()
{
    return words[random_word_category][random_word_index];
}

const char* GetDailyWord()
{
    return words[daily_word_category][daily_word_index];
}

void NextRandomWord()
{
    SetRandomWord(enabled_categories, &random_word_category, &random_word_index);
}

bool CategoryIsEnabled(int index)
{
    return enabled_categories[index];
}

void EnableCategory(int index, bool enable)
{
    enabled_categories[index] = enable;
}

void QuitWords()
{
    if (!words_file) return;

    for (int i = 0; i < NUM_LETTER_SPAN; i++)
    {
        if (words[i]) arrfree(words[i]);
        words[i] = NULL;
    }
    if (all_words) arrfree(all_words);

    SDL_free(words_file);
    words_file = NULL;
}
