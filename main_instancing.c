#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#else
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#endif

#include <sys/time.h>
#include <stdbool.h>

#include "shader.h"
#include "projection.h"
#include "texture.h"
#include "context.h"
#include "util.h"

#ifndef SPRITES_SIZE
#define SPRITES_SIZE (32)
#endif

static void ems_loop( void* arg )
{
    struct Context *context = (struct Context*)arg;

    double now = get_now();
    double delta = now - context->now;
    if(delta > 1000) {
      printf("FPS: %d\n",context->frames);
      context->fps = context->frames;
      context->frames = 0;
      context->now = now;
    }
    context->frames++;

    //
    // calc
    //
    GLfloat xy_position[3*context->node_size];
    GLfloat sprite_size[2*context->node_size];
    GLfloat texture_z[context->node_size];
    for(int i=0; i<context->node_size; i++){
      int position_index = i*3;
      int sprite_index = i*2;

#if 1
      // no-rotation
      GLfloat angle = 0;
#else
      // rotation
      GLfloat angle = i*0.1 / 360.0 * 2 * 3.1415;
#endif

#if 1
      // random position
      xy_position[position_index+0] = rand() % context->w;
      xy_position[position_index+1] = rand() % context->h;
      xy_position[position_index+2] = angle;
#else
      // lined up
      xy_position[position_index+0] = i % (context->w/32) * 32;
      xy_position[position_index+1] = i / (context->w/32) * 32;
      xy_position[position_index+2] = angle;
#endif

      // sprite_size
      if(i%2==0){
        sprite_size[sprite_index+0] = 32;
        sprite_size[sprite_index+1] = 32;
      }else{
        sprite_size[sprite_index+0] = 16;
        sprite_size[sprite_index+1] = 16;
      }

      // texture_z
      texture_z[i] = i%3;
    }


    //
    //
    //
    glClearColor(0.1, 0.1, 0.2, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // vbo update
    glBindBuffer(GL_ARRAY_BUFFER, context->xy_position_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * context->node_size, xy_position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, context->sprite_size_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 2 * context->node_size, sprite_size);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, context->texture_z_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 1 * context->node_size, texture_z);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // uniform update
    for(int i=0;i<8;i++)
      glUniform1i(context->texture_locations[i], i);

    // texture bind
    for(int i=0;i<3;i++){
      glActiveTexture(GL_TEXTURE0+i);
      glBindTexture(GL_TEXTURE_2D, context->texture_ids[i]);
    }

    // enable vao
    glBindVertexArray(context->vao);
      // instance draw
      // XXX: not glDrawArraysInstancedANGLE
      glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, context->node_size);
    glBindVertexArray(0);

    //
    SDL_GL_SwapWindow(context->window);
}

