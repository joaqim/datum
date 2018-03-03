
#include "Application.h"

float tri[] = { -0.8,  0.6,    1, 0, 0,
		 0.7,  0.0,    1, 1, 1,
		-0.8, -0.6,    1, 1, 0, };

const char *vertex_shader_src = ""
"#version 330 core\n"
"layout (location = 0) in vec2 pos;"
"layout (location = 1) in vec3 col;"
"out vec3 passed_color;"
"void main() {"
"  passed_color = col;"
"  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);"
"}";

const char *fragment_shader_src = ""
"#version 330 core\n"
"in vec3 passed_color;"
"out vec4 color;"
"void main() {"
"  color = vec4(passed_color.rgb, 1.0);"
"}";

void Application::check_shader(GLuint shader, const char *shader_name) {
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    GLchar info_log[512];
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    printf("Error in %s: %s\n", shader_name, info_log);
  }
}

int Application::initialize() {

glfwInit();

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
  m_window = glfwCreateWindow(400, 300, "Mini", NULL, NULL);
  glfwMakeContextCurrent(m_window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW\n");
    return -1;
  }
  
  glClearColor(0.95, 0.9, 0.95, 1.0);
  
  GLuint m_vertexShader;
  m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vertexShader, 1, &vertex_shader_src, NULL);
  glCompileShader(m_vertexShader);
  check_shader(m_vertexShader, "m_vertexShader");
  
  GLuint m_fragmentShader;
  m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_fragmentShader, 1, &fragment_shader_src, NULL);
  glCompileShader(m_fragmentShader);
  check_shader(m_fragmentShader, "m_fragmentShader");

  GLuint m_program;
  m_program = glCreateProgram();
  glAttachShader(m_program, m_vertexShader);
  glAttachShader(m_program, m_fragmentShader);
  glLinkProgram(m_program);

  GLint success;
  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  if(!success) {
    GLchar info_log[512];
    glGetProgramInfoLog(m_program, 512, NULL, info_log);
    printf("Linker error: %s\n", info_log);
  }

  glUseProgram(m_program);
  
  GLuint m_vertexArrayBuffer;
  glGenVertexArrays(1, &m_vertexArrayBuffer);
  glBindVertexArray(m_vertexArrayBuffer);
  
  GLuint m_vertexBufferObject;
  glGenBuffers(1, &m_vertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
}
void Application::run() {
  while(!glfwWindowShouldClose(m_window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
}

void Application::exit() {
  glDeleteShader(m_vertexShader);
  glDeleteShader(m_fragmentShader);
	glfwTerminate();
}
