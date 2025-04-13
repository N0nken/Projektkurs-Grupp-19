#ifndef INPUT_LOGGER_H_
#define INPUT_LOGGER_H_

typedef struct Input_Logger Input_Logger;

Input_Logger *create_Input_Logger();
void destroy_Input_Logger(Input_Logger *logger);
void Input_Logger_update_action(Input_Logger *logger, char action[]);
void Input_Logger_unset_action(Input_Logger *logger, char action[]);
int Input_Logger_is_action_just_pressed(Input_Logger *logger, char action[]);
int Input_Logger_is_action_pressed(Input_Logger *logger, char action[]);
int Input_Logger_is_action_just_released(Input_Logger *logger, char action[]);

#endif