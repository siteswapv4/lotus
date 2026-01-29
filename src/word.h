#pragma once
#include <SDL3/SDL.h>

void InitWords();

bool WordExists(const char* word);

const char* GetDailyWord();

const char* GetRandomWord();

void QuitWords();