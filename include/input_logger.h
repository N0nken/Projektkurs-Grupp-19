#ifndef INPUT_LOGGER_H_
#define INPUT_LOGGER_H_
#include <SDL2/SDL.h>

typedef struct InputLogger InputLogger;

InputLogger *create_InputLogger();
void destroy_InputLogger(InputLogger *logger);
void InputLogger_update_all_actions(InputLogger *logger, const Uint8 *keystates);
void InputLogger_set_action_state(InputLogger *logger, char action[], int stateID, int state);
int InputLogger_get_action_state(InputLogger *logger, char action[], int stateID);
int InputLogger_is_action_just_pressed(InputLogger *logger, char action[]);
int InputLogger_is_action_pressed(InputLogger *logger, char action[]);
int InputLogger_is_action_just_released(InputLogger *logger, char action[]);

#endif