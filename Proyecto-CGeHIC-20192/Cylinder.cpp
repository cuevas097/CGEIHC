#include "Cylinder.h"

Cylinder::Cylinder(int slices, int stacks, float topRadius, float bottomRadius, float height) :
	slices(slices), stacks(stacks), topRadius(topRadius), bottomRadius(bottomRadius), height(height), VAO(0), VBO(0), EBO(0) {

}

Cylinder::~Cylinder() {

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

}

void Cylinder::init() 
{
	float stackHeight = height / stacks;
	float radiusStep = (topRadius - bottomRadius) / stacks;
	int count = 0;

	vertexArray.resize((slices + 1) * (stacks + 1) + 2 * (slices + 1) + 2);
	index.resize(slices * stacks * 2 * 3 + 2 * slices * 3);

	for (int i = 0; i <= stacks; i++) {
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;
		float dTheta = float(2.0f * M_PI) / slices;
		float V = i / (float)stacks;
		for (int j = 0; j <= slices; j++) {
			float c = cos(j * dTheta);
			float s = sin(j * dTheta);
			float U = j / (float)slices;
			vertexArray[count++] = Vertex(glm::vec3(r * c, y, r * s), glm::vec3(), glm::vec2(U, V), glm::vec3(-r * c, -y, -r * s));
		}
	}

	//top cap
	float y = 0.5f * height;
	float dTheta = float(2.0f * M_PI) / slices;

	for (int i = slices; i >= 0; i--) {
		float x = cos(i * dTheta);
		float z = sin(i * dTheta);
		float U = x / 2.0f + 0.5f;
		float V = z / 2.0f + 0.5f;
		vertexArray[count++] = Vertex(glm::vec3(topRadius * x, y, topRadius * z), glm::vec3(), glm::vec2(U, V), glm::vec3(0, z, 0));
	}
	vertexArray[count++] = Vertex(glm::vec3(0, y, 0), glm::vec3(), glm::vec2(0.5f, 0.5f), glm::vec3(0, y, 0));
	//bottom cap
	y = -y;

	for (int i = 0; i <= slices; i++) {
		float x = cos(i * dTheta);
		float z = sin(i * dTheta);
		float U = x / 2.0f + 0.5f;
		float V = z / 2.0f + 0.5f;
		vertexArray[count++] = Vertex(glm::vec3(bottomRadius * x, y, bottomRadius * z), glm::vec3(), glm::vec2(U, V), glm::vec3(0, z, 0));
	}
	vertexArray[count++] = Vertex(glm::vec3(0, y, 0), glm::vec3(), glm::vec2(0.5, 0.5), glm::vec3(0, y, 0));

	//fill indices array
	int ringVertexCount = slices + 1;
	int id = 0;
	for (int i = 0; i < stacks; i++) {
		for (int j = 0; j < slices; j++) {
			index[id++] = (i * ringVertexCount + j);
			index[id++] = ((i + 1) * ringVertexCount + j);
			index[id++] = ((i + 1) * ringVertexCount + j + 1);

			index[id++] = (i * ringVertexCount + j);
			index[id++] = ((i + 1) * ringVertexCount + j + 1);
			index[id++] = (i * ringVertexCount + j + 1);
		}
	}

	//top cap
	int baseIndex = (slices + 1) * (stacks + 1);
	int centerIndex = baseIndex + (slices + 1);

	for (int i = 0; i < slices; i++) {
		index[id++] = centerIndex;
		index[id++] = baseIndex + i;
		index[id++] = baseIndex + i + 1;
	}

	//bottom cap
	baseIndex = centerIndex + 1;
	centerIndex = baseIndex + (slices + 1);

	for (int i = 0; i < slices; i++) {
		index[id++] = centerIndex;
		index[id++] = baseIndex + i;
		index[id++] = baseIndex + i + 1;
	}
	//typeModel = TypeModel::CYLINDER;
}

void Cylinder::load() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(vertexArray[0]), vertexArray.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(index[0]), index.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid*) sizeof(vertexArray[0].m_pos));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid*)(sizeof(vertexArray[0].m_pos) + sizeof(vertexArray[0].m_color)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid*)(sizeof(vertexArray[0].m_pos) + sizeof(vertexArray[0].m_color) + sizeof(vertexArray[0].m_tex)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Cylinder::render() {

	glBindVertexArray(VAO);
	glDrawElements( GL_TRIANGLES, index.size(), GL_UNSIGNED_INT,
			(GLvoid*) (sizeof(GLuint) * 0));

	glBindVertexArray(0);

}
