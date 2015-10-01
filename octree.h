// Gets a node relative to the start node.
oct_node * oct_get_relative(oct_node * node, vec3i v);

// As oct_get_relative but returns NULL if the relative node does not exists.
oct_node * oct_try_get_relative(oct_node * node, vec3i v);

// Iterates down the node tree and calls f on each node.
void oct_render_node(oct_node * node, float size, vec3 offset, 
		     void (* f)(oct_node * oc, float size, vec3 offset));

// Returns the index of node relative to its parent.
int oct_index(oct_node * node);

// Returns the relative offset between the node 'super' and the sub node 'node'.
vec3 oct_get_super_offset(oct_node * node, oct_node * super);

// Returns the relative size of 'super' compared to 'node'.
float oct_get_super_size(oct_node * node, oct_node * super);

// Returns all nodes that collides with the box defined by pos and size.
void oct_lookup_blocks(oct_node * node, vec3 position, vec3 size, 
		       void (* cb)(oct_node * node, vec3 pos, vec3 size));

// Returns the nth parent of 'node'.
oct_node * oct_get_nth_super(oct_node * node, int n);

// Finds the node that fulfills the following criteria. all size < 1.0, any size > 0.5, 0 < position < 1
oct_node * oct_find_fitting_node(oct_node * node, vec3 * io_position, vec3 * io_size);

// Deletes all nodes with no payload.
void oct_clean_tree(oct_node * node);
