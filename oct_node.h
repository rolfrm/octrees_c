
// Opaque octree node type.
typedef struct _oct_node oct_node;

// Gets or creates the idx'th sub node of the octree node.
oct_node * oct_get_sub(oct_node * node, int idx);

// Returns true if the node has a idx'th item.
bool oct_has_sub(oct_node * node, int idx);

// Gets or creates a super node of the node.
oct_node * oct_get_super(oct_node * node);

// Returns true of node has a super node.
bool oct_has_super(oct_node * node);

// creates a new oct_node.
oct_node * oct_create();

// Destructs an oct_node and all child nodes.
void oct_delete(oct_node * node);

// Gets the payload pointer of a node
void * oct_get_payload(oct_node * node);

// Sets the oct node payload
void oct_set_payload(oct_node * node, void * payload);
