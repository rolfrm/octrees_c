#include <iron/full.h>
#include <stdlib.h>
#include <time.h>
#include "oct_node.h"
#include "vec3i.h"
#include "octree.h"

#include <signal.h>
#include "event.h"
#include "game_state.h"
#include "renderer.h"
#include "image.h"

void _error(const char * file, int line, const char * str, ...){
  loge("** ERROR at %s:%i **\n",file,line);
  loge("%s", str);
  logd("\n");
  logd("** **\n");
  raise(SIGINT);
}

tile * insert_tile(oct_node * oc, vec3i pos, texture_asset * asset){
  oc = oct_get_relative(oc, pos);
  tile t = {TILE, NULL, asset};
  tile * t2 = clone(&t, sizeof(t));
  add_entity(oc, (entity_header *) t2);
  return t2;
}

entity * insert_entity(oct_node * oc, vec3 pos, vec3 size, texture_asset * asset){
  oc = oct_find_fitting_node(oc, &pos, &size);
  entity e = {OBJECT, NULL, asset, pos, size};
  entity * e2 = clone(&e, sizeof(entity));
  add_entity(oc, (entity_header *) e2);
  return e2;
}

void update_entity(entity * e){
  oct_node * oc = e->node;
  remove_entity((entity_header *) e);
  oc = oct_find_fitting_node(oc, &e->offset, &e->size);
  add_entity(oc, (entity_header *) e);
}

int main(){

  game_renderer * rnd2 = renderer_load(500, 500, 28);
  oct_node * n1 = oct_create();
  world_state state = { n1 };
  texture_asset * tile22 = renderer_load_texture(rnd2, "../racket_octree/tile6.png");
  texture_asset * tile25 = renderer_load_texture(rnd2, "../racket_octree/tile7.png");
  texture_asset * tile3 = renderer_load_texture(rnd2, "../racket_octree/tile2x2.png");
  texture_asset * tile1 = renderer_load_texture(rnd2, "../racket_octree/tile1.png");
  //  texture_asset * guy = renderer_load_texture(rnd2, "../racket_octree/guy2.png");
  texture_asset * guy = renderer_load_texture(rnd2, "../racket_octree/guy3.png");
  texture_asset * guyupper = renderer_clone_texture(guy);
  texture_asset * tile4 = renderer_load_texture(rnd2, "../racket_octree/tile6.png");
  texture_asset * tile5 = renderer_load_texture(rnd2, "../racket_octree/tile8.png");
  texture_asset * rock_small = renderer_load_texture(rnd2, "../racket_octree/rock_small.png");
  texture_asset_set_offset(tile22, vec2mk(0, -42));
  texture_asset_set_offset(tile25, vec2mk(0, -42));
  texture_asset_set_offset(tile5, vec2mk(0, -42));
  texture_asset_set_offset(tile4, vec2mk(0, -42));
  texture_asset_set_offset(tile3, vec2mk(0, - 77));
  texture_asset_set_offset(tile1, vec2mk(0, -23));
  texture_asset_set_offset(rock_small, vec2mk(0, -19));
  texture_asset_set_offset(guy, vec2mk(0, -70));
  texture_asset_set_offset(guyupper, vec2mk(0, -42));
  texture_asset_set_size(guyupper, (vec2i){40, 40});
  
  for(int j = 0; j < 10; j++)
    for(int i = 0; i < j; i++)
      insert_tile(oct_get_sub(n1,0), vec3i_make(0, 2 + i, -2 - j), rock_small);
  int size = 100;
  for(int i = -size; i < size; i++)
    for(int j = -size; j < size; j++){
      int biome = ((i+j) / 10) & 3;
      insert_tile(n1, vec3i_make(i, 0, j), biome == 1 ? tile5 : tile22);
      if(rand() % 40 == 0){
	insert_tile(n1, vec3i_make(i, 1, j), tile25);
	if(rand() % 1 == 0){
	  insert_tile(n1, vec3i_make(i, 2, j), tile25);
	  if(rand() % 1 == 0){
	    insert_tile(n1, vec3i_make(i, 3, j), tile22);
	    if(rand()% 1 == 0){
	      insert_tile(n1, vec3i_make(i + 1, 3, j), tile22);
	      insert_tile(n1, vec3i_make(i + 2, 3, j), tile22);
	      insert_tile(n1, vec3i_make(i + 2, 2, j), tile25);
	      insert_tile(n1, vec3i_make(i + 2, 1, j), tile25);
	    }
	  }
	}
      }else if(rand() % 20 == 0){
	insert_tile(n1, vec3i_make(i, 1, j), tile5);
	insert_tile(n1, vec3i_make(i, 1, j+1), tile5);
	insert_tile(n1, vec3i_make(i+1, 1, j), tile5);
	insert_tile(n1, vec3i_make(i+1, 1, j+1), tile5);
	if(rand() % 10 == 0)
	  insert_tile(n1, vec3i_make(i, 2, j), tile5);
      }else if(rand() % 20 == 0){
	for(int k = 0 ; k < rand() % 20; k++)
	  insert_tile(n1, vec3i_make(i, 1 + k, j), tile5);
      }
    }
  entity * n = insert_entity(n1, vec3mk(0, 1, 0), vec3mk(1, 1, 1), guy);
  entity * n_2 = insert_entity(n1, vec3mk(0, 2, 0), vec3mk(1, 1, 1), guyupper);
  while(true){
    UNUSED(state);
    renderer_render(rnd2, &state);
    event evt[32];
    u32 event_cnt = renderer_read_events(evt, array_count(evt));
    game_controller_state gcs = renderer_game_controller();
    for(u32 i = 0; i < event_cnt; i++)
      switch(evt[i].type){
      case QUIT:
	return 0;
      case KEY:
	break;
      default:
	continue;
      }

    game_controller_state_print(gcs);logd("\n");
    
    vec3 mv = vec3mk(gcs.axes[0] * 1, gcs.axes[3] * 1, gcs.axes[1] * 1);
    vec3 newoffset = vec3_add(n->offset, mv);
    bool collision = false;
    void check_collision(oct_node * oc, vec3 pos, vec3 size){
      UNUSED(pos);UNUSED(size);
      entity_list * lst = oct_get_payload(oc);
      if(lst == NULL) return;
      for(size_t i = 0; i < lst->cnt; i++)
	collision |= lst->entity[i]->type == TILE;
    }
    oct_lookup_blocks(n->node, newoffset, n->size, check_collision);
    oct_lookup_blocks(n_2->node, newoffset, n_2->size, check_collision);

    if(!collision){
      oct_node * _n = n->node;
      oct_node * _n2 = n_2->node;
      
      n->offset = newoffset;
      n_2->offset = newoffset;
      remove_entity((entity_header *) n);
      remove_entity((entity_header *) n_2);
      add_entity(oct_find_fitting_node(_n, &n->offset, &n->size), (entity_header *) n);
      add_entity(oct_find_fitting_node(_n2, &n_2->offset, &n_2->size), (entity_header *) n_2);
    }
    oct_clean_tree(oct_get_nth_super(n->node, 5));
    state.center_node = n->node;
    usleep(100000);
  }
    
  return 0;
}
