#pragma once
#include <SDL3/SDL.h>

#define MIN_NUM_LETTERS 7
#define MAX_NUM_LETTERS 9
#define NUM_LETTER_SPAN (MAX_NUM_LETTERS - MIN_NUM_LETTERS + 1)

void InitWords();

bool WordExists(const char* word);

const char* GetDailyWord();

const char* GetRandomWord();

bool CategoryIsEnabled(int index);

void EnableCategory(int index, bool enable);

void QuitWords();