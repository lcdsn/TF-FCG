#ifndef LABORATORIOS_H
#define LABORATORIOS_H

float g_ScreenRatio = 1.0f;

// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	// Este construtor lê o modelo de um arquivo utilizando a biblioteca
	// tinyobjloader. Veja: https://github.com/syoyo/tinyobjloader
	ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
	{
		printf("Carregando objetos do arquivo \"%s\"...\n", filename);

		// Se basepath == NULL, então setamos basepath como o dirname do
		// filename, para que os arquivos MTL sejam corretamente carregados caso
		// estejam no mesmo diretório dos arquivos OBJ.
		std::string fullpath(filename);
		std::string dirname;
		if(basepath == NULL)
		{
			auto i = fullpath.find_last_of("/");
			if(i != std::string::npos)
			{
				dirname = fullpath.substr(0, i + 1);
				basepath = dirname.c_str();
			}
		}

		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(
			&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

		if(!err.empty())
			fprintf(stderr, "\n%s\n", err.c_str());

		if(!ret)
			throw std::runtime_error("Erro ao carregar modelo.");

		for(size_t shape = 0; shape < shapes.size(); ++shape)
		{
			if(shapes[shape].name.empty())
			{
				fprintf(stderr,
						"*********************************************\n"
						"Erro: Objeto sem nome dentro do arquivo '%s'.\n"
						"Veja "
						"https://www.inf.ufrgs.br/~eslgastal/"
						"fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
						"*********************************************\n",
						filename);
				throw std::runtime_error("Objeto sem nome.");
			}
			printf("- Objeto '%s'\n", shapes[shape].name.c_str());
		}

		printf("OK.\n");
	}
};

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
	// Criamos um identificador (ID) para este programa de GPU
	GLuint program_id = glCreateProgram();

	// Definição dos dois shaders GLSL que devem ser executados pelo programa
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);

	// Linkagem dos shaders acima ao programa
	glLinkProgram(program_id);

	// Verificamos se ocorreu algum erro durante a linkagem
	GLint linked_ok = GL_FALSE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

	// Imprime no terminal qualquer erro de linkagem
	if(linked_ok == GL_FALSE)
	{
		GLint log_length = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

		// Alocamos memória para guardar o log de compilação.
		// A chamada "new" em C++ é equivalente ao "malloc()" do C.
		GLchar* log = new GLchar[log_length];

		glGetProgramInfoLog(program_id, log_length, &log_length, log);

		std::string output;

		output += "ERROR: OpenGL linking of program failed.\n";
		output += "== Start of link log\n";
		output += log;
		output += "\n== End of link log\n";

		// A chamada "delete" em C++ é equivalente ao "free()" do C
		delete[] log;

		fprintf(stderr, "%s", output.c_str());
	}

	// Os "Shader Objects" podem ser marcados para deleção após serem linkados
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	// Retornamos o ID gerado acima
	return program_id;
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
	fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja
// http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	// Se o usuário pressionar a tecla ESC, fechamos a janela.
	if(key == GLFW_KEY_Q && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Se o usuário apertar a tecla R, recarregamos os shaders dos arquivos
	// "shader_fragment.glsl" e "shader_vertex.glsl".
	// if(key == GLFW_KEY_R && action == GLFW_PRESS)
	// {
	// 	LoadShadersFromFiles();
	// 	fprintf(stdout, "Shaders recarregados!\n");
	// 	fflush(stdout);
	// }
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// Indicamos que queremos renderizar em toda região do framebuffer. A
	// função "glViewport" define o mapeamento das "normalized device
	// coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
	// "Screen Mapping" ou "Viewport Mapping" vista em aula
	// ({+ViewportMapping2+}).
	glViewport(0, 0, width, height);

	// Atualizamos também a razão que define a proporção da janela (largura /
	// altura), a qual será utilizada na definição das matrizes de projeção,
	// tal que não ocorra distorções durante o processo de "Screen Mapping"
	// acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215
	// do documento Aula_09_Projecoes.pdf.
	//
	// O cast para float é necessário pois números inteiros são arredondados ao
	// serem divididos!
	g_ScreenRatio = (float)width / height;
}
#endif // LABORATORIOS_H
