#include <iron/full.h>
#include "oct_node.h"
#include "vec3i.h"

int oct_index(oct_node * oc){
  if(!oct_has_super(oc))
    return 0;
  oct_node * super = oct_get_super(oc);
  for(int i = 0; i < 8; i++)
    if(oct_has_sub(super, i) && oct_get_sub(super, i) == oc)
      return i;
  ERROR("Not a child node of that element\n");
  return -1;
}

int vec3i_to_index(vec3i v){
  return v.x + v.y * 2 + v.z * 4;
}

vec3i vec3i_from_index(int idx){
  ASSERT(idx >= 0 && idx < 8);
  return vec3i_make(idx & 1, (idx / 2) & 1, (idx / 4) & 1);
}

vec3i vec3i_half(vec3i v){
  return vec3i_make(v.x >> 1, v.y >> 1, v.z >> 1);
}


oct_node * oct_get_relative(oct_node * oc, vec3i v){
  if(v.x == 0 && v.y == 0 && v.z == 0)
    return oc;
  oct_node * super = oct_get_super(oc);
  vec3i idx = vec3i_from_index(oct_index(oc));
  vec3i super_offset = vec3i_half(vec3i_add(idx, v));
  if(super_offset.x != 0 || super_offset.y != 0 || super_offset.z != 0)
    super = oct_get_relative(super, super_offset);
  return oct_get_sub(super, vec3i_to_index(vec3i_bitand(vec3i_add(v, idx), vec3i_make(1,1,1))));
}

void oct_render_node(oct_node * oc, float size, vec3 offset, 
		     void (* f)(oct_node * oc, float size, vec3 offset)){
  f(oc, size, offset);
  int idxes[] = {1, 0, 5, 4, 3, 2, 7, 6};
  ASSERT(array_count(idxes) == 8);
  for(size_t i = 0; i < array_count(idxes); i++){
    int idx = idxes[i];
    if(oct_has_sub(oc, idx)){
      vec3 off = vec3i_to_vec3(vec3i_from_index(idx));
      oct_render_node(oct_get_sub(oc, idx), size * 0.5, vec3_add(offset, vec3_scale(off, size * 0.5)), f);
    }
  }
}

vec3 to_super_coords (int idx, vec3 coords){
  vec3 offset = vec3i_to_vec3(vec3i_from_index(idx));
  return vec3_scale(vec3_add(coords, offset), 0.5);
}

vec3 to_super_size(vec3 size){
  return vec3_scale(size, 0.5);
}

vec3 to_sub_coords(int idx, vec3 coords){
  vec3 offset = vec3i_to_vec3(vec3i_from_index(idx));
  return vec3_sub(vec3_scale(coords, 2), offset);
}

vec3 to_sub_size(vec3 size){
  return vec3_scale(size, 2.0);
}

static vec3 _oct_get_super_offset(oct_node * node, oct_node * super){
  if(super == node)
    return vec3mk(0,0,0);
  int idx = oct_index(node);
  vec3 par = _oct_get_super_offset(oct_get_super(node), super);
  return to_sub_coords(idx, par);
}

vec3 oct_get_super_offset(oct_node * node, oct_node * super){
  { // Sanity check
    oct_node * n = node;
    for(; n != super && oct_has_super(n); n = oct_get_super(n));
    ASSERT(n == super);
  }
  return _oct_get_super_offset(node, super);
}

float oct_get_super_size(oct_node * node, oct_node * super){
  { // Sanity check
    oct_node * n = node;
    for(; n != super && oct_has_super(n); n = oct_get_super(n));
    ASSERT(n == super);
  }
  float f = 1;
  while(node != super){
    f = f * 2;
    node = oct_get_super(node);
  }
  return f;
}
