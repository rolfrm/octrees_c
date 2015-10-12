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

typedef struct{
  texture_asset ** tiles;
  int tiles_cnt;
  texture_asset ** sprites;
  int sprite_cnt;
  hash_table * loaded_nodes;
  hash_table * enemies;
  game_controller_state controller;
}game_data;


void game_data_add_tile(game_data * gd, texture_asset * texasset){
  int old_cnt = gd->tiles_cnt++;
  gd->tiles = ralloc(gd->tiles, gd->tiles_cnt * sizeof(texture_asset *));
  gd->tiles[old_cnt] = texasset;
}

void game_data_add_sprite(game_data * gd, texture_asset * sprite){
  int old_cnt = gd->sprite_cnt++;
  gd->sprites = ralloc(gd->sprites, (gd->sprite_cnt) * sizeof(texture_asset *));
  gd->sprites[old_cnt] = sprite;
}

enum{
  GD_GRASSY = 0,
  GD_ROCK = 1,
  GD_DIRT = 2,
  GD_ROCK_SMALL = 3,
  GD_TREE_1 = 7,
  GD_TREE_2 = 8,
  GD_TREE_3 = 9,
  GD_FOILAGE = 10
};

enum{
  GD_GUY = 0,
  GD_GUY_UPPER = 1,
  GD_FIREPLACE = 2,
  GD_BUG = 3,
  GD_CAT = 4,
  GD_SELECT_CUBE = 5

};

// lod_offset: how much above nominal LOD this node is. 
void load_node(oct_node * node, game_data * game_data, int lod_offset){
  if(ht_lookup(game_data->loaded_nodes, &node))
    return;
  logd("Loading node.. %i\n", node);
  int val = 0;
  ht_insert(game_data->loaded_nodes, &node, &val);
  int size = 1;
  for(int i = lod_offset; i> 0; i--){
    node = oct_get_sub(node, 0);
    size *= 2;
  }
  for(int i = 0; i < size; i++)
    for(int j = 0; j < size; j++){
      insert_tile(node, vec3i_make(i, 0, j), game_data->tiles[rand()&1]);
      if(rand() % 16 == 0){
	insert_tile(node, vec3i_make(i, 1, j), game_data->tiles[GD_TREE_1]);
	insert_tile(node, vec3i_make(i, 2, j), game_data->tiles[GD_TREE_2]);
	insert_tile(node, vec3i_make(i, 3, j), game_data->tiles[GD_TREE_3]);
	for(int k1 = -1; k1 <= 1; k1++)
	  for(int k2 = -1; k2 <= 1; k2++)
	    insert_tile(node, vec3i_make(i + k1, 4, j + k2), game_data->tiles[GD_FOILAGE]);
	insert_tile(node, vec3i_make(i, 5, j), game_data->tiles[GD_FOILAGE]);
      }else if(rand() % 4000 == 0){
	insert_entity(node, vec3mk(i, 1, j), vec3mk(1, 1, 1), game_data->sprites[GD_FIREPLACE]);
      
      }else if(rand() % 2024 == 0){
	/*int len = rand() % 10;
	for(int i2 = 0; i2 < len; i2++)
	  for(int j2 = 0; j2 < len - i2; j2++)
	    for(int k2 = 0; k2 < len - i2; k2++)
	      insert_tile(node, vec3i_make(i + k2 + i2/2, 1  + i2, j + j2 + i2/2), game_data->tiles[5]);
	*/
	for(int l = 0; l < 100; l++)
	  insert_tile(node, vec3i_make(i + l, 1  + l, j), game_data->tiles[5]);
      }else if(rand() % 2000 == 0){
	entity * n2 = insert_entity(node, vec3mk(i, 1, j), 
				    vec3mk(1, 1, 1), 
				    game_data->sprites[(rand() & 1) == 0 ? GD_BUG : GD_CAT]);
      int unused_1;
      ht_insert(game_data->enemies, &n2, &unused_1);
      }
    }
  /*
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
  */
}

