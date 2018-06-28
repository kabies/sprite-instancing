
#ifdef __EMSCRIPTEN__
#define VERTEX_SHADER_HEADER "#version 100\nprecision mediump float;"
#define FRAGMENT_SHADER_HEADER "#version 100\nprecision mediump float;"
#else
#define VERTEX_SHADER_HEADER "#version 120\n"
#define FRAGMENT_SHADER_HEADER "#version 120\n"
#endif

// 一行にまとめられる
// コメントはCのコメントとしてマクロの処理前に消える
#define D(...) #__VA_ARGS__ "\n";

const char *VERTEX_SHADER = VERTEX_SHADER_HEADER D(
uniform mat4 u_mvpMatrix;
attribute vec2 vertex;
attribute vec4 texture_uv;
attribute vec3 position;
attribute vec2 sprite_size;
attribute float vertex_index;
attribute float texture_z;
varying vec3 uv;
void main()
{
  mat4 scale_transform = mat4(
    sprite_size.x,           0.0, 0.0, 0.0,
              0.0, sprite_size.y, 0.0, 0.0,
              0.0,           0.0, 1.0, 0.0,
              0.0,           0.0, 0.0, 1.0
  );

  mat4 translation_transform = mat4(
           1.0,        0.0, 0.0, 0.0,
           0.0,        1.0, 0.0, 0.0,
           0.0,        0.0, 1.0, 0.0,
    position.x, position.y, 0.0, 1.0
  );

  // cos -sin  0  0      cos  sin  0  0
  // sin  cos  0  0     -sin  cos  0  0
  //   0    0  1  0  =>    0    0  1  0
  //   0    0  0  1        0    0  0  1
  mat4 rotation_transform = mat4(
     cos(position.z), sin(position.z), 0.0, 0.0,
    -sin(position.z), cos(position.z), 0.0, 0.0,
                 0.0,             0.0, 1.0, 0.0,
                 0.0,             0.0, 0.0, 1.0
  );

  gl_Position = u_mvpMatrix * translation_transform * rotation_transform * scale_transform * vec4(vertex,0.0,1.0);

  // vertex = [ left-top, left-bottom, right-top, right-bottom ]
  // texture_uv = [ x:left, y:top, z:right, w:bottom ]
  int vertexid = int(vertex_index);
  if( vertexid == 0 ){
    uv = vec3(texture_uv.x,texture_uv.y,texture_z); // left-top
  }else if( vertexid == 1 ){
    uv = vec3(texture_uv.x,texture_uv.w,texture_z); // left-bottom
  }else if( vertexid == 2 ){
    uv = vec3(texture_uv.z,texture_uv.y,texture_z); // right-top
  }else if( vertexid == 3 ){
    uv = vec3(texture_uv.z,texture_uv.w,texture_z); // right-bottom
  }
}
);

const char *FRAGMENT_SHADER = FRAGMENT_SHADER_HEADER D(
varying vec3 uv;
uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2D sampler3;
uniform sampler2D sampler4;
uniform sampler2D sampler5;
uniform sampler2D sampler6;
uniform sampler2D sampler7;
void main()
{
  int samplerID = int(uv.z);
  if(samplerID==0){
    gl_FragColor = texture2D( sampler0, uv.xy );
  }else if(samplerID==1){
    gl_FragColor = texture2D( sampler1, uv.xy );
  }else if(samplerID==2){
    gl_FragColor = texture2D( sampler2, uv.xy );
  }else if(samplerID==3){
    gl_FragColor = texture2D( sampler3, uv.xy );
  }else if(samplerID==4){
    gl_FragColor = texture2D( sampler4, uv.xy );
  }else if(samplerID==5){
    gl_FragColor = texture2D( sampler5, uv.xy );
  }else if(samplerID==6){
    gl_FragColor = texture2D( sampler6, uv.xy );
  }else if(samplerID==7){
    gl_FragColor = texture2D( sampler7, uv.xy );
  }
}
);

#undef D
// あぶねえのでマクロ消す

void errorCheck(const char* name, GLuint shader)
{
  GLint result;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    GLsizei length;
    GLchar log[1024];
    glGetShaderInfoLog(shader, 1024, &length, log);
    printf("ERR: %s @ %s\n",name,log);
  }
}

GLuint crateShader()
{
    printf("----\n%s\n----\n", VERTEX_SHADER);
    printf("----\n%s\n----\n", FRAGMENT_SHADER);

    // バーテックスシェーダのコンパイル
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &VERTEX_SHADER, NULL);
    glCompileShader(vertex_shader);
    errorCheck("vertexShader",vertex_shader);

    // フラグメントシェーダのコンパイル
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER, NULL);
    glCompileShader(fragment_shader);
    errorCheck("fragmentShader",fragment_shader);

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id,vertex_shader);
    glAttachShader(program_id,fragment_shader);

    glLinkProgram(program_id);

    return program_id;
}
