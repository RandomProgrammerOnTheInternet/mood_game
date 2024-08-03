#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <cglm/cglm.h>

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

f32 square_vertices[] = {
	// first triangle
	0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // top right
	-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // top left
	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
	// second triangle
	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
	-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // top left
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f // bottom left
};

u32 vertex_shader;
u32 fragment_shader;
u32 shader_program;
u32 height = 480;
u32 width = 640;
i32 tex_width;
i32 tex_height;
i32 nr_channels;
u8 *data;
u32 texture;
f32 previous_mouse_xcor = 320;
f32 previous_mouse_ycor = 240;
f32 mouse_xcor = 320;
f32 mouse_ycor = 240;
f32 mouse_x_offset;
f32 mouse_y_offset;

/*typedef struct {
    vec3 bottom_left;
    vec3 top_right;
    const u8 vertices = 6;
} wall;*/

SDL_Window *window = NULL;
SDL_Event event;
bool keep_window_open = 1;
bool stop_focus = 0;
u32 VAO;
u32 VBO;

vec3 camera_position = {0.0f, 0.0f, 3.0f};
vec3 camera_front = {0.0f, 0.0f, -1.0f};
vec3 camera_up = {0.0f, -1.0f, 0.0f};

f32 pitch = 0.0f;
f32 yaw = 90.0f;
f32 roll = 0.0f;

mat4 rotation_matrix;

f32 mouse_sensitivity = 0.1f;

f32 delta_time = 0.0f;
f32 last_frame = 0.0f;

int tick();
float mouse_callback(f32 x_pos, f32 y_pos);
char *read_whole_file(FILE *);

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

	char *vsrc = read_whole_file(v);
	char *fsrc = read_whole_file(f);
	const char *glvsrc = vsrc;
	const char *glfsrc = fsrc;
	fclose(v);
	fclose(f);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("textures/texture.jpg", &tex_width, &tex_height,
					 &nr_channels, 0);
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB,
				 GL_UNSIGNED_BYTE, data);
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
	// Then we *bind* the VAO to the uninited VBO.
	glBindVertexArray(VAO);
	// Now, we switch focus to the VBO and upload it's data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices,
				 GL_STATIC_DRAW);

	// Continue as normal.
	// I can't belive you can't even follow a tutorial.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
						  (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
						  (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	/* wHYYYYYYYYY CANT Isduasjdnakdhsa duasdh sa dhisdyYO Yshaudhsadsaodhaosuhd*/

	// Also, you only really need to call this once. You are not doing hot reloading
	// for shaders.
	glUseProgram(shader_program);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	if(SDL_GetWindowFlags(window) == SDL_WINDOW_INPUT_FOCUS |
		   SDL_WINDOW_MOUSE_FOCUS &&
	   stop_focus == 0) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
    else if(SDL_GetWindowFlags(window) == SDL_WINDOW_MOUSE_FOCUS &&
			  stop_focus == 1) {
		stop_focus = 0;
	}
	f32 current_frame = SDL_GetTicks64();
	delta_time = current_frame - last_frame;
	last_frame = current_frame;
	f32 camera_speed = 0.05f * delta_time;
	vec3 camera_front_up;
	vec3 direction;
	bool first_mouse = 1;
	vec3 axis = {1.0f, 0.0f, 0.0f};
	vec3 camera_normalize = { camera_front[0], 0.0f, camera_front[2] };
	glm_vec3_normalize(camera_normalize);
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
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
			break;
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym) {
			case SDLK_w:
				glm_vec3_muladds(camera_normalize, camera_speed,
								 camera_position);
				camera_position[1] = 0.0f;
				break;
			case SDLK_s:
				glm_vec3_mulsubs(camera_normalize, camera_speed,
								 camera_position);
				camera_position[1] = 0.0f;
				break;
			case SDLK_a:
				glm_vec3_cross(camera_normalize, camera_up, camera_front_up);
				glm_vec3_normalize(camera_front_up);
				glm_vec3_muladds(camera_front_up, camera_speed,
								 camera_position);
				camera_position[1] = 0.0f;
				break;
			case SDLK_d:
				camera_up[0] = 1.0f / 3.0f;
				glm_vec3_cross(camera_normalize, camera_up, camera_front_up);
				glm_vec3_normalize(camera_front_up);
				glm_vec3_mulsubs(camera_front_up, camera_speed,
								 camera_position);
				camera_position[1] = 0.0f;
				break;
			case SDLK_q:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case SDLK_e:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case SDLK_ESCAPE:
				SDL_SetRelativeMouseMode(SDL_FALSE);
				stop_focus = 1;
				break;
			default:
				camera_up[0] = 0.0f;
				break;
			}

			break;
		case SDL_MOUSEMOTION:
			yaw += event.motion.xrel * 0.00005;
			pitch += -event.motion.yrel * 0.00005;

			if(pitch > (glm_deg(89.0f))) {
				pitch = glm_deg(89.0f);
			}
			if(pitch < (glm_deg(-89.0f))) {
				pitch = glm_deg(-89.0f);
			}

			direction[0] = cos(glm_deg(yaw)) * cos(glm_deg(pitch));
			direction[1] = sin(glm_deg(pitch));
			direction[2] = sin(glm_deg(yaw)) * cos(glm_deg(pitch));
			glm_vec3_normalize_to(direction, camera_front);
			break;
		default:
			camera_up[0] = 0.0f;
			break;
		}
	}
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	axis[0] = 1.0f;
	axis[1] = 1.0f;
	axis[2] = 1.0f;
	vec3 translation_factor;
	translation_factor[0] = 0.0f;
	translation_factor[1] = 0.0f;
	translation_factor[2] = -3.0f;

	mat4 view;
	glm_mat4_identity(view);
	mat4 projection;
	glm_mat4_identity(projection);
	glm_perspective(glm_deg(114), 640.0f / 480.0f, 0.0001f, 5.0f, projection);
	u32 projection_location =
		glGetUniformLocation(shader_program, "projection");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection[0]);

	vec3 position = { 0.0f, 0.0f, 0.0f };

	f32 radius = 10.0f;
	f32 cam_x = sin(SDL_GetTicks64() / 1000.0f) * radius;
	f32 cam_z = cos(SDL_GetTicks64() / 1000.0f) * radius;
	vec3 camera_pos_front;
	glm_vec3_add(camera_position, camera_front, camera_pos_front);
	glm_lookat(camera_position, camera_pos_front, camera_up, view);
	u32 view_location = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(view_location, 1, GL_FALSE, view[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	/* Please learn Vulkan at this point. At least it *tells* you
	 * where you screwed up via validation layers, and also the
	 * boilerplate makes sure that stuff like this doesn't happen.
	 * Seriously? Forgetting to set the uniform of the texture?
	 */
	u32 tex0_location = glGetUniformLocation(shader_program, "ourTexture");
	glUniform1i(tex0_location, 0);

	glBindVertexArray(VAO);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, position);
	u32 model_location = glGetUniformLocation(shader_program, "model");
	glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(window);

	// No.
	/*printf("%f %f %f\n", camera_position[0], camera_position[1],
		   camera_position[2]);*/

	return 0;
}

char *read_whole_file(FILE *f) {
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *str = malloc(fsize + 1);
	fread(str, fsize, 1, f);
	str[fsize] = 0;

	return str;
}

/*level load_level(FILE* f) {
    char level_data[];
    // probably unsafe code
    while(fgets(level_data, sizeof(level_data), f) != NULL) {
        if(level_data[])
    }
}*/
