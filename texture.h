
GLuint createTextureFromSurface(SDL_Surface *src)
{
    SDL_Surface* img = src;

    // force change Format to SDL_PIXELFORMAT_ABGR8888
    if(src->format->format != SDL_PIXELFORMAT_ABGR8888)
    {
        img = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, 32, SDL_PIXELFORMAT_ABGR8888);
        if (img == NULL) {
            SDL_Log("SDL_CreateRGBSurfaceWithFormat() failed: %s", SDL_GetError());
            exit(1);
        }
        SDL_BlitSurface(src,NULL,img,NULL);
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);

    printf("%d,%d format: %s\n",img->w,img->h, SDL_GetPixelFormatName(img->format->format));

    // テクスチャをGPUに転送
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // XXX: RGBA is correct.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);

    // テクスチャの設定
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // XXX: Non-power-of-two textures must have a wrap mode of CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // incorrect wrap settings
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // テクスチャのアンバインド
    glBindTexture(GL_TEXTURE_2D, 0);

    if(src->format->format != SDL_PIXELFORMAT_ABGR8888)
      SDL_FreeSurface(img);

    return texture_id;
}

GLuint loadTexture(const char* filename, int *w, int *h)
{
    GLuint texture_id;

    // XXX: ARGB? ABGR?
    // Desktop OpenGL:
    //   SDL_PIXELFORMAT_ARGB8888 としてロードされる。表示上、RとBが逆になってるぽい。
    // WebGL:
    //   SDL_PIXELFORMAT_ABGR8888 としてロードされる。ただしく表示される。
    //
    SDL_Surface *image = IMG_Load(filename);
    if(image) {
      printf("%s: %d,%d format: %s\n",filename, image->w,image->h, SDL_GetPixelFormatName(image->format->format));
    }else{
      printf("Error IMG_Load %s\n",filename);
    }

    texture_id = createTextureFromSurface(image);
    *w = image->w;
    *h = image->h;

    SDL_FreeSurface(image);

    return texture_id;
}

