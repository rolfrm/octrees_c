#include <iron/full.h>
#include "oct_node.h"
#include "vec3i.h"

int oct_index(oct_node oc){
  oct_node s = oct_peek_super(oc);
  ASSERT(!oct_node_null(s));
  for(int i = 0; i < 8; i++)
    if(oct_node_compare(oct_peek_sub(s, i), oc))
      return i;
  ERROR("Not a child node of that element\n");
  return -1;
}

static int vec3i_to_index(vec3i v){
  return v.x + v.y * 2 + v.z * 4;
}

static vec3i vec3i_from_index(int idx){
  ASSERT(idx >= 0 && idx < 8);
  return vec3i_make(idx & 1, (idx / 2) & 1, (idx / 4) & 1);
}

static vec3i vec3i_half(vec3i v){
  return vec3i_make(v.x >> 1, v.y >> 1, v.z >> 1);
}

oct_node oct_get_relative(oct_node oc, vec3i v){
  if(v.x == 0 && v.y == 0 && v.z == 0)
    return oc;
  oct_node super = oct_get_super(oc);
  vec3i idx = vec3i_from_index(oct_index(oc));
  vec3i super_offset = vec3i_half(vec3i_add(idx, v));
  if(super_offset.x != 0 || super_offset.y != 0 || super_offset.z != 0)
    super = oct_get_relative(super, super_offset);
  return oct_get_sub(super, vec3i_to_index(vec3i_bitand(vec3i_add(v, idx), vec3i_make(1,1,1))));
}

void oct_render_node(oct_node oc, float size, vec3 offset, 
		     void (* f)(oct_node oc, float size, vec3 offset)){
  f(oc, size, offset);
  int idxes[] = {1, 0, 5, 4, 3, 2, 7, 6};
  ASSERT(array_count(idxes) == 8);
  float size2 = size * 0.5;
  for(size_t i = 0; i < array_count(idxes); i++){
    int idx = idxes[i];
    oct_node n = oct_peek_sub(oc, idx);
    if(!oct_node_null(n)){
      vec3 off = vec3i_to_vec3(vec3i_from_index(idx));
      oct_render_node(n, size2, vec3_add(offset, vec3_scale(off, size2)), f);
    }
  }
}

static vec3 to_super_coords (int idx, vec3 coords){
  vec3 offset = vec3i_to_vec3(vec3i_from_index(idx));
  return vec3_scale(vec3_add(coords, offset), 0.5);
}

static vec3 to_super_size(vec3 size){
  return vec3_scale(size, 0.5);
}

static vec3 to_sub_coords(int idx, vec3 coords){
  vec3 offset = vec3i_to_vec3(vec3i_from_index(idx));
  return vec3_sub(vec3_scale(coords, 2), offset);
}

static vec3 to_sub_size(vec3 size){
  return vec3_scale(size, 2.0);
}

static vec3 _oct_get_super_offset(oct_node node, oct_node super){
  if(oct_node_compare(super,node))
    return vec3mk(0,0,0);
  int idx = oct_index(node);
  vec3 par = _oct_get_super_offset(oct_get_super(node), super);
  return to_sub_coords(idx, par);
}

vec3 oct_get_super_offset(oct_node node, oct_node super){
  { // Sanity check
    oct_node n = node;
    for(; !oct_node_compare(n, super) && !oct_node_null(oct_peek_super(n)); n = oct_get_super(n));
    ASSERT(oct_node_compare(n, super));
  }
  return _oct_get_super_offset(node, super);
}

float oct_get_super_size(oct_node node, oct_node super){
  { // Sanity check
    oct_node n = node;
    for(; !oct_node_compare(n,super) && !oct_node_null(oct_peek_super(n)); n = oct_get_super(n));
    ASSERT(oct_node_compare(n, super));
  }
  float f = 1;
  while(!oct_node_compare(node, super)){
    f = f * 2;
    node = oct_get_super(node);
  }
  return f;
}

