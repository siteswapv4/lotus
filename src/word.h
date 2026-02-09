#pragma once
#include <SDL3/SDL.h>

void InitWords();

bool WordExists(const char* word);

void EnableSevenLetterWords(bool enable);

void EnableEightLetterWords(bool enable);

void EnableNineLetterWords(bool enable);

const char* GetDailyWord();

const char* GetRandomWord();

void QuitWords();