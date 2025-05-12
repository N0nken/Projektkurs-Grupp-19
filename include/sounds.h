#ifndef SOUNDS_H_
#define SOUNDS_H_

#include <stdbool.h>

bool init_music_system(const char* filename);

void close_music_system(void);

void play_sound_effect(const char* filename, int volume);

#endif