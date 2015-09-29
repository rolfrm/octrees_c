#include <stdio.h>
#include <stdbool.h>
#include <iron/mem.h>
#include <iron/log.h>

typedef struct _oct_node oct_node;

struct _oct_node{
  oct_node * super;
  oct_node * sub[8];
  void * payload;
};

oct_node * oct_get_sub(oct_node * oc, int idx){
  ASSERT(idx < 8 && idx >= 0);
  if(oc->sub[idx] == NULL){
    oc->sub[idx] = alloc0(sizeof(oct_node));
    oc->sub[idx]->super = oc;
  }

  return oc->sub[idx];
}

bool oct_has_sub(oct_node * oc, int idx){
  ASSERT(idx < 8 && idx >= 0);
  return oc->sub[idx] != NULL;
}

oct_node * oct_get_super(oct_node * oc){
  static int parent_it = 0;
  if(!oc->super){
    oc->super = alloc0(sizeof(oct_node));
    oc->super->sub[parent_it] = oc;
    parent_it = 7 - parent_it; 
  }
  return oc->super;
}

bool oct_has_super(oct_node * oc){
  return oc->super != NULL;
}

oct_node * oct_create(){
  return alloc0(sizeof(oct_node));
}

void oct_delete(oct_node * oc){
  for(int i = 0; i < 8; i++)
    if(oc->sub[i] != NULL){
      oct_delete(oc->sub[i]);
      oc->sub[i] = NULL;
    }
  dealloc(oc);
}

void * oct_get_payload(oct_node * node){
  return node->payload;
}

void oct_set_payload(oct_node * node, void * payload){
  node->payload = payload;
}
