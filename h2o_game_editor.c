#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <iron/utils.h>
#include <iron/log.h>
#include <iron/fileio.h>
#include <iron/mem.h>
#include <iron/linmath.h>

#include <microhttpd.h>
#include "vec3i.h"
#include "oct_node.h"
#include "octree.h"
#include "game_state.h"
#include "game_editor.h"
typedef struct{
  struct MHD_Daemon * d;
  world_state * world;
}g_ed;

size_t rel_cb(void * cls, uint64_t pos, int * buf, size_t max){
  UNUSED(cls);UNUSED(pos);UNUSED(buf);UNUSED(max);
  logd("%i %i %i %i %i %i\n", max, buf[0], buf[1],buf[2], buf[3], buf[4]);
  return max;
}

bool compare_strs(char * s1, char * s2){
  while(*s1 == *s2 && *s1 && *s2){
    s1++; s2++;
  }
  if(*s1 == 0) return true;
  return false;
    
}

int game_editor_main(void * _ed, struct MHD_Connection * con, const char * url,
		      const char * method, const char * version,
		      const char *upload_data, size_t * upload_data_size,
		      void ** con_cls){
  UNUSED(url); UNUSED(version); UNUSED(upload_data); UNUSED(upload_data_size);
  UNUSED(method); UNUSED(con_cls); 
  g_ed * ed = _ed;
  logd("Url: %s %s %s\n", url, method, version);
  bool world_loc = compare_strs((char *) "world_loc", (char *)url + 1);
  bool rel_loc = compare_strs((char *) "rel_loc", (char *) url + 1);
  bool lookup_loc = compare_strs((char *) "lookup", (char *) url + 1);
  struct MHD_Response * response;
  logd("world loc: %i %i \n", world_loc, rel_loc);
  
  if(world_loc){
    response = MHD_create_response_from_data(sizeof(void *),
					     (void*) &ed->world->center_node.ptr2,
					     0, MHD_NO);
  }else if(rel_loc | lookup_loc){
    char * end;
    size_t loc = strtol(url + 1 + (rel_loc ? 7 : 6), &end, 10);
    int x = strtol(end + 1, &end, 10);
    int y = strtol(end + 1, &end, 10);
    int z = strtol(end + 1, &end, 10);
    oct_node n = ed->world->center_node;
    n.ptr2 = (void *) loc;
    if(rel_loc){
      n = oct_get_relative(n, (vec3i){x, y, z});
      loc = (size_t)n.ptr2;
      response = MHD_create_response_from_data(sizeof(void *),
					       (void*) &loc,
					       0, MHD_NO);
    }else{
      vec3 pos = vec3mk(0, 0, 0);
      vec3 size = vec3mk(x, y, z);
      logd("%i %i %i\n", x, y, z);
      int * tiles = alloc0(x * y * z * sizeof(int));
      //memset(tiles, 0, x * y * z * sizeof(int));
      void _cb(oct_node node, vec3 p, vec3 s){
	UNUSED(node);
	if(s.x == size.x){
	  int _x =  -(int)p.x;
	  int _y =  -(int)p.y;
	  int _z =  -(int)p.z;
	  logd("End lookup.. %i %i %i %i\n", _x, _y, _z, _x + x*_y + x*y*_z);
	  tiles[_x + x*_y + x*y*_z] = 1;
	}
      }
      UNUSED(_cb); UNUSED(pos);
      oct_lookup_blocks(n, pos, size, _cb);
      logd("End lookup..\n");
      response = MHD_create_response_from_data(x * y * z * sizeof(int),(void*) tiles,0, MHD_YES);
    }
  }else{
    
    char * file = (char *) "page.html";
    char * pg = read_file_to_string(file);
    
    response = MHD_create_response_from_data(strlen(pg),
					     pg,
					     1,
					     MHD_NO);
  }
  
  int ret = MHD_queue_response(con, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return ret;
}

game_editor start_game_editor(world_state * world){
  UNUSED(world);
  g_ed * ed = alloc0(sizeof(g_ed));
  
  ed->d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8000, NULL, NULL, &game_editor_main, ed,
			   MHD_OPTION_END);
  ed->world = world;
  return ed;
}

void stop_game_editor(game_editor _ed){
  UNUSED(_ed);
}

			
