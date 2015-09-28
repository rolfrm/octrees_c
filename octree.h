// Gets a node relative to the start node.
oct_node * oct_get_relative(oct_node * node, vec3i v);
// Iterates down the node tree and calls f on each node.
void oct_render_node(oct_node * node, float size, vec3 offset, 
		     void (* f)(oct_node * oc, float size, vec3 offset));
// Returns the index of node relative to its parent.
int oct_index(oct_node * node);
vec3 oct_get_super_offset(oct_node * node, oct_node * super);
float oct_get_super_size(oct_node * node, oct_node * super);
void oct_lookup_blocks(oct_node * node, vec3 position, vec3 size, 
		       void (* cb)(oct_node * node, vec3 pos, vec3 size));
