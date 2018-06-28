#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL_image.h>
#endif

#include <sys/time.h>
#include <stdbool.h>

#ifndef SPRITES_SIZE
#define SPRITES_SIZE (32)
#endif

struct Context {
  int w;
  int h;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *textures[3];

  int frames;
  int fps;
  double now;
};


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
    // draw
    //

    SDL_RenderClear(context->renderer);

    for(int i=0; i<SPRITES_SIZE; i++)
    {
      // int x = i % (context->w/32) * 32;
      // int y = i / (context->w/32) * 32;
      int x = rand() % context->w;
      int y = rand() % context->w;
      double angle = 0;

      SDL_Point zero = {0,0};
      SDL_Rect src = {0,0,32,32};
      SDL_Rect dst = {x,y,32,32};
      SDL_RenderCopyEx(context->renderer,context->textures[i%3],&src,&dst,angle,&zero,false);
    }

    SDL_RenderPresent(context->renderer);
}

void setup(struct Context *context)
{
  if( SDL_Init(SDL_INIT_VIDEO) != 0 ){
      exit(-1);
  }

  char title[1024];
  snprintf(title,1024,"%d sprites", SPRITES_SIZE);

  context->w = 1024;
  context->h = 640;
  context->window = SDL_CreateWindow(title, 0, 0, context->w, context->h, SDL_WINDOW_OPENGL);
  context->renderer = SDL_CreateRenderer(context->window,-1,SDL_RENDERER_ACCELERATED);


  SDL_Surface *surfaces[3];
  surfaces[0] = IMG_Load("asset_dir/invader.png");
  surfaces[1] = IMG_Load("asset_dir/vehicle.png");
  surfaces[2] = IMG_Load("asset_dir/face01.png");

  context->textures[0] = SDL_CreateTextureFromSurface(context->renderer,surfaces[0]);
  context->textures[1] = SDL_CreateTextureFromSurface(context->renderer,surfaces[1]);
  context->textures[2] = SDL_CreateTextureFromSurface(context->renderer,surfaces[2]);

  context->fps = 0;
  context->frames = 0;
  context->now = get_now();
}

int main()
{
    struct Context _context;
    struct Context* context = &_context;
    setup(context);

    const int FPS = 0;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(ems_loop, context, FPS, true);
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
        ems_loop(context);
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
}
