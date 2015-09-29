typedef struct _game_state{
  int i;

}game_state;
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <iron/types.h>
#include <iron/mem.h>

#include "event.h"
#include "renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <GL/gl.h>
#include "sdl_event.h"
#include <iron/log.h>

struct _game_renderer{
  SDL_Window * window;
  SDL_Renderer * renderer;
};
bool sdl_inited = false;
game_renderer * renderer_load(int width, int height){
  if(!sdl_inited){
    SDL_Init(SDL_INIT_VIDEO);
    sdl_inited = true;
  }
  game_renderer * rnd = alloc0(sizeof(game_renderer));
  rnd->window = SDL_CreateWindow("--", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 
				 SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  rnd->renderer = SDL_CreateRenderer(rnd->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  return rnd;
}

void renderer_render(game_renderer * rnd, game_state * state){
  SDL_SetRenderDrawColor(rnd->renderer, 255, 255, 255, 255);
  SDL_RenderClear(rnd->renderer);
  ellipseColor(rnd->renderer,10,10,10,10,0xFF00FFFF);
  SDL_RenderPresent(rnd->renderer);
  if(state != NULL)
    logd("%i\n", state->i);
}

u32 renderer_read_events(event * buffer, u32 buffer_size){
  SDL_Event evt; 
  u32 cnt = 0;
  for(; cnt<buffer_size;cnt++){
    if(SDL_PollEvent(&evt)){
      buffer[cnt] = sdl_event_to_event(evt);
    }else{
      return cnt;
    }
  }
  return buffer_size;
}

