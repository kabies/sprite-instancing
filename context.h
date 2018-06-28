
struct Context {
    int w;
    int h;
    SDL_Window *window;

    float fps;
    int frames;
    double now;

    int node_size;

    GLuint texture_ids[3];

    GLuint program_id;
    GLuint vao;
    GLuint vertex_buffer;
    GLuint xy_position_buffer;
    GLuint uv_buffer;
    GLuint sprite_size_buffer;
    GLuint vertex_index_buffer;
    GLuint texture_z_buffer;

    // uniform location
    GLuint texture_locations[8];
    GLuint projection_location;

    // attribute location
    GLuint vertex_location;
    GLuint texture_uv_location;
    GLuint position_location;
    GLuint sprite_size_location;
    GLuint vertex_index_location;
    GLuint texture_z_location;
};
