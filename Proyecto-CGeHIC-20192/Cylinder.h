#ifndef CYLINDER_H
#define CYLINDER_H

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include "definition.h"

class Cylinder {
public:
	Cylinder(int slices, int stacks, float topRadius = 1.0, float bottomRadius = 1.0, float height = 1.0);
	void init();
	void load();
	void render();
	virtual ~Cylinder();
	
	int getSlices() {
		return this->slices;
	}

	int getStacks() {
		return this->stacks;
	}

	
private:
	//std::vector<_VertexColor> vertexC;
	std::vector<_Vertex> vertexArray;
	std::vector<VertexLightColor> vertexLC;
	/*std::vector<VertexLightTexture> vertexLT;*/
	std::vector<GLuint> index;
	int slices;
	int stacks;
	float topRadius;
	float bottomRadius;
	float height;
	//MODEL_MODE mode;

	GLuint VAO, VBO, EBO;
};

#endif // CYLINDER_H