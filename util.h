
double get_now()
{
#ifdef __EMSCRIPTEN__
  return emscripten_get_now();
#else
  struct timeval now;
  gettimeofday(&now, NULL);
  return (double)now.tv_sec*1000 + (double)now.tv_usec/1000;
#endif
}

// XXX: wrapper
void glGenVertexArrays_APPLE_wrapper(GLsizei s, GLuint *p) { glGenVertexArraysAPPLE(s,p); }
void glBindVertexArray_APPLE_wrapper(GLuint vao) {glBindVertexArrayAPPLE(vao); }

void checkSupports()
{
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    printf("GL_MAX_TEXTURE_SIZE: %d\n",max_texture_size);

    GLint max_texture_image_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_image_units);
    printf("GL_MAX_TEXTURE_IMAGE_UNITS: %d\n",max_texture_image_units);

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_instanced_arrays.txt
    if(GLEW_ARB_instanced_arrays) printf("ARB_instanced_arrays ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_draw_instanced.txt
    if(GLEW_ARB_draw_instanced) printf("ARB_draw_instanced ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_vertex_array_object.txt
    if(GLEW_ARB_vertex_array_object) printf("ARB_vertex_array_object ok\n");

#ifdef __EMSCRIPTEN__
    // https://www.khronos.org/registry/webgl/extensions/ANGLE_instanced_arrays/
    if(GLEW_ANGLE_instanced_arrays) printf("ANGLE_instanced_arrays ok\n");

    // https://www.khronos.org/registry/OpenGL/extensions/OES/OES_vertex_array_object.txt
    if(GLEW_OES_vertex_array_object) printf("OES_vertex_array_object ok\n");
#else
    // https://www.khronos.org/registry/OpenGL/extensions/APPLE/APPLE_vertex_array_object.txt
    if(GLEW_APPLE_vertex_array_object){
        printf("APPLE_vertex_array_object ok\n");
        glGenVertexArrays = glGenVertexArrays_APPLE_wrapper;
        glBindVertexArray = glBindVertexArray_APPLE_wrapper;
    }
#endif

    printf("checkSupports done.\n");
}
