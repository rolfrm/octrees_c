
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <iron/types.h>
#include <iron/mem.h>

#include "event.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <GL/gl.h>
#include "sdl_event.h"
#include <iron/log.h>
#include <iron/utils.h>
#include <iron/linmath.h>
#include "oct_node.h"
#include "vec3i.h"
#include "octree.h"
#include "game_state.h"

#include "renderer.h"

static vec2 iso_offset(vec3 v){
  return vec2mk(v.x + v.z, v.z / 2 - v.y - v.x / 2);
}

struct _game_renderer{
  SDL_Window * window;
  SDL_Renderer * renderer;
};

struct _texture_asset{
  SDL_Texture * texture;
  vec2 offset;
};

static bool sdl_inited = false;
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
  SDL_SetRenderTarget(rnd->renderer, NULL);
  SDL_SetRenderDrawColor(rnd->renderer, 0, 255, 255, 255);
  SDL_RenderClear(rnd->renderer);
  int h,w;
  SDL_GetWindowSize(rnd->window,&w, &h);

  oct_node * start_node = oct_get_nth_super(state->center_node, 5);
  vec3 offset = oct_get_super_offset(state->center_node, start_node);
  float size = oct_get_super_size(state->center_node, start_node);
  float base_size = 48.0/2;
  void render_fcn(oct_node * n, float s, vec3 offset)
  {
    UNUSED(s);
    texture_asset * payload = oct_get_payload(n);
    if(payload == NULL) return;
    vec2 point = vec2_add(iso_offset(offset), payload->offset);
    //logd("Render: %f %f", s, size);vec2_print(point);vec3_print(offset);logd("\n");
    SDL_Rect rec;
    rec.x = point.x + h / 2;
    rec.y = point.y + w / 2;
    rec.w = 0; 
    rec.h = 0;
    int access;
    u32 format;
    SDL_QueryTexture(payload->texture, &format, &access, &rec.w, &rec.h);
    SDL_SetTextureBlendMode(payload->texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rnd->renderer, payload->texture, NULL, &rec);
  }

  oct_render_node(start_node, size * base_size, vec3_scale(offset, base_size), render_fcn);
  
  ellipseColor(rnd->renderer,10,10,10,10,0xFF00FFFF);

  SDL_RenderPresent(rnd->renderer);  
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

static int get_pixel_format(int channels){
  switch(channels){
  case 3:
    return SDL_PIXELFORMAT_RGB24;
  case 4:
    return SDL_PIXELFORMAT_ABGR8888;
  case 1:
    return SDL_PIXELFORMAT_INDEX8;
  }
  UNREACHABLE();
  return 0;
}

#include "image.h"
texture_asset * renderer_load_texture(game_renderer * rnd, const char * path){
  int w,h,c;
  char * im_data = stbi_load((char *)path, &w, &h, &c,4);
  SDL_Texture * tex = SDL_CreateTexture(rnd->renderer, get_pixel_format(c), 
					SDL_TEXTUREACCESS_STATIC, w, h);
  SDL_UpdateTexture(tex, NULL, im_data, w * c);
  texture_asset * texasset = alloc(sizeof(texasset));
  *texasset = (texture_asset) {tex, vec2mk(0,0)};
  return texasset;
}

void texture_asset_set_offset(texture_asset * asset, vec2 offset){
  asset->offset = offset;
}

vec2 texture_asset_get_offset(texture_asset * asset){
  return asset->offset;
}


void renderer_delete_asset(texture_asset * asset){
  UNUSED(asset);
}
