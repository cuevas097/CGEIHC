#include "Shader.h"



Shader1::Shader1()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView=0;
	uniformColor = 0;
}

void Shader1::CreatefromString1(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}
void Shader1::CreateFromFiles1(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile1(vertexLocation);
	std::string fragmentString = ReadFile1(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);

}

std::string Shader1::ReadFile1(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);
	if (!fileStream.is_open())
	{
		printf("Falló en leer el archivo: %s", fileLocation);
		return "";
	}
	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}



void Shader1::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderID = glCreateProgram(); //se crea un programa
	if (!shaderID)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);//Agregar fragment shader
												   //Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shaderID);//se linkean los shaders a la tarjeta gráfica
						  //verificaciones y prevención de errores
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformView= glGetUniformLocation(shaderID, "view");
	uniformColor = glGetUniformLocation(shaderID, "color");

}

GLuint Shader1::getColorLocation1()
{
	return uniformColor;
}
GLuint Shader1::getViewLocation1()
{
	return uniformView;
}
GLuint Shader1::getProjectLocation1()
{
	return uniformProjection;
}
GLuint Shader1::getModelLoaction1()
{	
	return uniformModel;
}

void Shader1::useShader1()
{
	glUseProgram(shaderID);

}

void Shader1::ClearShader1()
{
	if (!shaderID)
	{ 
		glDeleteProgram(shaderID);//borra programa de la tarjeta gráfica
		shaderID = 0;
	}
	uniformModel = 0;
	uniformProjection = 0;
	uniformView=0;
	uniformColor = 0;
}
void Shader1:: AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica
																			 //the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

Shader1::~Shader1()
{
	ClearShader1();
}
