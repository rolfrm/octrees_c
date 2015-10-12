
// Opaque octree node type.
//typedef struct _oct_node oct_node;

typedef struct _octree{
  i64 * group; // index. number of this is node_count / 8.
  i64 * sub_group;  // number of these is node_count
  i64 * super_group; // super group.
  void ** payload; // number of these is also node_count
  
  u64 node_count;
  u64 capacity;
}octree;

struct _oct_node{
  octree * tree;
  i64 group;
  i8 index;
  i64 last_index;
};

oct_node * oct_get_sub(oct_node * node, int idx){
  
}


oct_node * oct_peek_sub(oct_node * node, int idx){

}


oct_node * oct_get_super(oct_node * node){

}

oct_node * oct_peek_super(oct_node * node){
  i64 idx = oct_find(node->tree->group, node->tree->node_count, node->group);
  i64 super_group = node->tree->sub_grooup[idx];
  i64 idx2 = oct_find(node->tree->group, node->tree->node_count, super_group);
  i64 start_pt = idx2 * 8;
  for(i64 s = start_pt; < start_pt + 8; s++)
}

// Generates a group of nodes and returns 
static i64 gen_group(octree * tree){
  if(tree->node_count == tree->capacity){
    size_t oldcap = tree->capacity;
    tree->capacity = oldcap == 0 ? 16 : oldcap * 2;
    tree->group = ralloc(tree->group, tree->capacity * sizeof(tree->group[0]));
    tree->sub_group = ralloc(tree->sub_group, tree->capacity * sizeof(tree->sub_group[0]) * 8);
    tree->payload = ralloc(tree->payload, tree->capacity * sizeof(tree->payload[0]) * 8);
  }
  
  u64 prev_cnt = tree->node_count++;
  i64 ngroup = tree->node_count == 0 : 0 ? (tree->group[tree->node_count - 1] + 1);
  tree->group[tree->node_count] = ngroup;

  return prev_cnt;
}


oct_node * oct_create(){
  oct_node * oc = alloc0(sizeof(oct_node));
  oc->tree = alloc0(sizeof(octree));
  oc->item = 0;
  gen_group(oc->tree);
  oc->tree->super_group[0] = -1;
  return oc;
}


void oct_delete(oct_node * node){
  UNUSED(node);
}

void oct_delete_sub(oct_node * node, int idx){
  UNUSED(node);UNUSED(idx);
}


void * oct_get_payload(oct_node * node){
  return node->tree->payload[node->item];
}


void oct_set_payload(oct_node * node, void * payload){
  node->tree->payload[node->item] = payload;
}