static void lookup_blocks(oct_node node, vec3 position, vec3 size, 
		   void (* cb)(oct_node node, vec3 pos, vec3 size), 
		   int idx){
  vec3 end = vec3_add(position, size);
  bool collision = 
    end.x > 0 && end.y > 0 && end.z > 0 
    && position.x < 1 && position.y < 1 && position.z < 1;
  if(collision){
    cb(node, position, size);
    for(int i = 0; i < 8; i++){
      if(i == idx) continue;
      oct_node n = oct_peek_sub(node, i);
      if(oct_node_null(n)) continue;
      lookup_blocks(n, to_sub_coords(i, position), to_sub_size(size), cb, -2);
    }
  }
  oct_node super = oct_peek_super(node);
  if(idx == -2 || oct_node_null(super)) return;
  int idx2 = oct_index(node);
  lookup_blocks(super, 
		to_super_coords(idx2, position), to_super_size(size), cb, idx2);
}

void oct_lookup_blocks(oct_node node, vec3 position, vec3 size, 
		       void (* cb)(oct_node node, vec3 pos, vec3 size)){
  lookup_blocks(node, position, size, cb, -1);
}


oct_node oct_find_fitting_node(oct_node node, vec3 * io_position, vec3 * io_size){
  vec3 size = *io_size;
  vec3 position = *io_position;
  while(size.x > 1 || size.y > 1 || size.z > 1){
    position = to_super_coords(oct_index(node), position);
    size = to_super_size(size);
    node = oct_get_super(node);
  }
  while(size.x < 0.5 && size.y < 0.5 && size.z < 0.5){
    position = to_sub_coords(0, position);
    size = to_sub_size(size);
    node = oct_get_sub(node, 0);
  }
  vec3i offset = vec3i_from_vec3(position);
  oct_node newnode = oct_get_relative(node, offset);
  *io_position = vec3_sub(position, vec3i_to_vec3(offset));
  *io_size = size;
  return newnode;
}

oct_node oct_get_nth_super(oct_node node, int n){
  for(;n > 0; n -= 1)
    node = oct_get_super(node);
  return node;
}

static bool rec_clean(oct_node node){
  
  int doclean[8] = {0};
  for(int i = 0; i < 8; i++){
    oct_node n = oct_peek_sub(node, i);
    if(!oct_node_null(n)){
      bool cancleansub = rec_clean(n);
      doclean[i] = cancleansub ? 1 : 2;
    };
  }
  bool allcleanable = oct_get_payload(node) == NULL;
  for(int i = 0; i < 8; i++)
    allcleanable &= doclean[i] != 2;
  if(allcleanable){

    return true;
  }
  for(int i = 0; i < 8; i++){
    if(doclean[i] == 1){
      oct_delete_sub(node, i);
    }
  }
  return false;
}

void oct_clean_tree(oct_node node){
  rec_clean(node);
}

oct_node find_common_super(oct_node a, oct_node b){
  if(oct_node_compare(a,b))
    return oct_peek_super(a);

  oct_node asupers[64]; // Max size: ~2^64, enough? too much to push 2x64 pointers on the stack?
  oct_node bsupers[64];
  size_t acnt = 0, bcnt = 0;
  
  for(; acnt < array_count(asupers); acnt++)
    if(oct_node_null(a)) break;
    else{
      asupers[acnt] = a;
      a = oct_peek_super(a);
    }
  for(; bcnt < array_count(bsupers); bcnt++)
    if(oct_node_null(b)) break;
    else{
      bsupers[bcnt] = b;
      b = oct_peek_super(b);
    }

  ASSERT(acnt < 64 && bcnt < 64);
  do{
    bcnt = bcnt - 1;
    acnt = acnt - 1;
    if(!oct_node_compare(asupers[acnt], bsupers[bcnt]))
      return asupers[acnt + 1];

  }while(acnt > 0 && bcnt > 0);
  UNREACHABLE();
  // this can happen if a and b are not part of the same octree.
  // or if height of tree is > 64.
  return oct_node_empty;
}

vec3 oct_node_offset(oct_node a, oct_node b){
  ASSERT(!oct_node_null(a));ASSERT(!oct_node_null(b));
  oct_node super = find_common_super(a, b);
  vec3 offset_a = oct_get_super_offset(a, super);
  vec3 offset_b = oct_get_super_offset(b, super);
  return vec3_sub(offset_b, offset_a);
}
