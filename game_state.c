#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iron/types.h>
#include <iron/mem.h>
#include "oct_node.h"
#include "iron/linmath.h"
#include "iron/log.h"
#include "vec3i.h"
#include "octree.h"
#include "game_state.h"

void add_entity(oct_node * oc, entity_header * eh){
  entity_list * ceh = oct_get_payload(oc);
  if(ceh == NULL){
    ceh = alloc0(sizeof(entity_header));
    oct_set_payload(oc, ceh);
  }
  ceh->cnt += 1;
  ceh->entity = realloc(ceh->entity, ceh->cnt * sizeof(entity_header *));
  ceh->entity[ceh->cnt - 1] = eh;
  eh->node = oc;
}

void remove_entity(oct_node * oc, entity_header * eh){
  entity_list * ceh = oct_get_payload(oc);
  ASSERT(ceh != NULL);
  for(size_t i = 0; i < ceh->cnt; i++){
    if(ceh->entity[i] == eh){
      memmove(ceh->entity + i - 1, ceh->entity + i + 1, (ceh->cnt - i - 1) * sizeof(entity_header *));
      break;
    }
  }
  ceh->cnt -= 1;
  if(ceh->cnt == 0){
    dealloc(ceh->entity);
    ceh->entity = NULL;
  }
  else
    ceh->entity = realloc(ceh->entity, ceh->cnt);
  eh->node = NULL;
}
