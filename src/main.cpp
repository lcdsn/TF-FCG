#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <algorithm>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

// Headers das bibliotecas OpenGL
#include "glad/glad.h" // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h> // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "laboratorios.h"
#include "matrices.h"
#include "utils.h"

GLuint CreateGpuProgram(GLuint vertex_shader_id,
						GLuint fragment_shader_id); // Cria um programa de GPU

int main()
{
	// Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
	// sistema operacional, onde poderemos renderizar com OpenGL.
	int success = glfwInit();
	if(!success)
	{
		fprintf(stderr, "ERROR: glfwInit() failed.\n");
		std::exit(EXIT_FAILURE);
	}

	// Definimos o callback para impressão de erros da GLFW no terminal
	glfwSetErrorCallback(ErrorCallback);

	// Pedimos para utilizar OpenGL versão 3.3 (ou superior)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
	// funções modernas de OpenGL.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
	// de pixels, e com título "INF01047 ...".
	GLFWwindow* window;
	window = glfwCreateWindow(800, 600, "TF-FCG", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
		fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
		std::exit(EXIT_FAILURE);
	}

	// Definimos a função de callback que será chamada sempre que o usuário
	// pressionar alguma tecla do teclado ...
	glfwSetKeyCallback(window, KeyCallback);
	// ... ou clicar os botões do mouse ...
	// glfwSetMouseButtonCallback(window, MouseButtonCallback);
	// ... ou movimentar o cursor do mouse em cima da janela ...
	// glfwSetCursorPosCallback(window, CursorPosCallback);
	// ... ou rolar a "rodinha" do mouse.
	// glfwSetScrollCallback(window, ScrollCallback);

	// Indicamos que as chamadas OpenGL deverão renderizar nesta janela
	glfwMakeContextCurrent(window);

	// Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
	// biblioteca GLAD.
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	// Definimos a função de callback que será chamada sempre que a janela for
	// redimensionada, por consequência alterando o tamanho do "framebuffer"
	// (região de memória onde são armazenados os pixels da imagem).
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// Forçamos a chamada do callback acima, para definir g_ScreenRatio.
	FramebufferSizeCallback(window, 800, 600);

	// Imprimimos no terminal informações sobre a GPU do sistema
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

	// Inicializamos o código para renderização de texto.

	// Habilitamos o Z-buffer. Veja slides 104-116 do documento
	// Aula_09_Projecoes.pdf.
	glEnable(GL_DEPTH_TEST);

	// Habilitamos o Backface Culling. Veja slides 8-13 do documento
	// Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento
	// Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento
	// Aula_14_Laboratorio_3_Revisao.pdf.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
	while(!glfwWindowShouldClose(window))
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