void setup_shader(struct Context *context)
{
    printf("setup_shader\n");

    context->program_id = crateShader();
    glUseProgram ( context->program_id );

    // uniform locations
    context->projection_location = glGetUniformLocation(context->program_id, "u_mvpMatrix");
    context->texture_locations[0]  = glGetUniformLocation(context->program_id, "sampler0");
    context->texture_locations[1]  = glGetUniformLocation(context->program_id, "sampler1");
    context->texture_locations[2]  = glGetUniformLocation(context->program_id, "sampler2");
    context->texture_locations[3]  = glGetUniformLocation(context->program_id, "sampler3");
    context->texture_locations[4]  = glGetUniformLocation(context->program_id, "sampler4");
    context->texture_locations[5]  = glGetUniformLocation(context->program_id, "sampler5");
    context->texture_locations[6]  = glGetUniformLocation(context->program_id, "sampler6");
    context->texture_locations[7]  = glGetUniformLocation(context->program_id, "sampler7");
    // attributes locations
    context->vertex_location = glGetAttribLocation(context->program_id, "vertex");
    context->texture_uv_location = glGetAttribLocation(context->program_id, "texture_uv");
    context->position_location = glGetAttribLocation(context->program_id, "position");
    context->sprite_size_location = glGetAttribLocation(context->program_id, "sprite_size");
    context->vertex_index_location = glGetAttribLocation(context->program_id, "vertex_index");
    context->texture_z_location = glGetAttribLocation(context->program_id, "texture_z");

    for(int i=0;i<8;i++)
      printf("texture sampler location %d: %d\n",i, context->texture_locations[i]);

    printf(
      "vertex: %d\n"
      "texture_uv: %d\n"
      "position: %d\n"
      "sprite_size: %d\n"
      "vertex_index: %d\n"
      "texture_z: %d\n",
      context->vertex_location,
      context->texture_uv_location,
      context->position_location,
      context->sprite_size_location,
      context->vertex_index_location,
      context->texture_z_location
    );

    // texture
    int w,h;
    context->texture_ids[0] = loadTexture("asset_dir/invader.png", &w,&h);
    context->texture_ids[1] = loadTexture("asset_dir/vehicle.png", &w,&h);
    context->texture_ids[2] = loadTexture("asset_dir/face01.png", &w,&h);

    // projection
    setOrthographicProjection(context->w,context->h,context->projection_location);

    context->node_size = SPRITES_SIZE;

    // vertex
    GLfloat l=0, t=1, r=1, b=0;
    GLfloat vertex[8] = {
      l, t,
      l, b,
      r, t,
      r, b
    };

    // XXX: prior OpenGL3 and ES3, can not send int...
    GLfloat vertex_index[4] = {0,1,2,3};

    // texture_uv = [ x:left, y:top, z:right, w:bottom ]
    GLfloat uv[4*context->node_size];
    for(int i=0; i<context->node_size;i++) {
      if(i%2==0) {
        uv[i*4+0] = 0; uv[i*4+1] = 0; // Left Top
        uv[i*4+2] = 1; uv[i*4+3] = 1; // Right Bottom
      }else{
        uv[i*4+0] = 0.25; uv[i*4+1] = 0.25; // Left Top
        uv[i*4+2] = 0.75; uv[i*4+3] = 0.75; // Right Bottom
      }
    };

    // create vbo : vertex
    glGenBuffers(1, &context->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, context->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertex, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vbo : uv
    glGenBuffers(1, &context->uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, context->uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * context->node_size, uv, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vbo : xy_position
    glGenBuffers(1, &context->xy_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, context->xy_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * context->node_size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vbo : sprite_size
    glGenBuffers(1, &context->sprite_size_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, context->sprite_size_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * context->node_size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vbo : vertex index
    glGenBuffers(1, &context->vertex_index_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, context->vertex_index_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, vertex_index, GL_DYNAMIC_DRAW); // XXX: float. not int.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vbo : texture_z
    glGenBuffers(1, &context->texture_z_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, context->texture_z_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 1 * context->node_size, NULL, GL_DYNAMIC_DRAW); // XXX: float. not int.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vao
    // XXX: in macOS, glGenVertexArraysAPPLE and glBindVertexArrayAPPLE are wrapped. see "util.h".
    glGenVertexArrays(1, &context->vao);
    glBindVertexArray(context->vao);

      // vertex
      glBindBuffer(GL_ARRAY_BUFFER, context->vertex_buffer);
      glEnableVertexAttribArray ( context->vertex_location );
      glVertexAttribPointer ( context->vertex_location, 2, GL_FLOAT, GL_FALSE, 0, NULL );

      // texture_uv
      glBindBuffer(GL_ARRAY_BUFFER, context->uv_buffer);
      glEnableVertexAttribArray ( context->texture_uv_location );
      glVertexAttribPointer ( context->texture_uv_location, 4, GL_FLOAT, GL_FALSE, 0, NULL );

      // xy-position
      glBindBuffer(GL_ARRAY_BUFFER, context->xy_position_buffer);
      glEnableVertexAttribArray ( context->position_location );
      glVertexAttribPointer ( context->position_location, 3, GL_FLOAT, GL_FALSE, 0, NULL );

      // sprite_size
      glBindBuffer(GL_ARRAY_BUFFER, context->sprite_size_buffer);
      glEnableVertexAttribArray ( context->sprite_size_location );
      glVertexAttribPointer ( context->sprite_size_location, 2, GL_FLOAT, GL_FALSE, 0, NULL );

      // vertex_index
      glBindBuffer(GL_ARRAY_BUFFER, context->vertex_index_buffer);
      glEnableVertexAttribArray ( context->vertex_index_location );
      // XXX: no glVertexAttribIPointer : since OpenGL3.0 and ES3.0
      glVertexAttribPointer ( context->vertex_index_location, 1, GL_FLOAT, GL_FALSE, 0, NULL );

      // texture_z
      glBindBuffer(GL_ARRAY_BUFFER, context->texture_z_buffer);
      glEnableVertexAttribArray ( context->texture_z_location );
      // XXX: no glVertexAttribIPointer : since OpenGL3.0 and ES3.0
      glVertexAttribPointer ( context->texture_z_location, 1, GL_FLOAT, GL_FALSE, 0, NULL );

      // for instancing
      // XXX: not glVertexAttribDivisorANGLE in emscripten
      glVertexAttribDivisorARB(context->vertex_location, 0); // vertex
      glVertexAttribDivisorARB(context->texture_uv_location, 1); // uv
      glVertexAttribDivisorARB(context->position_location, 1); // xy_position
      glVertexAttribDivisorARB(context->sprite_size_location, 1); // sprite_size
      glVertexAttribDivisorARB(context->vertex_index_location, 0); // vertex_index
      glVertexAttribDivisorARB(context->texture_z_location, 1); // texture_z

    glBindVertexArray(0);
}


int main()
{
    struct Context context;

    if( SDL_Init(SDL_INIT_VIDEO) != 0 ){
        return -1;
    }

#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

    context.w = 1024;
    context.h = 640;
    context.window = SDL_CreateWindow("SDL2/OpenGL Demo", 0, 0, context.w, context.h, SDL_WINDOW_OPENGL);

    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(context.window);

    if( glewInit() == GLEW_OK ){
      printf("glewInit ok\n");
    }else{
      printf("glewInit faild\n");
    }

    checkSupports();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setup_shader(&context);

    context.fps = 0;
    context.frames = 0;
    context.now = get_now();

    const int FPS = 0;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(ems_loop, &context, FPS, true);
#else
    SDL_GL_SetSwapInterval(0); // set FPS
    bool running = true;
    while (running) {
        double start_at, end_at;
        SDL_Event e;
        start_at = get_now();
        while (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) {
              SDL_Log("Program quit after %i ticks", e.quit.timestamp);
              running = false;
              break;
          }
        }
        ems_loop(&context);
        end_at = get_now();

        int sleep = 1;
        if(FPS>0) {
          sleep = (1.0 / FPS) * 1000 - (end_at - start_at);
        }
        if(sleep<=0) {
          sleep = 1;
        }

        SDL_Delay(sleep);
    }
#endif

  SDL_GL_DeleteContext(sdl_gl_context);
}
