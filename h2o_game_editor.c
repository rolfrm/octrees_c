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
}g_ed;

int game_editor_main(void * ed, struct MHD_Connection * con, const char * url,
		      const char * method, const char * version,
		      const char *upload_data, size_t * upload_data_size,
		      void ** con_cls){
  UNUSED(url); UNUSED(version); UNUSED(upload_data); UNUSED(upload_data_size);
  UNUSED(method); UNUSED(con_cls); UNUSED(ed);
  char * file = (char *) "page.html";
  char * pg = read_file_to_string(file);
  struct MHD_Response * response = MHD_create_response_from_data(strlen(pg),
								 pg,
								 1,
								 MHD_NO);
  int ret = MHD_queue_response(con, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return ret;
}

game_editor start_game_editor(world_state * world){
  UNUSED(world);
  g_ed * ed = alloc0(sizeof(g_ed));
  
  ed->d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8000, NULL, NULL, &game_editor_main, ed,
			   MHD_OPTION_END);
  return ed;
}

void stop_game_editor(game_editor _ed){
  UNUSED(_ed);
}

			
