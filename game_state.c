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
    ceh = alloc0(sizeof(entity_list));
    oct_set_payload(oc, ceh);
  }
  int prevcnt = ceh->cnt++;
  ceh->entity = ralloc(ceh->entity, ceh->cnt * sizeof(entity_header *));
  ceh->entity[prevcnt] = eh;
  eh->node = oc;
}

void remove_entity(entity_header * eh){
  entity_list * ceh = oct_get_payload(eh->node);
  ASSERT(ceh != NULL);
  size_t prevcnt = ceh->cnt--;
  for(size_t i = 0; i < prevcnt; i++){

    if(ceh->entity[i] == eh){
      memmove(ceh->entity + i, ceh->entity + i + 1, (prevcnt - i) * sizeof(entity_header *));
      break;
    }
  }
  if(ceh->cnt == 0){
    dealloc(ceh->entity);
    ceh->entity = NULL;
    oct_set_payload(eh->node, NULL);
    dealloc(ceh);
  }else{
    //ceh->entity = ralloc(ceh->entity, ceh->cnt * sizeof(entity_header *));
  }
  eh->node = NULL;
}
