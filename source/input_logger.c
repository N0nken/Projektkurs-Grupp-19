#include "../include/input_logger.h"
#include <SDL2/SDL.h>
#include <string.h>
#include <stdio.h>

#define NUMBEROFACTIONS 9
// Contains string representations of each action
char allActions[9][20] = {
    "move_up", "move_down", "move_left", "move_right",
    "attack", "switch_to_rock", "switch_to_paper", "switch_to_scissors", "dash"
};
// Contains SDL_SCANCODE representation of each action
int actionTranslations[9][2] = {
    {SDL_SCANCODE_W, SDL_SCANCODE_UP}, {SDL_SCANCODE_S, SDL_SCANCODE_DOWN}, {SDL_SCANCODE_A, SDL_SCANCODE_LEFT}, {SDL_SCANCODE_D, SDL_SCANCODE_RIGHT},
    {SDL_SCANCODE_SPACE, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_1, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_2, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_3, SDL_SCANCODE_UNKNOWN}, {SDL_SCANCODE_LSHIFT}
};

struct InputLogger {
    int up[3];
    int down[3];
    int left[3];
    int right[3];
    int attack[3];
    int switchToRock[3];
    int switchToPaper[3];
    int switchToScissors[3];
    int dash[3];
};

InputLogger *create_InputLogger() {
    InputLogger *newInputLogger = malloc(sizeof(struct InputLogger));
    for (int i = 0; i < 3; i++) {
        newInputLogger->up[i] = 0;
        newInputLogger->down[i] = 0;
        newInputLogger->left[i] = 0;
        newInputLogger->right[i] = 0;
        newInputLogger->attack[i] = 0;
        newInputLogger->switchToRock[i] = 0;
        newInputLogger->switchToPaper[i] = 0;
        newInputLogger->switchToScissors[i] = 0;
        newInputLogger->dash[i] = 0;
    }
    return newInputLogger;
}
void destroy_InputLogger(InputLogger *logger) {
    free(logger);
}
// Gets the selected state of the selected action. Ideally not used outside of this input_logger.c. Use is_action_just_pressed/is_action_pressed/is_action_just_released instead
int InputLogger_get_action_state(InputLogger *logger, char action[], int stateID) {
    if (strcmp(action, "move_up") == 0) {
        return logger->up[stateID];
    } else if (strcmp(action, "move_down") == 0) {
        return logger->down[stateID];
    } else if (strcmp(action, "move_left") == 0) {
        return logger->left[stateID];
    } else if (strcmp(action, "move_right") == 0) {
        return logger->right[stateID];
    } else if (strcmp(action, "attack") == 0) {
        return logger->attack[stateID];
    } else if (strcmp(action, "switch_to_rock") == 0) {
        return logger->switchToRock[stateID];
    } else if (strcmp(action, "switch_to_paper") == 0) {
        return logger->switchToPaper[stateID];
    } else if (strcmp(action, "switch_to_scissors") == 0) {
        return logger->switchToScissors[stateID];
    } else if (strcmp(action, "dash") == 0) {
        return logger->dash[stateID];
    }
}
// Sets the selected state of the selected action. Ideally not used outside of this input_logger.c
void InputLogger_set_action_state(InputLogger *logger, char action[], int stateID, int state) {
    if (strcmp(action, "move_up") == 0) {
        logger->up[stateID] = state;
    } else if (strcmp(action, "move_down") == 0) {
        logger->down[stateID] = state;
    } else if (strcmp(action, "move_left") == 0) {
        logger->left[stateID] = state;
    } else if (strcmp(action, "move_right") == 0) {
        logger->right[stateID] = state;
    } else if (strcmp(action, "attack") == 0) {
        logger->attack[stateID] = state;
    } else if (strcmp(action, "switch_to_rock") == 0) {
        logger->switchToRock[stateID] = state;
    } else if (strcmp(action, "switch_to_paper") == 0) {
        logger->switchToPaper[stateID] = state;
    } else if (strcmp(action, "switch_to_scissors") == 0) {
        logger->switchToScissors[stateID] = state;
    } else if (strcmp(action, "dash") == 0) {
        logger->dash[stateID] = state;
    }
}
/*  CLIENT ONLY 
    Updates the state of all actions
*/
void InputLogger_update_all_actions(InputLogger *logger, const Uint8 *keystates) {
    for (int i = 0; i < NUMBEROFACTIONS; i++) {
        if (keystates[actionTranslations[i][0]] == 1 || keystates[actionTranslations[i][1]] == 1) {
            //printf("action %s pressed\n", allActions[i]);
            if (!InputLogger_get_action_state(logger, allActions[i], 0) && !InputLogger_get_action_state(logger, allActions[i], 1) && !InputLogger_get_action_state(logger, allActions[i], 2)) {
                InputLogger_set_action_state(logger, allActions[i], 0, 1);
            } else if (InputLogger_get_action_state(logger, allActions[i], 0) && !InputLogger_get_action_state(logger, allActions[i], 1) && !InputLogger_get_action_state(logger, allActions[i], 2)) {
                InputLogger_set_action_state(logger, allActions[i], 1, 1);
                InputLogger_set_action_state(logger, allActions[i], 0, 0);
            }
        } else {
            //printf("action %s not pressed\n", allActions[i]);
            if ((InputLogger_get_action_state(logger, allActions[i], 0) || InputLogger_get_action_state(logger, allActions[i], 1)) && !InputLogger_get_action_state(logger, allActions[i], 2)) {
                InputLogger_set_action_state(logger, allActions[i], 2, 1);
                InputLogger_set_action_state(logger, allActions[i], 1, 0);
                InputLogger_set_action_state(logger, allActions[i], 0, 0);
            } else if (InputLogger_get_action_state(logger, allActions[i], 2)) {
                InputLogger_set_action_state(logger, allActions[i], 2, 0);
                InputLogger_set_action_state(logger, allActions[i], 1, 0);
                InputLogger_set_action_state(logger, allActions[i], 0, 0);
            }
        }
    }
}
// returns 1 if the selected action was just pressed (not pressed the frame before but pressed this frame) else return 0
int InputLogger_is_action_just_pressed(InputLogger *logger, char action[]) {
    return InputLogger_get_action_state(logger, action, 0);
}
// returns 1 if the selected action was pressed the frame before and this frame else return 0
int InputLogger_is_action_pressed(InputLogger *logger, char action[]) {
    return InputLogger_get_action_state(logger, action, 1);
}
// returns 1 if the selected action was pressed the frame before but not this frame else return 0
int InputLogger_is_action_just_released(InputLogger *logger, char action[]) {
    return InputLogger_get_action_state(logger, action, 2);
}

void InputLogger_reset_all_actions(InputLogger *logger) {
    for (int i = 0; i < 3; i++) {
        logger->up[i] = 0;
        logger->down[i] = 0;
        logger->left[i] = 0;
        logger->right[i] = 0;
        logger->attack[i] = 0;
        logger->switchToRock[i] = 0;
        logger->switchToPaper[i] = 0;
        logger->switchToScissors[i] = 0;
        logger->dash[i] = 0;
    }
}

void InputLogger_print_inputs(InputLogger *logger) {
    for (int j = 0; j < 3; j++) {
        printf("inputs stage %d: %d %d %d %d %d %d %d %d\n", j, InputLogger_get_action_state(logger, "move_up", j), 
            InputLogger_get_action_state(logger, "move_down", j), 
            InputLogger_get_action_state(logger, "move_left", j), 
            InputLogger_get_action_state(logger, "move_right", j), 
            InputLogger_get_action_state(logger, "attack", j), 
            InputLogger_get_action_state(logger, "switch_to_rock", j), 
            InputLogger_get_action_state(logger, "switch_to_paper", j), 
            InputLogger_get_action_state(logger, "switch_to_scissors", j));
    }
}