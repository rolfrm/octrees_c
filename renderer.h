// Requires: event, game_state.
typedef struct _game_renderer game_renderer;
typedef struct _texture_asset texture_asset;

game_renderer * renderer_load(int width, int height);
void renderer_render(game_renderer * renderer, game_state * gamestate);
u32 renderer_read_events(event * buffer, u32 buffer_size);
texture_asset * renderer_load_texture(game_renderer * renderer, const char * path);
