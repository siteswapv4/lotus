#pragma once
#include <SDL3/SDL.h>

extern int NUM_WORDS;
extern char** WORDS;
extern int DAILY_WORD_INDEX;

void SetDailyWord();

void LoadWords();

bool WordExists(const char* word);

void DestroyWords();