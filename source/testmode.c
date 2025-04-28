#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../include/testmode.h"
#include "../include/player.h"
#include "../include/vector2.h"
#include "../include/collision.h"
#include "../include/movement.h"
#include "../include/attacks.h"
#include "../include/input_logger.h"
#include "../include/renderController.h"
#include "../include/dynamic_textarea.h"

#define TARGETFPS 60

// Copy of Frame struct from client.c
struct Frame { int x; int y; int h; int w; } typedef frame;

int test_mode_main(RenderController* renderController) {/*
    // Create single player
    Player* players[1];
    int aliveCount = 0;
    players[0] = create_Player(
        create_Vector2(100, 100),
        create_Collider(create_Vector2(100, 100), create_Vector2(PLAYERWIDTH, PLAYERHEIGHT), 0, PLAYERCOLLISIONLAYER),
        create_Collider(create_Vector2(PLAYERATTACKHITBOXOFFSETX, PLAYERATTACKHITBOXOFFSETY), create_Vector2(PLAYERATTACKHITBOXWIDTH, PLAYERATTACKHITBOXHEIGHT), 0, PLAYERATTACKLAYER),
        100, 0, 1, players, &aliveCount);
    aliveCount = 1;
    InputLogger_reset_all_actions(Player_get_inputs(players[0]));

    // Set up platforms for collision
    Collider* platform1 = create_Collider(create_Vector2(100, 410), create_Vector2(120, 20), 0, 1);
    Collider* platform2 = create_Collider(create_Vector2(290, 310), create_Vector2(120, 20), 0, 1);
    Collider* platform3 = create_Collider(create_Vector2(480, 410), create_Vector2(120, 20), 0, 1);
    SDL_Rect platforms[3] = {
        {10, 452, 240, 20},
        {200, 364, 240, 20},
        {390, 452, 240, 20}
    };

    frame playerFrame = {0};
    int spriteW = 32, spriteH = 32;
    SDL_Event event;
    Uint64 lastTicks = SDL_GetTicks64();

    while (1) {
        // Input & events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return 0;
        }
        InputLogger_update_all_actions(Player_get_inputs(players[0]), SDL_GetKeyboardState(NULL));

        // Simulation step
        Uint64 currentTicks = SDL_GetTicks64();
        float deltaTime = (currentTicks - lastTicks) * 0.001f;
        lastTicks = currentTicks;
        handle_movement(players[0], PLAYERSPEED, platform1, platform2, platform3, deltaTime);
        handle_attack_input(players, 1);

        // Rendering
        SDL_RenderClear(renderController->renderer);
        SDL_RenderCopy(renderController->renderer, renderController->background, NULL, NULL);
        for (int i = 0; i < 3; i++) SDL_RenderFillRect(renderController->renderer, &platforms[i]);
        SDL_QueryTexture(renderController->playerSpritesheet, NULL, NULL, &spriteH, &spriteW);
        SDL_RenderCopy(
            renderController->renderer,
            renderController->playerSpritesheet,
            get_Player_Frame(&playerFrame, 2, get_Animation_Counter(Player_get_inputs(players[0]))),
            Player_get_rect(players[0])
        );
        SDL_RenderPresent(renderController->renderer);
        SDL_Delay(1000 / TARGETFPS);
    }
    return 0;*/
}