#ifndef TEST_H
#define TEST_H


typedef struct button button;

//Creates a button
button *button_create(int x, int y, int w,int h, TTF_Font *Font, SDL_Renderer *Renderer, SDL_Window* Window);
// Free the button from memory.
void button_destroy(button *Button);

//One of three hover functions. This one sets the button hover state to true.
void set_button_hover_true(button *Button);
//One of three hover functions. This one sets the button hover state to false.
void set_button_hover_false(button *Button);
//One of three hover functions. This one returns the button hover state.
bool ret_button_hover_state(button *Button);

//One of three click functions. This one sets the button click state to true.
void set_button_click_true(button *Button);
//One of three click functions. This one sets the button click state to false.
void set_button_click_false(button *Button);
//One of three click functions. This one returns the button click state.
bool ret_button_click_state(button *Button);

//Returns true if the x and y is in the button, image or rectangle else false.
bool is_in_button_rect(int x, int y, SDL_Rect r);
//Return the SDL_rect in the button.
SDL_Rect ret_button_rect(button *Button);

//Pallar inte kommentera saker längre.....
void ret_button_normal(SDL_Rect Area, SDL_Renderer *Renderer);
void ret_button_hover(SDL_Rect Area, SDL_Renderer *Renderer);
void ret_button_clicked(SDL_Rect Area, SDL_Renderer *Renderer);
void ret_button_image(SDL_Rect Area, SDL_Renderer *Renderer, SDL_Surface* img);
void load_Button_Text(button *Button, const char *text, SDL_Renderer* Renderer);
void center_button(button *Button,SDL_Window *Window);

#endif