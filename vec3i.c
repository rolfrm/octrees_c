#include <stdbool.h>
#include <math.h>
#include "iron/linmath.h"
#include "iron/log.h"
#include "vec3i.h"

vec3i vec3i_make(int x, int y, int z){
  return (vec3i){x, y, z};
}

vec3i vec3i_add(vec3i a, vec3i b){
  return vec3i_make(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3i vec3i_bitand(vec3i a, vec3i b){
  return vec3i_make(a.x & b.x, a.y & b.y, a.z & b.z);
}

void vec3i_print(vec3i v){
  logd("(%i %i %i)", v.x, v.y, v.z);
}

vec3 vec3i_to_vec3(vec3i v){
  return vec3mk(v.x, v.y, v.z);
}

vec3i vec3i_from_vec3(vec3 v){
  return (vec3i){floor(v.x), floor(v.y), floor(v.z)};
}
