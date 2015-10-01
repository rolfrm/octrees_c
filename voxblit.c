#include <iron/full.h>
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
  texture_asset_set_offset(tile22, vec2mk(0, -41));
  texture_asset_set_offset(tile3, vec2mk(0, - 77));
  oct_set_payload(n1, tile22);
  oct_set_payload(oct_get_relative(n1, vec3i_make(0,0,1)), tile22);
  oct_set_payload(oct_get_relative(n1, vec3i_make(0,-1,0)), tile22);
  oct_set_payload(oct_get_relative(n1, vec3i_make(4,-4,4)), tile22);
  oct_set_payload(oct_get_super(oct_get_relative(n1, vec3i_make(4,-6,4))), tile3);
  for(int i = 0; i < 4; i++)
    oct_set_payload(oct_get_relative(n1, vec3i_make(0,-2, 1 + i)), tile22);
  for(int i = 0; i < 4; i++)
    oct_set_payload(oct_get_relative(n1, vec3i_make(i, 0, 1)), tile22);
  for(int i = 0; i < 10; i++)
    oct_set_payload(oct_get_relative(n1, vec3i_make(3, 0, i)), tile22);
  while(true){
    renderer_render(rnd2, &state);
    event evt[32];
    u32 event_cnt = renderer_read_events(evt, array_count(evt));
    for(u32 i = 0; i < event_cnt; i++)
      switch(evt[i].type){
      case QUIT:
	return 0;
      case KEY:
	if(evt[i].key.sym == KEY_ESCAPE)
	  return 0;
	logd("%c\n", keysym_descr_from_keysym(evt[i].key.sym).charcode);
	break;
      default:
	continue;
      }
  }
  
  return 0;
}
