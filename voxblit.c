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
  oct_node * n1 = oct_create();
  oct_node * n2 = oct_get_super(n1);
  oct_node * n3 = oct_get_super(n2);
  oct_node * n4 = oct_get_super(n3);
  oct_node * n5 = oct_get_super(n4);
  oct_node * n15 = oct_get_super(n5);
  oct_node * n16 = oct_get_super(n15);
  logd("N4 index: %i %i %i %i %i\n", oct_index(n1), oct_index(n2), oct_index(n3), oct_index(n4), oct_index(n5));
  ASSERT(n1 == oct_get_sub(n2, oct_index(n1)));
  oct_node * n6 = oct_get_relative(n1, vec3i_make(10,0,0));
  ASSERT(n1 != n6);
  oct_node * n7 = oct_get_relative(n6, vec3i_make(-10,0,0));
  oct_node * n8 = oct_get_relative(n7, vec3i_make(10,0,0));
  ASSERT(n7 == n1);
  ASSERT(n8 == n6);
  
  hash_table * ht = ht_create(1024, 8, 8);
  size_t x = 1;
  ht_insert(ht, &n1, &x);
  for(int i = -5; i < 5; i++){
    for(int j = -5; j < 5; j++){
      oct_node * n = oct_get_relative(n1, vec3i_make(i,j,0));
      size_t x2 = i * i * j * j;
      ht_insert(ht, &n, &x2);
    }
  }

  oct_node * render_node = n16;
  vec3 offset = oct_get_super_offset(n1, render_node);
  float super_size = oct_get_super_size(n1, render_node);
  
  void rnd(oct_node * oc, float s, vec3 o){  
    size_t * x1 = ht_lookup(ht, &oc);
    if(x1 != NULL){
      logd("%p %f %4i ", oc, s * super_size, (x1 != NULL ? *x1 : 0)); 
      vec3_print(vec3_scale(o, super_size)); 
      logd("\n");
    }
  }
  
  oct_render_node(render_node, 1.0, vec3mk(0.0, 0.0, 0.0), rnd);

  vec3_print(offset);logd(" %f ", super_size);logd("\n");

  void collision_node(oct_node * oc, vec3 pos, vec3 size){
    logd("Collision with: %p %p", oc, ht_lookup(ht, &oc));vec3_print(pos);vec3_print(size);logd("\n");
  }
  oct_lookup_blocks(n1, vec3mk(0.0, 0.0, 0.0), vec3mk(2.0,2.0,1.0), collision_node);

  game_renderer * rnd2 = renderer_load(600, 600);
  world_state state = { n1 };
  texture_asset * tile22 = renderer_load_texture(rnd2, "../racket_octree/tile22.png");
  texture_asset * tile3 = renderer_load_texture(rnd2, "../racket_octree/tile2x2.png");
  texture_asset * tile1 = renderer_load_texture(rnd2, "../racket_octree/tile1.png");
  texture_asset * guy = renderer_load_texture(rnd2, "../racket_octree/guy.png");
  texture_asset * tile4 = renderer_load_texture(rnd2, "../racket_octree/tile23.png");
  texture_asset_set_offset(tile22, vec2mk(0, -41));
  texture_asset_set_offset(tile4, vec2mk(0, -41));
  texture_asset_set_offset(tile3, vec2mk(0, - 77));
  texture_asset_set_offset(tile1, vec2mk(0, -23));
  texture_asset_set_offset(guy, vec2mk(0, -23));
  vec3 p = vec3mk(1.0, 1.0, 1.0);
  vec3 s = vec3mk(0.4, 0.4, 0.4);
  oct_find_fitting_node(n1, &p, &s);
  int size = 500;
  for(int i = -size; i < size; i++)
    for(int j = -size; j < size; j++){
      insert_tile(n1, vec3i_make(i, 0, j), tile22);
      if(rand() % 40 == 0){
	insert_tile(n1, vec3i_make(i, 1, j), tile4);
	if(rand() % 1 == 0){
	  insert_tile(n1, vec3i_make(i, 2, j), tile4);
	  if(rand() % 1 == 0){
	    insert_tile(n1, vec3i_make(i, 3, j), tile4);
	    if(rand()% 1 == 0){
	      insert_tile(n1, vec3i_make(i + 1, 3, j), tile4);
	      insert_tile(n1, vec3i_make(i + 2, 3, j), tile4);
	      insert_tile(n1, vec3i_make(i + 2, 2, j), tile4);
	      insert_tile(n1, vec3i_make(i + 2, 1, j), tile4);
	    }
	  }
	}
      }
    }
  //entity * n = insert_entity(n1, vec3mk(0, 1, 0), vec3mk(1, 1, 1), tile1);
  //entity * n = insert_entity(oct_get_sub(n1,0), vec3mk(0, 3, 0), vec3mk(1, 1, 1), tile1)
  entity * n = insert_entity(oct_get_sub(n1,0), vec3mk(0, 3, 0), vec3mk(1, 1, 1), guy);
  while(true){
    {
      for(int i = -1; i < 2; i++)
	for(int j = -1; j < 2; j++)
	  for(int k = -1; k < 2; k++)
	    oct_get_relative(n->node, (vec3i){i,j,k});
    }
    size_t cnt = 0;
    void cnt_cells(oct_node * oc, vec3 pos, vec3 size){
      UNUSED(pos);UNUSED(size);UNUSED(oc);
      if(oc == n->node) return;
      if(size.x != n->size.x) return;
      if(oct_get_payload(oc) != NULL) return;
      cnt++;
    }
    oct_lookup_blocks(n->node, n->offset, n->size,  cnt_cells);
    
    satelite sat[cnt];
    memset(&sat,0, sizeof(sat));
    oct_node * nodes[cnt];
    cnt = 0;
    void load_sat(oct_node * oc, vec3 pos, vec3 size){
      if(oc == n->node) return;
      if(size.x != n->size.x) return;
      if(oct_get_payload(oc) != NULL){ 
	logd("Payload \n");
	return;

      }
      nodes[cnt] = oc;
      sat[cnt++] = (satelite){SATELITE, NULL, pos,n};
    }
    
    oct_lookup_blocks(n->node, n->offset, n->size,  load_sat);
    for(size_t i = 0; i < cnt; i++){
      add_entity(nodes[i], (entity_header *) &sat[i]);
    }
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
	//if(evt[i].key.sym == KEY_ESCAPE)
	//  return 0;
	//logd("%i %i\n", evt[i].key.sym, evt[i].key.scancode);
	break;
      default:
	continue;
      }

    game_controller_state_print(gcs);logd("\n");
    for(size_t i = 0; i < cnt; i++){
      remove_entity((entity_header *) &sat[i]);
    }
    vec3 mv = vec3mk(gcs.axes[0] * 1, gcs.axes[3] * 1, gcs.axes[1] * 1);
    vec3 newoffset = vec3_add(n->offset, mv);
    bool collision = false;
    void check_collision(oct_node * oc, vec3 pos, vec3 size){
      UNUSED(pos);UNUSED(size);
      collision |= oc != n->node && oct_get_payload(oc);
    }
    
    oct_lookup_blocks(n->node, newoffset, n->size, check_collision);
    
    if(!collision)
      n->offset = newoffset;
    update_entity(n);
 
    oct_clean_tree(oct_get_nth_super(n->node, 5));
    state.center_node = oct_get_super(n->node);//oct_get_nth_super(n1,10);
    usleep(100000);
  }
    
  return 0;
}
