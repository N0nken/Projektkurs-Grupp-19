#include "../include/input_logger.h"
#include <SDL2/SDL.h>
#include <string.h>
#include <stdio.h>

#define NUMBEROFACTIONS 8
char allActions[9][20] = {
    "move_up", "move_down", "move_left", "move_right",
    "attack", "switch_to_rock", "switch_to_paper", "switch_to_scissors", "dash"
};
int actionTranslations[9][2] = {
    {SDL_SCANCODE_W, SDL_SCANCODE_UP}, {SDL_SCANCODE_S, SDL_SCANCODE_DOWN}, {SDL_SCANCODE_A, SDL_SCANCODE_LEFT}, {SDL_SCANCODE_D, SDL_SCANCODE_RIGHT},
    {SDL_SCANCODE_SPACE, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_1, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_2, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_3, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_LSHIFT}
};

struct Input_Logger {
    int up[3];
    int down[3];
    int left[3];
    int right[3];
    int attack[3];
    int switchToRock[3];
    int switchToPaper[3];
    int switchToScissors[3];
};

Input_Logger *create_Input_Logger() {
    Input_Logger *newInputLogger = malloc(sizeof(struct Input_Logger));
    for (int i = 0; i < 3; i++) {
        newInputLogger->up[i] = 0;
        newInputLogger->down[i] = 0;
        newInputLogger->left[i] = 0;
        newInputLogger->right[i] = 0;
        newInputLogger->attack[i] = 0;
        newInputLogger->switchToRock[i] = 0;
        newInputLogger->switchToPaper[i] = 0;
        newInputLogger->switchToScissors[i] = 0;
    }
    return newInputLogger;
}
void destroy_Input_Logger(Input_Logger *logger) {
    free(logger);
}
int Input_Logger_get_action_state(Input_Logger *logger, char action[], int state) {
    if (strcmp(action, "move_up") == 0) {
        return logger->up[state];
    } else if (strcmp(action, "move_down") == 0) {
        return logger->down[state];
    } else if (strcmp(action, "move_left") == 0) {
        return logger->left[state];
    } else if (strcmp(action, "move_right") == 0) {
        return logger->right[state];
    } else if (strcmp(action, "attack") == 0) {
        return logger->attack[state];
    } else if (strcmp(action, "switch_to_rock") == 0) {
        return logger->switchToRock[state];
    } else if (strcmp(action, "switch_to_paper") == 0) {
        return logger->switchToPaper[state];
    } else if (strcmp(action, "switch_to_scissors") == 0) {
        return logger->switchToScissors[state];
    }
}
void Input_Logger_set_action_state(Input_Logger *logger, char action[], int state) {
    if (strcmp(action, "move_up") == 0) {
        logger->up[state] = 1;
    } else if (strcmp(action, "move_down") == 0) {
        logger->down[state] = 1;
    } else if (strcmp(action, "move_left") == 0) {
        logger->left[state] = 1;
    } else if (strcmp(action, "move_right") == 0) {
        logger->right[state] = 1;
    } else if (strcmp(action, "attack") == 0) {
        logger->attack[state] = 1;
    } else if (strcmp(action, "switch_to_rock") == 0) {
        logger->switchToRock[state] = 1;
    } else if (strcmp(action, "switch_to_paper") == 0) {
        logger->switchToPaper[state] = 1;
    } else if (strcmp(action, "switch_to_scissors") == 0) {
        logger->switchToScissors[state] = 1;
    }
}
void Input_Logger_unset_action_state(Input_Logger *logger, char action[], int state) {
    if (strcmp(action, "move_up") == 0) {
        logger->up[state] = 0;
    } else if (strcmp(action, "move_down") == 0) {
        logger->down[state] = 0;
    } else if (strcmp(action, "move_left") == 0) {
        logger->left[state] = 0;
    } else if (strcmp(action, "move_right") == 0) {
        logger->right[state] = 0;
    } else if (strcmp(action, "attack") == 0) {
        logger->attack[state] = 0;
    } else if (strcmp(action, "switch_to_rock") == 0) {
        logger->switchToRock[state] = 0;
    } else if (strcmp(action, "switch_to_paper") == 0) {
        logger->switchToPaper[state] = 0;
    } else if (strcmp(action, "switch_to_scissors") == 0) {
        logger->switchToScissors[state] = 0;
    }
}
void Input_Logger_update_all_actions(Input_Logger *logger, const Uint8 *keystates) {
    for (int i = 0; i < NUMBEROFACTIONS; i++) {
        if (i == 4) {
            printf("attack %d\n", keystates[actionTranslations[i][0]]);
        }
        if (keystates[actionTranslations[i][0]] == 1 || keystates[actionTranslations[i][1]] == 1) {
            if (!Input_Logger_get_action_state(logger, allActions[i], 0) && !Input_Logger_get_action_state(logger, allActions[i], 1) && !Input_Logger_get_action_state(logger, allActions[i], 2)) {
                Input_Logger_set_action_state(logger, allActions[i], 0);
            } else if (Input_Logger_get_action_state(logger, allActions[i], 0) && !Input_Logger_get_action_state(logger, allActions[i], 1) && !Input_Logger_get_action_state(logger, allActions[i], 2)) {
                Input_Logger_set_action_state(logger, allActions[i], 1);
                Input_Logger_unset_action_state(logger, allActions[i], 0);
            }
        } else {
            if ((Input_Logger_get_action_state(logger, allActions[i], 0) || Input_Logger_get_action_state(logger, allActions[i], 1)) && !Input_Logger_get_action_state(logger, allActions[i], 2)) {
                Input_Logger_set_action_state(logger, allActions[i], 2);
                Input_Logger_unset_action_state(logger, allActions[i], 1);
                Input_Logger_unset_action_state(logger, allActions[i], 0);
            } else if (Input_Logger_get_action_state(logger, allActions[i], 2)) {
                Input_Logger_unset_action_state(logger, allActions[i], 2);
                Input_Logger_unset_action_state(logger, allActions[i], 1);
                Input_Logger_unset_action_state(logger, allActions[i], 0);
            }
        }
    }
}
int Input_Logger_is_action_just_pressed(Input_Logger *logger, char action[]) {
    return Input_Logger_get_action_state(logger, action, 0);
}
int Input_Logger_is_action_pressed(Input_Logger *logger, char action[]) {
    return Input_Logger_get_action_state(logger, action, 1);
}
int Input_Logger_is_action_just_released(Input_Logger *logger, char action[]) {
    return Input_Logger_get_action_state(logger, action, 2);
}