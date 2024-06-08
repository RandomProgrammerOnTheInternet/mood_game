#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

SDL_Window* window = NULL;
SDL_Event event;
bool keep_window_open = 1; 
u32 VAO;
u32 VBO;
u32 EBO;

f32 vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   4.0f, 4.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 4.0f    // top left 
};

u32 indices[] = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

u32 vertex_shader;
u32 fragment_shader;
u32 shader_program;
u32 height = 480;
u32 width = 640;
u32 tex_width;
u32 tex_height;
u32 nr_channels;
u8* data;
u32 texture;

int tick();
void framebuffer_size_callback(SDL_Window*, u32, u32);
char* read_whole_file(FILE*);

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    window = SDL_CreateWindow(
        "m        o        o        d", SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 640, 480,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GLContext glctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glctx);
    SDL_GL_SetSwapInterval(1);
    // Your viewport should be the same as your window dimensions.
    glViewport(0, 0, width, height);
    FILE *v = fopen("vert.glsl", "r");
	  FILE *f = fopen("frag.glsl", "r");
	  
    char* vsrc = read_whole_file(v);
	  char* fsrc = read_whole_file(f);
	  const char* glvsrc = vsrc;
	  const char* glfsrc = fsrc;
    fclose(v);
	  fclose(f);
    
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("texture.jpg", &tex_width, &tex_height, &nr_channels, 0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &glvsrc, NULL);
    glCompileShader(vertex_shader);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &glfsrc, NULL);
    glCompileShader(fragment_shader);
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    free(vsrc);
	  free(fsrc);


    // HELLO ABDULMOHSIN!
    // README!!!
    // So, We generate both VAO and VBO, with VAO first and VBO second.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Then we *bind* the VAO to the uninited VBO.
    glBindVertexArray(VAO);
    // Now, we switch focus to the VBO and upload it's data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Continue as normal.
    // I can't belive you can't even follow a tutorial.

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Also, you only really need to call this once. You are not doing hot reloading
    // for shaders.
    glUseProgram(shader_program);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Your optimzation is terrible. keep_window_open is a boolean,
    // you don't need to check if it's 1 or not, just insert the statement.
    // This is because for while, if, and for statements
    // 0 = false
    // >=1 = true
    // And also, if you are using the argument for "it's more readable",
    // go switch to a higher level language. This is C. The 0, >=1 rule is something
    // you should know.
    while(keep_window_open) {
        tick();
    }

    SDL_GL_DeleteContext(glctx);

    return 0;
}

int tick() {
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            keep_window_open = 0;
            break;
        case SDL_WINDOWEVENT:
				    if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
				        width = event.window.data1;
				        height = event.window.data2;
				        glViewport(0, 0, width, height);
				    }

        }
    }
    glClearColor(0.5f, 0.2f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);

    return 0;
}

void framebuffer_size_callback(SDL_Window* window, u32 window_width, u32 window_height) {
    glViewport(0, 0, window_width, window_height);
}

char* read_whole_file(FILE *f) {
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* str = malloc(fsize + 1);
	  fread(str, fsize, 1, f);
	  str[fsize] = 0;

	  return str;
}
