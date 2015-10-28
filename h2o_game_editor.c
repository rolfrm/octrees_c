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

int game_editor_main(void * _ed, struct MHD_Connection * con, const char * url,
		      const char * method, const char * version,
		      const char *upload_data, size_t * upload_data_size,
		      void ** con_cls){
  UNUSED(url); UNUSED(version); UNUSED(upload_data); UNUSED(upload_data_size);
  UNUSED(method); UNUSED(con_cls); 
  g_ed * ed = _ed;
  logd("Url: %s\n", url);
  int world_loc = strcmp("world_loc", url + 1);
  struct MHD_Response * response;
  logd("world loc: %i %i\n", ed->world->center_node.ptr2, *upload_data_size);
  
  if( world_loc == 0){
    response = MHD_create_response_from_data(sizeof(void *),
					     (void*) &ed->world->center_node.ptr2,
					     0,
					     MHD_NO);
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

			
