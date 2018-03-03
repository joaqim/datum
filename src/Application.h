#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>



class Application {

private:
void check_shader(GLuint shader, const char *shader_name);
public:
int initialize();
void run();
void exit();

private:
	GLFWwindow *m_window;

	GLuint m_program;
	GLuint m_fragmentShader;
	GLuint m_vertexShader;
	GLuint m_vertexArrayBuffer;
	GLuint m_vertexBufferObject; //TODO: Probably
};
