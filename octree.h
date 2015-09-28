oct_node * oct_get_relative(oct_node * oc, vec3i v);
void oct_render_node(oct_node * oc, float size, vec3 offset, 
		     void (* f)(oct_node * oc, float size, vec3 offset));
int oct_index(oct_node * oc);
vec3 oct_get_super_offset(oct_node * node, oct_node * super);
float oct_get_super_size(oct_node * node, oct_node * super);