game_data * load_game_data(game_renderer * rnd2){
  game_data * gd = alloc0(sizeof(game_data));

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
  texture_asset * tree1 = renderer_load_texture(rnd2, "../racket_octree/tree1.png");
  texture_asset * tree2 = renderer_load_texture(rnd2, "../racket_octree/tree2.png");
  texture_asset * tree3 = renderer_load_texture(rnd2, "../racket_octree/tree3.png");
  texture_asset * foilage = renderer_load_texture(rnd2, "../racket_octree/foilage.png");
  texture_asset * fireplace = renderer_load_texture(rnd2, "../racket_octree/fireplace.png");
  texture_asset * buggy = renderer_load_texture(rnd2, "../racket_octree/buggy.png");
  texture_asset * cat = renderer_load_texture(rnd2, "../racket_octree/cat.png");
  texture_asset * select_cube = renderer_load_texture(rnd2, "../racket_octree/select_cube.png");
  texture_asset_set_offset(tile22, vec2mk(0, -42));
  texture_asset_set_offset(tile25, vec2mk(0, -42));
  texture_asset_set_offset(tile5, vec2mk(0, -42));
  texture_asset_set_offset(tile4, vec2mk(0, -42));  
  texture_asset_set_offset(tile3, vec2mk(0, - 77));
  texture_asset_set_offset(tile1, vec2mk(0, -23));
  texture_asset_set_offset(rock_small, vec2mk(0, -19));
  texture_asset_set_offset(guy, vec2mk(0, -70));
  texture_asset_set_offset(guyupper, vec2mk(0, -42));
  texture_asset_set_offset(buggy, vec2mk(0, -42));
  texture_asset_set_offset(cat, vec2mk(0, -42));
  texture_asset_set_offset(select_cube, vec2mk(0, -42));
  texture_asset_set_size(guyupper, (vec2i){40, 40});
  texture_asset_set_offset(tree1, vec2mk(0, -42));
  texture_asset_set_offset(tree2, vec2mk(0, -42));
  texture_asset_set_offset(tree3, vec2mk(0, -42));
  texture_asset_set_offset(foilage, vec2mk(0, -42));
  texture_asset_set_offset(fireplace, vec2mk(0, -42));
  game_data_add_tile(gd, tile22);
  game_data_add_tile(gd, tile25);
  game_data_add_tile(gd, tile3);
  game_data_add_tile(gd, tile1);
  game_data_add_tile(gd, tile4);
  game_data_add_tile(gd, tile5);
  game_data_add_tile(gd, rock_small);
  game_data_add_tile(gd, tree1);
  game_data_add_tile(gd, tree2);
  game_data_add_tile(gd, tree3);
  game_data_add_tile(gd, foilage);
  game_data_add_sprite(gd, guy);
  game_data_add_sprite(gd, guyupper);
  game_data_add_sprite(gd, fireplace);
  game_data_add_sprite(gd, buggy);
  game_data_add_sprite(gd, cat);
  game_data_add_sprite(gd, select_cube);
  gd->loaded_nodes = ht_create(1024, 8, 4);
  gd->enemies = ht_create(1024, 8, 4);
  return gd;
}

void update_entities(oct_node * n1, game_data * gd, void (* f)(entity_header * eh, game_data * gd)){
  void rnd(oct_node * oc, float size, vec3 offset){
    UNUSED(size);UNUSED(offset);
    entity_list * lst = oct_get_payload(oc);
    if(lst == NULL) return;
    for(size_t i = 0; i < lst->cnt; i++){
      f(lst->entity[i], gd);
    }
  }
  oct_render_node(n1, 1, vec3mk(0, 0, 0), rnd);
}

void update_entity_positions(oct_node * n1){
  void rnd(oct_node * oc, float size, vec3 offset){
    UNUSED(size);UNUSED(offset);
    entity_list * lst = oct_get_payload(oc);
    if(lst == NULL) return;
    size_t cnt = lst->cnt;
    bool needs_update = false;
    for(size_t i = 0; i < cnt; i++){
      if(lst->entity[i]->type == OBJECT){
	entity * obj = (entity *) lst->entity[i];
	vec3 offset = obj->offset;
	needs_update |= 
	  offset.x < 0 || offset.x >= 1 
	  || offset.y < 0 || offset.y >= 1
	  || offset.z < 0 || offset.z >= 1;
      }
    }
    if(needs_update){
      entity_header ** objs = clone(lst->entity, sizeof(entity_header *) * lst->cnt);
      size_t cnt = lst->cnt;
      for(size_t i = 0; i < cnt; i++){
	if(objs[i]->type == OBJECT)
	  remove_entity(objs[i]);
      }
      for(size_t i = 0; i < cnt; i++){
	if(objs[i]->type == OBJECT){
	  entity * e = (entity *) objs[i];
	  oct_node * oc2 = oct_find_fitting_node(oc, &e->offset, &e->size);
	  add_entity(oc2, (entity_header *) e);
	}
      }
      dealloc(objs);
    }
  }
  oct_render_node(n1, 1, vec3mk(0, 0, 0), rnd);
}

