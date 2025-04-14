#ifndef INPUT_LOGGER_H_
#define INPUT_LOGGER_H_
#include <SDL2/SDL.h>

typedef struct Input_Logger Input_Logger;

Input_Logger *create_Input_Logger();
void destroy_Input_Logger(Input_Logger *logger);
int Input_Logger_get_action_state(Input_Logger *logger, char action[], int state);
void Input_Logger_set_action_state(Input_Logger *logger, char action[], int state);
void Input_Logger_unset_action_state(Input_Logger *logger, char action[], int state);
void Input_Logger_update_all_actions(Input_Logger *logger, const Uint8 *keystates);
int Input_Logger_is_action_just_pressed(Input_Logger *logger, char action[]);
int Input_Logger_is_action_pressed(Input_Logger *logger, char action[]);
int Input_Logger_is_action_just_released(Input_Logger *logger, char action[]);

#endif