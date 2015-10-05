typedef struct{
  int x, y, z;
}vec3i;

typedef struct{
  int x, y;
}vec2i;

vec3i vec3i_make(int x, int y, int z);
vec3i vec3i_add(vec3i a, vec3i b);
vec3i vec3i_bitand(vec3i a, vec3i b);
void vec3i_print(vec3i v);
vec3 vec3i_to_vec3(vec3i v);
vec3i vec3i_from_vec3(vec3 v);
