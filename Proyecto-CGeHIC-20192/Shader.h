#pragma once
#include<stdio.h>
#include<string>
#include<iostream>
#include<fstream>

#include<glew.h>

class Shader1
{
public:
	Shader1();
	void CreatefromString1(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles1(const char* vertexLocation, const char* fragmentLocation);
	std::string ReadFile1(const char*  fileLocation);
	GLuint getProjectLocation1();
	GLuint getModelLoaction1();
	GLuint getViewLocation1();
	GLuint getColorLocation1();

	void useShader1();
	void ClearShader1();
	~Shader1();

private: 
	GLuint shaderID, uniformProjection, uniformModel,uniformView, uniformColor;
	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* ShaderCode, GLenum shaderType);
};

