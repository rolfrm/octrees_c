// requires oct_node.

typedef struct _texture_asset texture_asset;
typedef enum _entity_type{
  TILE,
  OBJECT,
  SATELITE,
  HASH64
}entity_type;

typedef struct {
  entity_type type;
  oct_node node;
}entity_header;

typedef struct _entity entity;

typedef struct {
  entity_type type;
  oct_node node;
  vec3 offset;
  entity * origin;
}satelite;

typedef struct {
  entity_type type;
  oct_node node;
  i32 entity_id;
}tile;

typedef struct {
  entity_header ** entity;
  size_t cnt;
}entity_list;

void add_entity(oct_node oc, entity_header * eh);
void remove_entity(entity_header * eh);
struct _entity{
  entity_type type;
  oct_node node;
  i32 entity_id;
  vec3 offset;
  vec3 size;
};

typedef struct _world_state{
  oct_node center_node;
}world_state;


