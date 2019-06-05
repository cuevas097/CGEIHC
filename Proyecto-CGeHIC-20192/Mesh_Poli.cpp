#include "Mesh_Poli.h"

Mesh_Poli::Mesh_Poli()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	Inicio = 0;
	Cantidad = 0;

}

void Mesh_Poli::CreateMesh_Poli(GLfloat *vertices, GLfloat *indices, unsigned int numOfVertices, unsigned int numberOfIndices)
{

	glGenVertexArrays(1, &VAO);//Genera 1 VAO
	glBindVertexArray(VAO);//asigna VAO
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])* numOfVertices, vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*numberOfIndices, indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*3, 0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*3, (void*)(sizeof(vertices[0])*3));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*2, (void*)(sizeof(vertices[0])*6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void Mesh_Poli::RenderMesh_Poli(GLint inicio, GLint fin)
{
	////////////Para dibujar desde los índices
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawArrays(GL_QUADS, inicio, fin);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh_Poli::ClearMesh_Poli()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO); ///LIMPIAR BUFFER PARAE EVITAR OCUPAR LA MEMORIA
		IBO = 0;
	}
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO); ///LIMPIAR BUFFER PARAE EVITAR OCUPAR LA MEMORIA
		VBO = 0;
	}
	if(VAO!=0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	
}
Mesh_Poli::~Mesh_Poli()
{
	ClearMesh_Poli();
}
