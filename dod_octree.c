#include <iron/full.h>

// Opaque octree node type.
//typedef struct _oct_node oct_node;
#define realloc_column(table,column) table->column = ralloc(table->column, sizeof(table->column[0]) * table->capacity);

typedef struct _octree{
  // points up. cnt: count / 8
  i64 * super_group;
  // points down. cnt: count
  // values are group and not item, so max_value(sub_group) is max_value(super_group)/8.
  i64 * sub_group; 
  // payload for node. cnt: count
  void ** payload; 
  
  u64 cnt, capacity;
}octree;

// Generates a group of nodes and returns 
static i64 gen_group(octree * tree){
  if(tree->cnt == tree->capacity){
    tree->capacity = tree->capacity == 0 ? 64 : (tree->capacity * 2);
    realloc_column(tree, sub_group);
    realloc_column(tree, payload);
    tree->super_group = ralloc(tree->super_group, tree->capacity / 8 * sizeof(tree->super_group[0]));
    logd("Resize octree %p\n", tree->capacity);
  }

  u64 prev_cnt = tree->cnt++;
  tree->sub_group[prev_cnt] = -1;
  tree->payload[prev_cnt] = NULL;
  return prev_cnt;
}

static void copy_defragmented(octree * src, octree * dst){
  dst->cnt = 0;
  i64 current_group = 0;
  while(src->super_group[current_group] != -1)
    current_group = src->super_group[current_group] / 8;
  logd("Found group: %i", current_group);
  i64 fix_group(i64 grp, i64 super){
    i64 items[8];
    for(int i = 0; i < 8; i++)
      items[i] = gen_group(dst);
    
    dst->super_group[items[0] / 8] = super;
    for(int i = 0; i < 8;i++){
      i64 item = i + grp;
      dst->payload[items[i]] = src->payload[item]; 
      if(src->sub_group[item] != -1)
	dst->sub_group[items[i]] = fix_group(src->sub_group[item] * 8, item);
    }
    return items[0] / 8;
  }
  fix_group(current_group, -1);
}


typedef struct _oct_node{
  octree * tree;
  i64 item;
}oct_node;


oct_node oct_defragment(oct_node oc){
  ASSERT(false);
  // this code does not work
  // it should probably be a part of some more general
  // cleanup procedure where unused nodes can be pruned.
  // It probably does not need to be called often. Only on save / load
  // or when memory limit is reached.
  octree * ntree = alloc0(sizeof(octree));
  copy_defragmented(oc.tree, ntree);
  dealloc(oc.tree->super_group);
  dealloc(oc.tree->sub_group);
  dealloc(oc.tree->payload);
  
  oc.tree->super_group = ntree->super_group;
  oc.tree->sub_group = ntree->sub_group;
  oc.tree->payload = ntree->payload;
  oc.tree->cnt = ntree->cnt;
  oc.tree->capacity = ntree->capacity;
  dealloc(ntree);
  return oc;
}

int oct_index(oct_node oc){
  return oc.item - (oc.item / 8) * 8;
}

oct_node oct_node_empty = {0};
int oct_node_compare(oct_node a, oct_node b){
  return a.tree == b.tree && a.item == b.item;
}

bool oct_node_null(oct_node oc){
  return oct_node_compare(oc, oct_node_empty);
}

oct_node oct_get_sub(oct_node node, int idx){
  octree * tree = node.tree;
  i64 sub = tree->sub_group[node.item];
  if(sub == -1){
    i64 newitems[8];
    for(size_t i = 0 ; i < 8; i++){
      newitems[i] = gen_group(tree);
    }
    tree->super_group[newitems[0]/8] = node.item;
    tree->sub_group[node.item] = newitems[0] / 8;
    sub = tree->sub_group[node.item];
  }
  node.item = sub * 8 + idx;
  return node;
}

bool oct_has_sub(oct_node node){
  return node.tree->sub_group[node.item] != -1;
}

oct_node oct_peek_sub(oct_node node, int idx){
  octree * tree = node.tree;
  i64 sub = tree->sub_group[node.item];
  if(sub == -1)
    return oct_node_empty;
  node.item = sub * 8 + idx;
  return node;
}

void oct_print(oct_node node){
  octree * tree = node.tree;
  for(size_t i = 0 ; i < tree->cnt; i++){
    logd("%i:  %i   %i   %p\n",i,  tree->super_group[i/8], tree->sub_group[i], tree->payload[i]);
  }
}


oct_node oct_get_super(oct_node node){
  octree * tree = node.tree;
  i64 gp_idx = node.item / 8;
  i64 super_grp = tree->super_group[gp_idx];
  //logd("find grp %i %i %i\n", super_grp, gp_idx, node.item);
  if(super_grp == -1){
    i64 offset = (node.item / 8 & 1) ? 0 : 7;
    i64 newitems[8];
    for(size_t i = 0 ; i < 8; i++){
      newitems[i] = gen_group(tree);
     
    }
    tree->super_group[newitems[0] /8] = -1;
    tree->super_group[node.item / 8] = newitems[offset];
    tree->sub_group[newitems[offset]] = gp_idx;
  }
  node.item = tree->super_group[node.item / 8];
  return node;
}

oct_node oct_peek_super(oct_node node){
  i64 grp = node.item / 8;
  i64 super = node.tree->super_group[grp];
  if(super == -1)
    return oct_node_empty;
  node.item = super;
  return node;
}

oct_node oct_create(){
  oct_node oc;
  oc.item = 0;
  oc.tree = alloc0(sizeof(octree));
  i64 idxes[8];
  for(i64 i = 0 ; i < 8; i++){
    idxes[i] = gen_group(oc.tree);
    oc.tree->super_group[idxes[i]] = -1;
  }
  return oc;
}


void oct_delete(oct_node node){
  node.tree->sub_group[node.item] = -1;
}

void oct_delete_sub(oct_node node, int idx){
  UNUSED(idx);
  node.tree->sub_group[node.item] = -1;
}

void * oct_get_payload(oct_node node){
  return node.tree->payload[node.item];
}

void oct_set_payload(oct_node node, void * payload){
  node.tree->payload[node.item] = payload;
}
