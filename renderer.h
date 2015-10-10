// Requires: event, game_state.
typedef struct _game_renderer game_renderer;
typedef struct _texture_asset texture_asset;

game_renderer * renderer_load(int width, int height, int unit_size);
void renderer_render(game_renderer * renderer, world_state * gamestate);
u32 renderer_read_events(event * buffer, u32 buffer_size);
texture_asset * renderer_load_texture(game_renderer * renderer, const char * path);
texture_asset * renderer_clone_texture(texture_asset * asset);
void texture_asset_set_offset(texture_asset * asset, vec2 offset);
void texture_asset_set_size(texture_asset * asset, vec2i size);

typedef struct _game_controller_state{
  // gamepad: 2 Left Joystick, 2 right joystick, 1 LT, 1 RT.
  // keyboard: wasd -> 2, up/down/left/right -> 2, CTRL, Space
  float axes[6];
  // gamepad: X. Y, ?? -> 4, Start, select
  // keyboard: e, f, shift, ?, enter, esc
  button_state buttons[6];
}game_controller_state;

game_controller_state renderer_game_controller();
void game_controller_state_print(game_controller_state s);

vec2i renderer_get_mouse_position(game_renderer * rnd);
