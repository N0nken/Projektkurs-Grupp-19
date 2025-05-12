#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "../include/sounds.h"

static Mix_Music* background_music = NULL;
static bool audio_initialized = false;

// musik fungerar för mp3, inte testat för wav
bool init_music_system(const char* filename) {
    if (!audio_initialized) {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            SDL_Log("SDL_mixer could not initialize. SDL_mixer Error: %s\n", Mix_GetError());
            return false;
        }
        audio_initialized = true;
    }
    if (background_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(background_music);
        background_music = NULL;
    }
    background_music = Mix_LoadMUS(filename);
    if (!background_music) {
        SDL_Log("Failed to load music: %s\n", Mix_GetError());
        return false;
    }
    Mix_PlayMusic(background_music, -1); // -1 = loopas
    return true;
}

void close_music_system() {
    if (background_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(background_music);
        background_music = NULL;
    }
}
// OBS: För sfx *måste* filen vara wav för spelning. Annars måste koden ändras
void play_sound_effect(const char* filename, int volume) {
    Mix_Chunk* effect = Mix_LoadWAV(filename);
    if (!effect) {
        SDL_Log("Failed to load sound effect: %s\n", Mix_GetError());
        return;
    }
    // (0 = tyst, 128 = max)
    Mix_VolumeChunk(effect, volume);
    int channel = Mix_PlayChannel(-1, effect, 0);
    if (channel == -1) {
        SDL_Log("Failed to play sound effect: %s\n", Mix_GetError());
        Mix_FreeChunk(effect);
        return;
    }
    // frigör inte effekten direkt, endast vid programslut
}
