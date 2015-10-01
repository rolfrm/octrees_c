// requires oct_node.

typedef struct _texture_asset texture_asset;
typedef enum _entity_type{
  TILE,
  OBJECT
}entity_type;

typedef struct {
  entity_type type;
  texture_asset * asset;
  oct_node * node;
}entity_header;

typedef struct {
  entity_type type;
  texture_asset * asset;
  oct_node * node;
}tile;

typedef struct {
  entity_header ** entity;
  size_t cnt;
}entity_list;

void add_entity(oct_node * oc, entity_header * eh);
void remove_entity(oct_node * oc, entity_header * eh);
typedef struct{
  entity_type type;
  texture_asset * texture;
  oct_node * node;
  vec3 offset;
  vec3 size;
  
}entity;

typedef struct _world_state{
  oct_node * center_node;
}world_state;
