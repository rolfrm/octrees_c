// requires game_state.h
// invokes function in GUI thread.
void game_editor_invoke(void (*f)(void * userdata), void * userdata);
// invokes function in GUI thread and blocks the thread untill the edit is complete.
void game_editor_invoke_blocking(void (*f)(void * userdata), void * userdata);

// ext api:
typedef void * game_editor;
game_editor start_game_editor(world_state * world);
void stop_game_editor(game_editor ed);
