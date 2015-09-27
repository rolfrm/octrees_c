#include <iron/full.h>

typedef struct _oct_node oct_node;

struct _oct_node{
  oct_node * super;
  oct_node * sub[8];
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

typedef struct{
  int x, y, z;
}vec3i;

vec3i vec3i_make(int x, int y, int z){
  return (vec3i){x, y, z};
}

int vec3i_to_index(vec3i v){
  return v.x + v.y * 2 + v.z * 4;
}

vec3i vec3i_from_index(int idx){
  ASSERT(idx >= 0 && idx < 8);
  return vec3i_make(idx & 1, (idx / 2) & 1, (idx / 4) & 1);
}

int divd(int a, int b){
  if(a < 0)
    return (a - 1) / b;
  return a / b;
}

vec3i vec3i_divi(vec3i v, int d){
  return vec3i_make(divd(v.x,  d), divd(v.y, d), divd(v.z, d));
}

vec3i vec3i_half(vec3i v){
  return vec3i_make(v.x >> 1, v.y >> 1, v.z >> 1);
}

vec3i vec3i_add(vec3i a, vec3i b){
  return vec3i_make(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3i vec3i_bitand(vec3i a, vec3i b){
  return vec3i_make(a.x & b.x, a.y & b.y, a.z & b.z);
}

void test_div(){
  for(int i = -10; i < 10; i++)
    logd("i: %i /2 : %i %i %i\n", i, i / 2, divd(i, 2), i >> 1);
}

void vec3i_print(vec3i v){
  logd("(%i %i %i)", v.x, v.y, v.z);
}

vec3 vec3i_to_vec3(vec3i v){
  return vec3mk(v.x, v.y, v.z);
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

vec3 to_parent_coords (int idx, vec3 coords){
  vec3 offset = vec3i_to_vec3(vec3i_from_index(idx));
  return vec3_scale(vec3_add(coords, offset), 0.5);
}

vec3 to_parent_size(vec3 size){
  return vec3_scale(size, 0.5);
}

vec3 to_child_coords(int idx, vec3 coords){
  vec3 offset = vec3i_to_vec3(vec3i_from_index(idx));
  return vec3_sub(vec3_scale(coords, 2), offset);
}

#include <signal.h>

void _error(const char * file, int line, const char * str, ...){
  loge("** ERROR at %s:%i **\n",file,line);
  loge("%s", str);
  logd("\n");
  logd("** **\n");
  raise(SIGINT);
}

int main(){
  test_div();
  oct_node * n1 = alloc0(sizeof(oct_node));
  oct_node * n2 = oct_get_super(n1);
  oct_node * n3 = oct_get_super(n2);
  oct_node * n4 = oct_get_super(n3);
  oct_node * n5 = oct_get_super(n4);
  logd("N4 index: %i %i %i %i %i\n", oct_index(n1), oct_index(n2), oct_index(n3), oct_index(n4), oct_index(n5));
  ASSERT(n1 == oct_get_sub(n2, oct_index(n1)));
  oct_node * n6 = oct_get_relative(n1, vec3i_make(10,0,0));
  ASSERT(n1 != n6);
  oct_node * n7 = oct_get_relative(n6, vec3i_make(-10,0,0));
  oct_node * n8 = oct_get_relative(n7, vec3i_make(10,0,0));
  ASSERT(n7 == n1);
  ASSERT(n8 == n6);

  hash_table * ht = ht_create(1024, 8, 8);
  size_t x = 1;
  ht_insert(ht, &n1, &x);
  for(int i = -5; i < 5; i++){
    for(int j = -5; j < 5; j++){
      oct_node * n = oct_get_relative(n1, vec3i_make(i,j,0));
      size_t x2 = i * i * j * j;
      ht_insert(ht, &n, &x2);
    }
  }
  void rnd(oct_node * oc, float s, vec3 o){
    
    size_t * x1 = ht_lookup(ht, &oc);
    if(x1 != NULL){
      logd("%p %f %4i ", oc, s, (x1 != NULL ? *x1 : 0)); 
      vec3_print(o); 
      logd("\n");
    }
  }
  
  oct_render_node(n5, 1.0, vec3mk(0.0, 0.0, 0.0), rnd);
  
  
  return 0;
}