void update_ai(entity_header * eh, game_data * gd){
  if(eh->type == OBJECT && ht_lookup(gd->enemies, &eh)){
    entity * n = (entity *) eh;
    entity_list * el = oct_get_payload(oct_get_relative(n->node, (vec3i){0, -1, 0}));
    bool standing_on_ground = false;
    if(el != NULL){
      for(size_t i = 0; i < el->cnt; i++)
	if(el->entity[i]->type == TILE)
	  standing_on_ground = true;
    }
    if(standing_on_ground == false ){
      vec3 newoffset = vec3_add(n->offset, vec3mk(0, -1, 0));
      bool collision = false;
      
      void check_collision(oct_node * oc, vec3 pos, vec3 size){
	UNUSED(pos);UNUSED(size);
	entity_list * lst = oct_get_payload(oc);
	if(lst == NULL) return;
	for(size_t i = 0; i < lst->cnt; i++)
	  collision |= lst->entity[i]->type == TILE;
      }
      oct_lookup_blocks(n->node, newoffset, n->size, check_collision);
      if(!collision) n->offset = newoffset;
    }else if ((rand() % 3) == 0){
      int dx = rand() % 3 - 1;
      int dy = rand() % 3 - 1;
      vec3 newoffset = vec3_add(n->offset, vec3mk(dx, 0, dy));
      bool collision = false;
      
      void check_collision(oct_node * oc, vec3 pos, vec3 size){
	UNUSED(pos);UNUSED(size);
	entity_list * lst = oct_get_payload(oc);
	if(lst == NULL) return;
	for(size_t i = 0; i < lst->cnt; i++)
	  collision |= lst->entity[i]->type == TILE;
      }
      oct_lookup_blocks(n->node, newoffset, n->size, check_collision);
      if(!collision) n->offset = newoffset;
      
    }
  }
}

static vec3i inv_iso(vec2 p, int y){
  
  return (vec3i){(p.x + p.y * 2 + 1) / 2,  y,  -(p.y * 2 - p.x + 1) / 2};
}

// Binary search to find group.
static i64 find_index(i64 * indexes, i64 size, i64 index){
  i64 distance = 100;
  i64 start = 0;
  i64 search_bounds = (size + 1) / 2;
  if(size > index){
    i64 first_index = indexes[index];
    distance = index - first_index;
    if(distance == 0)
      return index;
    if(distance < search_bounds){
      search_bounds = distance;
      start = first_index + distance;
    }
  }

  while(true){

    i64 next = search_bounds + start;
    logd("%i %i %i %i\n", search_bounds, start, next, indexes[next]);
    if(indexes[next] == index)
      return next;
    if(indexes[next] > index){
      search_bounds = - (search_bounds + 1) / 2;
    }
    if(indexes[next] < index)
      search_bounds = search_bounds / 2;
    if(search_bounds == 0)
      return -1;
    start = next;
  }
  UNREACHABLE();
  return -1;
}

/*static i64 find_index_interp(i64 * indexes, i64 size, i64 index){
  
  i64 a = index;
  i64 s1;
  i64 b;
  i64 d;
  i64 s2;
  i64 c;
  while(true){
    s1 = MIN(a, size - 1);
    b = indexes[s1];
    if(b == index) return s1;
    d = b - s1;
    s2 = MIN(MAX(0, b), size -1);
    c = indexes[s2];
    if(c == index) return s2;
    double delta = (s1 - s2) / (double) (b - c);
    a = (index - c) * delta + s1;
    logd("%i %i %i %i %i %f\n", s1, s2, b, c, d, delta);
    if(a == s1) return -1;
  }
  UNREACHABLE();
  return -1;
  }*/

static i64 find_index_interp2(i64 * indexes, i64 size, i64 index){
  i64 i1 = 0, i2 = size -1, a1 = indexes[i1], a2 = indexes[i2];
  i64 a = ((index - a1) * size) / (a2 - a1);
  i64 binerror = 2 * (size / 4);
  i64 pa = a;
  while(true){
    i64 new_index = a;
    if(new_index < 0)
      new_index = 0;
    else if (new_index >= size)
      new_index = size - 1;
    i64 new_value = indexes[new_index];
    i64 error = new_value - index;
    if(error == 0)
      return new_index;
    else if(error < 0){
      if(binerror < -error)
	error = -binerror;
      else
	binerror = -error;
    }else{
      if(binerror < error)
	error = binerror;
      else
	binerror = error;
    }
    pa = a;
    a = new_index - error;
    if(a == pa) return -1;
    if(binerror > 1)
      binerror /= 2;
  }
  UNREACHABLE();
  return -1;
}

