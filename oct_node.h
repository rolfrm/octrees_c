
// Opaque octree node type.
typedef struct{
  void * ptr1;
  void * ptr2;
}oct_node;

// Gets or creates the idx'th sub node of the octree node.
oct_node oct_get_sub(oct_node node, int idx);

// Gets the idx'th sub node of the octree node, might be NULL if not set.
oct_node oct_peek_sub(oct_node node, int idx);

// Gets or creates a super node of the node.
oct_node oct_get_super(oct_node node);

oct_node oct_peek_super(oct_node node);

bool oct_has_sub(oct_node node);
bool oct_has_super(oct_node node);

// creates a new oct_node.
oct_node oct_create();

// Destructs an oct_node and all child nodes.
void oct_delete(oct_node node);

// Deletes a sub of a node.
void oct_delete_sub(oct_node node, int idx);

// Gets the payload pointer of a node
void * oct_get_payload(oct_node node);

// Sets the oct node payload
void oct_set_payload(oct_node node, void * payload);

int oct_node_compare(oct_node a, oct_node b);
extern oct_node oct_node_empty;

bool oct_node_null(oct_node oc);
