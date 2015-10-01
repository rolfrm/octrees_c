// requires oct_node.

/*typedef enum _entity_type{
  TILE,
  OBJECT
  
}entity_type;

typedef struct {
  entity_type type;
  texture_asset * asset;
}tile;

typedef struct{
  entity_type type;
  texture_asset * texture;
  vec3 offset;
  vec3 size;
}entity;
*/
typedef struct _world_state{
  oct_node * center_node;
}world_state;
