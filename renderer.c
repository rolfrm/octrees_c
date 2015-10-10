
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
  int unit_size;
};

struct _texture_asset{
  SDL_Texture * texture;
  vec2 offset;
  vec2i size;
};

static bool sdl_inited = false;
game_renderer * renderer_load(int width, int height, int unit_size){
  if(!sdl_inited){
    SDL_Init(SDL_INIT_VIDEO);
    sdl_inited = true;
  }
  game_renderer * rnd = alloc0(sizeof(game_renderer));
  rnd->window = SDL_CreateWindow("--", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 
				 SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  rnd->renderer = SDL_CreateRenderer(rnd->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  rnd->unit_size = unit_size;
  return rnd;
}

void renderer_render(game_renderer * rnd, world_state * state){
  SDL_SetRenderTarget(rnd->renderer, NULL);
  SDL_SetRenderDrawColor(rnd->renderer, 0, 255, 255, 255);
  SDL_RenderClear(rnd->renderer);
  int h,w;
  SDL_GetWindowSize(rnd->window,&w, &h);

  oct_node * start_node = oct_get_nth_super(state->center_node, 4);
  vec3 offset = oct_get_super_offset(state->center_node, start_node);
  float size = oct_get_super_size(state->center_node, start_node);

 float base_size = 56.0/2;
  void render_fcn(oct_node * n, float s, vec3 _offset)
  {
    UNUSED(s);
    entity_list * pl = oct_get_payload(n);
    if(pl == NULL) return;
    for(size_t i = 0; i < pl->cnt; i++){
      vec3 offset = _offset;
      if(offset.y > 28) continue;
      entity_header * payload = pl->entity[i];//pl->cnt - i - 1];
      texture_asset * asset = NULL;
      if(payload->type == OBJECT){
	offset = vec3_add(offset, vec3_scale(((entity *) payload)->offset, s));
	asset = ((entity *) payload)->texture;
      }else if(payload->type == TILE){
	asset = ((tile *) payload)->asset;

      }else if(payload->type == SATELITE){
	asset = ((satelite *) payload)->origin->texture;
	offset = vec3_add(offset, vec3_scale(((satelite *) payload)->offset, s));
	continue;
      }
      if(asset == NULL){
	logd("Continuing..\n");
	continue;
      }
      vec2 point = vec2_add(iso_offset(offset), asset->offset);
      SDL_Rect rec;
      rec.x = point.x + h / 2;
      rec.y = point.y + w / 2;
      rec.w = 0; 
      rec.h = 0;
      int access;
      u32 format;
      SDL_QueryTexture(asset->texture, &format, &access, &rec.w, &rec.h);
      SDL_Rect rec2 = {0,0,MIN(rec.w, asset->size.x),MIN(rec.h, asset->size.y)};
      rec.w = rec2.w;
      rec.h = rec2.h;
      SDL_RenderCopy(rnd->renderer, asset->texture, &rec2, &rec);
    }
  }
  for(int k = -3; k <= 0; k++)
    for(int i = -2; i <= 2; i++)
      for(int j = 2; j >= -2; j--){
	oct_node * sn1 = oct_get_relative(start_node, (vec3i){j, k, i});
	vec3 offset_sn1 = vec3_add(offset, vec3mk(size * j, size * k, size * i));
	oct_render_node(sn1, size * base_size, vec3_scale(offset_sn1, base_size), render_fcn);
      }
  SDL_RenderPresent(rnd->renderer);  
}

u32 renderer_read_events(event * buffer, u32 buffer_size){
  SDL_Event evt; 
  u32 cnt = 0;
  for(; cnt<buffer_size;cnt++){
    if(SDL_PollEvent(&evt)){
      buffer[cnt] = sdl_event_to_event(evt);
      //if(evt.type == SDL_KEYUP)
	//logd("%i %i\n", evt.key.keysym.sym, evt.key.keysym.scancode);
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
  int c;
  vec2i size;
  vec2 offset = vec2mk(0, 0);
  char * im_data = stbi_load((char *)path, &size.x, &size.y, &c,4);
  SDL_Texture * tex = SDL_CreateTexture(rnd->renderer, get_pixel_format(c), 
					SDL_TEXTUREACCESS_STATIC, size.x, size.y);
  SDL_UpdateTexture(tex, NULL, im_data, size.x * c);
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  texture_asset * texasset = alloc(sizeof(texasset));
  *texasset = (texture_asset) {tex, offset , size};
  return texasset;
}

texture_asset * renderer_clone_texture(texture_asset * asset){
  texture_asset * cloned = clone(asset, sizeof(texture_asset));  
  return cloned;
}
void texture_asset_set_offset(texture_asset * asset, vec2 offset){
  asset->offset = offset;
}

void texture_asset_set_size(texture_asset * asset, vec2i size){
  asset->size = size;
}

void renderer_delete_asset(texture_asset * asset){
  UNUSED(asset);
}

game_controller_state renderer_game_controller(){
  game_controller_state gc;
  if(false){ //game controller connected

  }else{
    const u8 * keys = SDL_GetKeyboardState(NULL);
    gc.axes[0] = (keys[SDL_SCANCODE_A] ? -1.0 : 0.0) +  (keys[SDL_SCANCODE_D] ? 1.0 : 0.0);
    gc.axes[1] = (keys[SDL_SCANCODE_W] ? 1 : 0) + (keys[SDL_SCANCODE_S] ? -1 : 0);
    gc.axes[2] = (keys[SDL_SCANCODE_LEFT] ? -1 : 0) +  (keys[SDL_SCANCODE_RIGHT] ? 1 : 0);
    gc.axes[3] = (keys[SDL_SCANCODE_UP] ? 1 : 0) + (keys[SDL_SCANCODE_DOWN] ? -1 : 0);
    gc.axes[4] = keys[SDL_SCANCODE_LCTRL] ? 1 : 0;
    gc.axes[5] = keys[SDL_SCANCODE_DOWN] ? 1 : 0;
    gc.buttons[0] = keys[SDL_SCANCODE_E] ? BUTTON_DOWN : BUTTON_UP;
    gc.buttons[1] = keys[SDL_SCANCODE_F] ? BUTTON_DOWN : BUTTON_UP;
    gc.buttons[2] = keys[SDL_SCANCODE_LSHIFT] ? BUTTON_DOWN : BUTTON_UP;
    gc.buttons[3] = keys[SDL_SCANCODE_SPACE] ? BUTTON_DOWN : BUTTON_UP;
    gc.buttons[4] = keys[SDL_SCANCODE_RETURN] ? BUTTON_DOWN : BUTTON_UP;
    gc.buttons[5] = keys[SDL_SCANCODE_ESCAPE] ? BUTTON_DOWN : BUTTON_UP;
  }
  return gc;
}
void game_controller_state_print(game_controller_state s){
  logd("Axes: {");
  for(size_t i = 0; i < array_count(s.axes); i++){
    logd("%f", s.axes[i]);
    if(i != array_count(s.axes) -1){
      logd(", ");
    }
  }
  logd("} Buttons: }");
  for(size_t i = 0; i < array_count(s.buttons); i++){
    logd("%i", s.buttons[i]);
    if((i + 1) != array_count(s.axes)){
      logd(", ");
    }
  }
}

vec2i renderer_get_mouse_position(game_renderer * rnd){
  int w, h;
  SDL_GetWindowSize(rnd->window,&w, &h);
  vec2i o;
  SDL_GetMouseState(&o.x, &o.y);
  return (vec2i){o.x - w/2, h/2 - o.y};
}
