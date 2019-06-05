#pragma once
#include<glew.h>
class Mesh_Poli
{
public:
	Mesh_Poli();
	void CreateMesh_Poli(GLfloat *vertices, GLfloat *indices,unsigned int numOfVertices, unsigned int numberOfIndices);
	void RenderMesh_Poli(GLint inicio, GLint fin);
	void ClearMesh_Poli();
	~Mesh_Poli();

private:
	GLuint VAO, VBO, IBO;
	GLuint Inicio, Cantidad;


};
