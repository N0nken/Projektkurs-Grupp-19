#ifndef dynamic_textarea_h
#define dynamic_textarea_h
void create_textarea(SDL_Renderer* renderer,int x, int y, int size, TTF_Font* font,   const char *text, SDL_Color color);
void create_textarea_linebreaks(SDL_Renderer* renderer,int x, int y, int size, TTF_Font* font,  const char *text, SDL_Color color);
#endif