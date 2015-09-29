// Requires: event, game_state.
typedef struct _game_renderer game_renderer;

game_renderer * renderer_load(int width, int height);
void renderer_render(game_renderer * renderer, game_state * gamestate);
u32 renderer_read_events(event * buffer, u32 buffer_size);