static i64 find_indexe_interp4(i64 * indexes, i64 size, i64 * index, i64 index_cnt, i64 * out_buffer){
  for(i64 i = 0; i < index_cnt; i++){
    i64 thisindex = index[i];
  }
}

static i64 find_index_interp3(i64 * indexes, i64 size, i64 index){
  i64 a = size / 2;
  i64 binerror = size / 2;
  while(true){
    i64 new_index = a;
    if(new_index < 0) new_index = 0;
    else if(new_index >= size) new_index = size -1;
    i64 new_value = indexes[new_index];
    i64 error = new_value - index;
    if(error < 0)
      a = new_index + binerror;
    else if (error > 0)
      a = new_index - binerror;
    else
      return new_index;
    binerror = binerror >= 2 ? (binerror / 2) : 1;
  }
  UNREACHABLE();
  return -1;
}

int main(){
  UNUSED(find_index_interp3);
  i64 size = 1000000;
  i64 * indexes = aligned_alloc(16, size * sizeof(i64));// alloc(size * sizeof(i64));// = {4,5,6,7,9, 11 ,14 , 17,18,19, 30, 40, 41,42,43};
  indexes[0] = 5;
  for(i64 i = 1; i <size; i++)
    indexes[i] = indexes[i - 1] + (rand() % 1) + ((rand() % 100 == 0) ? 20 : 0) + 1;
  i64 idx = find_index(indexes, array_count(indexes), 4);
  logd("IDX: %i\n", idx);  
  i64 i1_total = 0, i2_total = 0;
  i64 ts = timestamp();
  for(i64 i = 0; i < size; i++){
    ASSERT(i == find_index_interp2(indexes, size, indexes[i]));
  }

  i64 ts2 = timestamp();
  for(i64 i = 0; i < size; i++){
    ASSERT(i == find_index_interp3(indexes, size, indexes[i]));
  }
  int comp(i64 * a, i64 * b){
    if(*a == *b) return 0;
    if(*a < *b)
      return -1;
    return 1;
  }
  i64 ts3 = timestamp();
  for(i64 i = 0; i < size; i++){
    ASSERT((indexes + i) == bsearch(indexes + i, indexes, size, sizeof(i64), (__compar_fn_t) comp) );
  }
  i64 ts4 = timestamp();
  logd("Tested find algorithms { fancy search: %i in %i µs, bin search: %i in %i µs, libc search %i µs }\n", i1_total, ts2 - ts, i2_total, ts3 - ts2, ts4 - ts3);

  return 0;
  int yoff = 0;
  game_renderer * rnd2 = renderer_load(500, 500, 28);
  oct_node * n1 = oct_create();
  world_state state = { n1 };
  game_data * gd = load_game_data(rnd2);
  
  entity * n = insert_entity(n1, vec3mk(1, -4, 0), vec3mk(1, 1, 1), gd->sprites[GD_GUY]);
  entity * n_2 = insert_entity(n1, vec3mk(1, -3, 0), vec3mk(1, 1, 1), gd->sprites[GD_GUY_UPPER]);
  entity * n_3 = insert_entity(n1, vec3mk(0, 0, 0), vec3mk(1, 1, 1), gd->sprites[GD_GUY]);
  //insert_entity(n1, vec3mk(0, 0, 0), vec3mk(1, 1, 1), gd->sprites[GD_GUY]);
  tile * orig = insert_tile(n1, vec3i_make(0, 0, 0), gd->tiles[GD_TREE_1]);
  entity * ne = insert_entity(n1, vec3mk(-5, -4, -5), vec3mk(1, 1, 1), gd->sprites[GD_BUG]);
  entity * select_cube = insert_entity(n1, vec3mk(-2, -8, -4), vec3mk(1, 1, 1), gd->sprites[GD_SELECT_CUBE]);
  int unused_1;
  ht_insert(gd->enemies, &ne, &unused_1);
  ht_insert(gd->enemies, &n_3, &unused_1);
  while(true){
    vec2i p = renderer_get_mouse_position(rnd2);
    vec2 p2 = vec2mk(p.x / 28.0, (p.y - 40) / 28.0 );
    vec3i p3 =inv_iso(p2, yoff);
    select_cube->offset = vec3i_to_vec3(p3);
    remove_entity((entity_header *)select_cube);
    logd(" +++ "); vec3i_print(p3);logd("\n");
    oct_node * oc2 = oct_find_fitting_node(n->node, &select_cube->offset, &select_cube->size);
    add_entity(oc2, (entity_header *) select_cube);


    vec3 offset = oct_node_offset(n_2->node, orig->node);
    logd("Offset: "); vec3_print(offset);vec3_print(n->offset);logd("\n");
    oct_node * super_1 = oct_get_nth_super(oct_get_relative(n->node, (vec3i){0, (int)-offset.y, 0}), 4);
    for(int i = -4; i <= 4; i++)
      for(int j = -4; j <= 4; j++){
	oct_node * r = oct_get_relative(super_1, (vec3i){i, 0, j});
	load_node(r, gd, 4);
      }
    super_1 = oct_get_nth_super(oct_get_relative(n->node, (vec3i){0, 0, 0}), 4);
    for(int i = -2; i <= 2; i++)
      for(int j = -2; j <= 2; j++){
	oct_node * r = oct_get_relative(super_1, (vec3i){i, 0, j});
	update_entities(r, gd, update_ai);
	update_entity_positions(r);
      }
    UNUSED(state);
    renderer_render(rnd2, &state);
    event evt[32];
    u32 event_cnt = renderer_read_events(evt, array_count(evt));
    gd->controller = renderer_game_controller();
    for(u32 i = 0; i < event_cnt; i++){
      oct_node * node1;
    entity_list * lst;
      switch(evt[i].type){
      case QUIT:
	return 0;
      case KEY:
	break;
      case MOUSE_WHEEL:
	logd("WHEELS! %i %i\n", evt[i].mouse_wheel.delta_x, evt[i].mouse_wheel.delta_y );
	yoff = yoff + evt[i].mouse_wheel.delta_y;
	break;
      case MOUSE_BUTTON:
	node1 = select_cube->node;
	remove_entity((entity_header *)select_cube);
	while((lst = oct_get_payload(node1)))
	  remove_entity(lst->entity[0]);
	add_entity(node1, (entity_header *) select_cube);
	break;
      default:
	continue;
      }
    }
    game_controller_state_print(gd->controller);logd("\n");
    entity_list * el = oct_get_payload(oct_get_relative(n->node, (vec3i){0, -1, 0}));
    bool standing_on_ground = false;
    if(el != NULL){
      for(size_t i = 0; i < el->cnt; i++)

	if(el->entity[i]->type == TILE)
	  standing_on_ground = true;
    }
    if(standing_on_ground){
      vec3 mv = vec3mk(gd->controller.axes[0] * 1, 
		       gd->controller.axes[3] * 1, 
		       gd->controller.axes[1] * 1);
      vec3 newoffset = vec3_add(n->offset, mv);
      bool collision = false;
      
      void check_collision(oct_node * oc, vec3 pos, vec3 size){
	UNUSED(pos);UNUSED(size);
	entity_list * lst = oct_get_payload(oc);
	if(lst == NULL) return;
	for(size_t i = 0; i < lst->cnt; i++)
	  collision |= lst->entity[i]->type == TILE;
      }
      bool bottom_collision, top_collision;
      oct_lookup_blocks(n->node, newoffset, n->size, check_collision);
      bottom_collision = collision;
      collision = false;
      oct_lookup_blocks(n_2->node, newoffset, n_2->size, check_collision);
      top_collision = collision;
      if(bottom_collision && !top_collision){
	newoffset = vec3_add(newoffset, vec3mk(0,1,0));
	collision = false;
	oct_lookup_blocks(n->node, newoffset, n->size, check_collision);
	oct_lookup_blocks(n_2->node, newoffset, n->size, check_collision);
	bottom_collision = false;
      }
      
      if(!collision && !bottom_collision){
	n->offset = newoffset;
	n_2->offset = newoffset;
      }
    }else{
      // handle gravity.
      vec3 newoffset = vec3_add(n->offset, vec3mk(0, -1, 0));
      n->offset = newoffset;
      n_2->offset = newoffset;
    }
    oct_clean_tree(oct_get_nth_super(n->node, 5));
    state.center_node = n->node;
    usleep(100000);
  }
    
  return 0;
}
