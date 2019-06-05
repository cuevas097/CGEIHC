/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*---------------- Alumnos:                              ------------------*/
/*----------------          Cuevas Salgado Carlos        ------------------*/
/*----------------          Galindo Chavez Lisset America------------------*/
/*----------------------  Version: Visual Studio 2017 ---------------------*/
/* -------------------------- Primer Avance -------------------------------*/
/*----------------------  Instrucciones de uso ----------------------------*/
/*-------------     Tecla     ----    Movimiento   ------------------------*/
/*-------------       D       ----     Derecha     ------------------------*/
/*-------------       A       ----    Izquierda    ------------------------*/
/*-------------       W       ----     Aleja       ------------------------*/
/*-------------       S       ----     Acerca      ------------------------*/
/*------------- Mouse + botDer---- Rota la camara  ------------------------*/
/*------------------- Fecha de entrega: 03/05/2019 ------------------------*/
/*----------------------       2019-2      --------------------------------*/
/*------------- Computación gráfica e interacción humano computadora ---------------*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <string>
#include <iostream>
#include <fstream>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
//#include "Mesh_texturizado.h"
#include "Mesh_Poli.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

#include "Cylinder.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
std::vector<Mesh_Poli*> meshPoliList;
Camera camera;

Texture plainTexture;//Textura del suelo


//Texturas Juego Giratorio
Texture MetalPisoTexture;//Suelo del juego
Texture rojoMetalTexture;//Estructura del guego giratorio
Texture laminaCirculoTexture;
Texture amarillo2Texture;//Estructura del juego giratorio
Texture circuitoAzulTexture; //Carrito del juego
Texture amarilloTexture; //Asientos del carrusel
Texture FloresTexture; //Carrito del juego
Texture azulTexture; //Carrito del juego
Texture CajaMetalTexture;
Texture coloresTexture;
Texture RayasTexture;
Texture JuegoTexture;

//Para los keyframes
float reproduciranimacion,habilitaranimacion, guardoFrame,reinicioFrame,ciclo,ciclo2,contador=0;

//Para cambio de hora
bool dia = true, noche = false, horario = false;
float nocheCont = 0.0f, diaCont = 0.0f;
std::string rtSkybox = "Textures/Skybox/sgod_ft.tga";
std::string lfSkybox = "Textures/Skybox/sgod_bk.tga";
std::string dnSkybox = "Textures/Skybox/sgod_dn.tga";
std::string upSkybox = "Textures/Skybox/sgod_up.tga";
std::string bkSkybox = "Textures/Skybox/sgod_rt.tga";
std::string ftSkybox = "Textures/Skybox/sgod_lf.tga";

float movCoche;
float movOffset;
bool avanza;
////Variables para movimiento del juego giratorio
float GiratorioMov;
float MovimientoGira;
float MovY;
int estado;
glm::vec3 PosJuegoGira = glm::vec3(-30, -2, 70);//Vector para mover el juego giratorio


//Juego de las sillas
Model sillas;

//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional

DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Squartle
Model sq1;
Model sq2;
Model sq3;
Model sq4;

Skybox skybox;

//Varibales para mover el agua
Model Mar;
Model Alberca;
float MarZ1 = 0.0f;
float MarZ2 = 10.724f;
float MarZ3 = 10.724f*2.0f;
bool MarOn = false;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//void my_input(GLFWwindow *window);
void inputKeyframes(bool* keys);

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
	  2, 1, 0,
	  0, 3, 2,
	  // right
	  8, 9, 11,
	  8, 11, 10,
	  // back
	  5, 6, 7,
	  7, 4, 5,
	  // left
	  12, 13, 14,
	  12, 14, 15,
	  // bottom
	  22, 21, 20,
	  21, 22, 23,
	  // top
	  16, 17, 18,
	  19, 18, 17,
	};

	GLfloat cubo_vertices[] = {

		//x     y       z       S			T		NX		NY		NZ
	   // front (6)
	   -0.5f, -0.5f,  0.5f,    0.66f,  0.0f,		0.0f,	0.0f,	-1.0f,
	   0.5f, -0.5f,  0.5f,     1.0f,   0.0f,		0.0f,	0.0f,	-1.0f,
	   0.5f,  0.5f,  0.5f,     1.0f,   0.5f,		0.0f,	0.0f,	-1.0f,
	   -0.5f,  0.5f,  0.5f,    0.66f,  0.5f,		0.0f,	0.0f,	-1.0f,
	   // back (1)
	   -0.5f, -0.5f, -0.5f,    0.0f,   0.5f,		0.0f,	0.0f,	1.0f,
	   0.5f, -0.5f, -0.5f,     0.33f,  0.5f,		0.0f,	0.0f,	1.0f,
	   0.5f,  0.5f, -0.5f,     0.33f,  1.0f,		0.0f,	0.0f,	1.0f,
	   -0.5f,  0.5f, -0.5f,    0.0f,   1.0f,		0.0f,	0.0f,	1.0f,
	   // right (3)
	   0.5f, -0.5f,  0.5f,     0.66f,  0.5f,		-1.0f,	0.0f,	0.0f,
	   0.5f,  0.5f, 0.5f,      0.66,   1.0f,		-1.0f,	0.0f,	0.0f,
	   0.5f,  -0.5f,  -0.5f,   1.0f,   0.5f,		-1.0f,	0.0f,	0.0f,
	   0.5f, 0.5f, -0.5f,      1.0f,   1.0f,		-1.0f,	0.0f,	0.0f,
	   // left (4)
	   -0.5f, 0.5f, 0.5f,      0.33f,  0.5f,		1.0f,	0.0f,	0.0f,
	   -0.5f, -0.5f,  0.5f,    0.33f,  0.0f,		1.0f,	0.0f,	0.0f,
	   -0.5f, -0.5f,  -0.5f,   0.0f,   0.0f,		1.0f,	0.0f,	0.0f,
	   -0.5f,  0.5f, -0.5f,    0.0f,   0.5f,		1.0f,	0.0f,	0.0f,
	   // top (5)
	   0.5f,  0.5f,  0.5f,     0.66f,  0.5f,		0.0f,	-1.0f,	0.0f,
	   -0.5f,  0.5f,  0.5f,    0.33f,  0.5f,		0.0f,	-1.0f,	0.0f,
	   0.5f,  0.5f, -0.5f,     0.66f,  0.0f,		0.0f,	-1.0f,	0.0f,
	   -0.5f,  0.5f, -0.5f,    0.33,   0.0f,		0.0f,	-1.0f,	0.0f,
	   // bottom (2)
	   0.5f, -0.5f,  0.5f,     0.66f,  1.0f,		0.0f,	1.0f,	0.0f,
	   -0.5f, -0.5f,  0.5f,    0.33f,  1.0f,		0.0f,	1.0f,	0.0f,
	   0.5f, -0.5f, -0.5f,     0.66f,  0.5f,		0.0f,	1.0f,	0.0f,
	   -0.5f, -0.5f, -0.5f,    0.33f,  0.5f,		0.0f,	1.0f,	0.0f,
	};
	calcAverageNormals(cubo_indices, 36, cubo_vertices, 192, 8, 5);
	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}
//Objetops mios
void creaCarro() {
	unsigned Carro_indices[] = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20,

		24, 25, 26,
		26, 27, 24,

		28, 29, 30,
		30, 31, 28,

		32, 33, 34,
		34, 35, 32,

		36, 37, 38,
		38, 39, 36,

		40, 41, 42,
		42, 43, 40,

		44, 45, 46,
		46, 47, 44,

	};

	GLfloat Carro_vertices[] = {

			 4.02f, -0.9f,  0.8f,  0.0f, 0.0f,  4.0f, 0.0f, 0.0f,
			 4.02f, -0.9f, -1.2f,  1.0f, 0.0f,  4.0f, 0.0f, 0.0f,
			 4.02f, -2.9f, -1.2f,  1.0f, 1.0f,  4.0f, 0.0f, 0.0f,
			 4.02f, -2.9f,  0.8f,  0.0f, 1.0f,  4.0f, 0.0f, 0.0f,

			 6.02f, -0.9f,  0.8f,  0.0f, 0.0f,  -4.0f, 0.0f, 0.0f,
			 6.02f, -0.9f, -1.2f,  1.0f, 0.0f,  -4.0f, 0.0f, 0.0f,
			 6.02f, -2.9f, -1.2f,  1.0f, 1.0f,  -4.0f, 0.0f, 0.0f,
			 6.02f, -2.9f,  0.8f,  0.0f, 1.0f,  -4.0f, 0.0f, 0.0f,

			 4.02f,  -0.9f, -1.2f,  0.0f, 0.0f,  0.80f, 0.0f, 0.0f,
			 4.02f,  -1.5f, -1.9f,  1.0f, 0.0f,  0.80f, 0.0f, 0.0f,
			 4.02f,  -2.9f, -2.2f,  1.0f, 1.0f,  0.80f, 0.0f, 0.0f,
			 4.02f,  -2.9f, -1.2f,  0.0f, 1.0f,  0.80f, 0.0f, 0.0f,

			 6.02f,  -0.9f, -1.2f,  0.0f, 0.0f,  -0.80f, 0.0f, 0.0f,
			 6.02f,  -1.5f, -1.9f,  1.0f, 0.0f,  -0.80f, 0.0f, 0.0f,
			 6.02f,  -2.9f, -2.2f,  1.0f, 1.0f,  -0.80f, 0.0f, 0.0f,
			 6.02f,  -2.9f, -1.2f,  0.0f, 1.0f,  -0.80f, 0.0f, 0.0f,

			 4.02f,  -2.9f, -2.2f,  0.0f, 0.0f,  0.0f, -0.6f, 2.8f,
			 4.02f,  -1.5f, -1.9f,  1.0f, 0.0f,  0.0f, -0.6f, 2.8f,
			 6.02f,  -1.5f, -1.9f,  1.0f, 1.0f,  0.0f, -0.6f, 2.8f,
			 6.02f,  -2.9f, -2.2f,  0.0f, 1.0f,  0.0f, -0.6f, 2.8f,

			 4.02f,  -0.9f, -1.2f,  0.0f, 0.0f,  0.0f, -1.4f, 1.2f,
			 4.02f,  -1.5f, -1.9f,  1.0f, 0.0f,  0.0f, -1.4f, 1.2f,
			 6.02f,  -1.5f, -1.9f,  1.0f, 1.0f,  0.0f, -1.4f, 1.2f,
			 6.02f,  -0.9f, -1.2f,  0.0f, 1.0f,  0.0f, -1.4f, 1.2f,

			 4.02f,  -2.9f, 0.8f,  0.0f, 0.0f,  0.0f, 0.0f, -4.0f,
			 4.02f,  -0.9f, 0.8f,  1.0f, 0.0f,  0.0f, 0.0f, -4.0f,
			 6.02f,  -0.9f, 0.8f,  1.0f, 1.0f,  0.0f, 0.0f, -4.0f,
			 6.02f,  -2.9f, 0.8f,  0.0f, 1.0f,  0.0f, 0.0f, -4.0f,

			 4.02f,  -2.9f,  0.8f,  0.0f, 0.0f,  0.0f, 6.0f, 0.0f,
			 4.02f,  -2.9f, -2.2f,  1.0f, 0.0f,  0.0f, 6.0f, 0.0f,
			 6.02f,  -2.9f, -2.2f,  1.0f, 1.0f,  0.0f, 6.0f, 0.0f,
			 6.02f,  -2.9f,  0.8f,  0.0f, 1.0f,  0.0f, 6.0f, 0.0f,

			 4.02f,  -0.9f,  0.8f,  0.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 4.02f,  -0.9f, -1.2f,  1.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 4.22f,  -0.9f, -1.2f,  1.0f, 1.0f,  0.0f, -0.4f, 0.0f,
			 4.22f,  -0.9f,  0.8f,  0.0f, 1.0f,  0.0f, -0.4f, 0.0f,

			 6.02f,  -0.9f,  0.8f,  0.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 6.02f,  -0.9f, -1.2f,  1.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 5.82f,  -0.9f, -1.2f,  1.0f, 1.0f,  0.0f, -0.4f, 0.0f,
			 5.82f,  -0.9f,  0.8f,  0.0f, 1.0f,  0.0f, -0.4f, 0.0f,

			 5.82f,  -0.9f, -1.2f,  0.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 4.22f,  -0.9f, -1.2f,  1.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 4.22f,  -0.9f, -1.0f,  1.0f, 1.0f,  0.0f, -0.4f, 0.0f,
			 5.82f,  -0.9f, -1.0f,  0.0f, 1.0f,  0.0f, -0.4f, 0.0f,

			 5.82f,  -0.9f, 0.8f,  0.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 4.22f,  -0.9f, 0.8f,  1.0f, 0.0f,  0.0f, -0.4f, 0.0f,
			 4.22f,  -0.9f, 0.6f,  1.0f, 1.0f,  0.0f, -0.4f, 0.0f,
			 5.82f,  -0.9f, 0.6f,  0.0f, 1.0f,  0.0f, -0.4f, 0.0f,

	};
	//obj 6
	Mesh *Carro = new Mesh();
	Carro->CreateMesh(Carro_vertices, Carro_indices, 8 * 4 * 12, 6 * 12);
	meshList.push_back(Carro);
}

void creaAsientos() {
	unsigned Asientos_indices[] = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20,

		24, 25, 26,
		26, 27, 24,

	};

	GLfloat Asientos_vertices[] = {

			 5.82f, -0.2f, 0.6f,  0.0f, 0.0f,  0.0f, 0.48f, 0.0f,
			 4.22f, -0.2f, 0.6f,  1.0f, 0.0f,  0.0f, 0.48f, 0.0f,
			 4.22f, -0.2f, 0.3f,  1.0f, 1.0f,  0.0f, 0.48f, 0.0f,
			 5.82f, -0.2f, 0.3f,  0.0f, 1.0f,  0.0f, 0.48f, 0.0f,

			 5.82f, -0.2f, 0.6f,  0.0f, 0.0f,  -0.81f, 0.0f, 0.0f,
			 5.82f, -0.2f, 0.3f,  1.0f, 0.0f,  -0.81f, 0.0f, 0.0f,
			 5.82f, -2.9f, 0.3f,  1.0f, 1.0f,  -0.81f, 0.0f, 0.0f,
			 5.82f, -2.9f, 0.6f,  0.0f, 1.0f,  -0.81f, 0.0f, 0.0f,

			 4.22f, -0.2f,  0.6f,  0.0f, 0.0f,  4.22f, 0.0f, 0.0f,
			 4.22f, -0.2f,  0.3f,  1.0f, 0.0f,  4.22f, 0.0f, 0.0f,
			 4.22f, -2.9f,  0.3f,  1.0f, 1.0f,  4.22f, 0.0f, 0.0f,
			 4.22f, -2.9f,  0.6f,  0.0f, 1.0f,  4.22f, 0.0f, 0.0f,

			 4.22f, -0.2f,  0.6f,  0.0f, 0.0f,  0.0f, 0.0f, -4.32f,
			 4.22f, -2.9f,  0.6f,  1.0f, 0.0f,  0.0f, 0.0f, -4.32f,
			 5.82f, -2.9f,  0.6f,  1.0f, 1.0f,  0.0f, 0.0f, -4.32f,
			 5.82f, -0.2f,  0.6f,  0.0f, 1.0f,  0.0f, 0.0f, -4.32f,

			 4.22f, -0.2f,  0.3f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 4.22f, -2.9f,  0.3f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 5.82f, -2.9f,  0.3f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			 5.82f, -0.2f,  0.3f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			 4.22f, -1.4f,  0.3f,  0.0f, 0.0f,   1.0f, -1.4f, 1.0f,
			 5.82f, -1.4f,  0.3f,  1.0f, 0.0f,   1.0f, -1.4f, 1.0f,
			 5.82f, -1.4f,  -0.5f,  1.0f, 1.0f,  1.0f, -1.4f, 1.0f,
			 4.22f, -1.4f,  -0.5f,  0.0f, 1.0f,  1.0f, -1.4f, 1.0f,

			 4.22f, -1.4f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 2.4f,
			 5.82f, -1.4f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 2.4f,
			 5.82f, -2.9f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 2.4f,
			 4.22f, -2.9f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 2.4f,

	};
	//7
	calcAverageNormals(Asientos_indices, 42, Asientos_vertices, 224, 8, 5);
	Mesh *Asientos = new Mesh();
	Asientos->CreateMesh(Asientos_vertices, Asientos_indices, 224, 42);
	meshList.push_back(Asientos);
}

void creaHexagono() {
	unsigned Hexagono_indices[] = {
		
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		8, 9, 6,

		10, 11, 12,
		13, 14, 15,
		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		23, 24, 25,
		26, 27, 28,
		28, 29, 26,

		30, 31, 32,
		33, 34, 35,
		36, 37, 38,
		38, 39, 36,

		40, 41, 42,
		43, 44, 45, 
		46, 47, 48,
		48, 49, 46,

		50, 51, 52, 
		53, 54, 55,
		56, 57, 58,
		58, 59, 56,
	};

	GLfloat Hexagono_vertices[] = {

			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			 1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, 0.0f, 0.866f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
			
			 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,    1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, 1.0f, 0.866f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 1.0f,  0.0f, 0.0f,    0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f,  0.0f, 0.866f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f,  1.0f, 0.866f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f,  0.0f,    0.0f, 1.0f, -1.0f, 0.0f, 0.0f,

			 //2
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, 0.0f, 0.866f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f, 0.0f, 0.866f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

			 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, 1.0f, 0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, 1.0f, 0.866f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.5f, 0.0f, 0.866f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			-0.5f, 0.0f, 0.866f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			-0.5f, 1.0f, 0.866f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 0.5f, 1.0f, 0.866f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

			 //3
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f, 0.0f, 0.866f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

			 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, 1.0f, 0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,    1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			-0.5f,  0.0f, 0.866f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			-1.0f,  0.0f, 0.0f,    1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,    1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			-0.5f,  1.0f, 0.866f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 
			//4
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f, 0.0f, -0.866f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

			 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,    1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, 1.0f, -0.866f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 -1.0f, 0.0f,  0.0f,    0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 -0.5f, 0.0f, -0.866f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 -0.5f, 1.0f, -0.866f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 -1.0f, 1.0f,  0.0f,    0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

			 //5
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		    -0.5f, 0.0f,-0.866f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, 0.0f,-0.866f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

			  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 -0.5f, 1.0f, -0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			  0.5f, 1.0f, -0.866f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

		   -0.5f, 0.0f, -0.866f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.5f, 0.0f, -0.866f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.5f, 1.0f, -0.866f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		   -0.5f, 1.0f, -0.866f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

			 //6
			0.0f, 0.0f, 0.0f,     0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.5f, 0.0f, -0.866f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,     1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

			 0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			0.5f, 1.0f, -0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,     1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			0.5f,  0.0f, -0.866f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			1.0f,  0.0f, 0.0f,     1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f,     1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			0.5f,  1.0f, -0.886f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,


	};
	//8 en lista meshList es el 7
	Mesh *Hexagono = new Mesh();
	Hexagono->CreateMesh(Hexagono_vertices, Hexagono_indices, 480, 72);
	meshList.push_back(Hexagono);
}

void creaBaseTriangulo() {
	unsigned BaseTriangulo_indices[] = {
		//1
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,

		12, 13, 14, 
		14, 15, 12,

		16, 17, 18,
		19, 20, 21,
		22, 23, 24,

		25, 26, 27,
		27, 28, 25,
		
		//2
		29, 30, 31,
		32, 33, 34,
		35, 36, 37, 
		38, 39, 40,

		41, 42, 43,
		43, 44, 41,

		45, 46, 47, 
		48, 49, 50, 
		51, 52, 53,

		54, 55, 56,
		56, 57, 54,
		
		//3
		58, 59, 60,
		61, 62, 63,
		64, 65, 66,
		67, 68, 69,

		70, 71, 72,
		72, 73, 70,

		74, 75, 76,
		77, 78, 79,
		80, 81, 82,

		83, 84, 85,
		85, 86, 83,
		
		//4
		87, 88, 89, 
		90, 91, 92, 
		93, 94, 95, 
		96, 97, 98,

		99, 100, 101,
		101, 102, 99,

		103, 104, 105,
		106, 107, 108,
		109, 110, 111,
		
		112, 113, 114,
		114, 115, 112,
		
		//5

		116, 117, 118,
		119, 120, 121,
		122, 123, 124, 
		125, 126, 127,

		128, 129, 130,
		130, 131, 128,

		132, 133, 134,
		135, 136, 137,
		138, 139, 140,

		141, 142, 143, 
		143, 144, 141,

		//6
		145, 146, 147, 
		148, 149, 150, 
		151, 152, 153, 
		154, 155, 156,

		157, 158, 159,
		159, 160, 157,

		161, 162, 163,
		164, 165, 166,
		167, 168, 169,

		170, 171, 172,
		172, 173, 170,
	
	};

	GLfloat BaseTriangulo_vertices[] = {
			//10 y 15 grados
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, 0.0f, 0.866f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.9848f, 0.0f, 0.1736f,    1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.9848f, 0.5f, 0.1736f,    1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.9659f, 0.0f, 0.2588f,    1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.9695f, 0.5f, 0.2588f,    1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.9848f, 0.5f, 0.1736f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.9659f, 0.5f, 0.2588f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.9848f,  0.0f, 0.1736f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.9848f,  0.5f, 0.1736f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.9659f,  0.5f, 0.2588f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 0.9659f,  0.0f, 0.2588f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			
			 //50 y 55 grados
			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.6427f, 0.0f, 0.7660f,    1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.6427f, 0.5f, 0.7660f,    1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.5735f, 0.0f, 0.8191f,    1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.5735f, 0.5f, 0.8191f,    1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.6427f, 0.5f, 0.7660f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5727f, 0.5f, 0.8191f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.6427f,  0.0f, 0.7660f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.6427f,  0.5f, 0.7660f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.5735f,  0.5f, 0.8191f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 0.5735f,  0.0f, 0.8191f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

			 //2
			 //70 y 75 grados
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, 0.0f, 0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, 0.0f, 0.866f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.3420f, 0.0f, 0.9396f,    1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.3420f, 0.5f, 0.9396f,    1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.2588f, 0.0f, 0.9659f,    1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 0.2588f, 0.5f, 0.9659f,    1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.3420f, 0.5f, 0.9396f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.2588f, 0.5f, 0.9659f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.3420f,  0.0f, 0.9396f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.3420f,  0.5f, 0.9396f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
			 0.2588f,  0.5f, 0.9659f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 0.2588f,  0.0f, 0.9659f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
			 //110 y 115
			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
		    -0.3420f, 0.0f, 0.9396f,    1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
		    -0.3420f, 0.5f, 0.9396f,    1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			-0.4426f, 0.0f, 0.9063f,    1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			-0.4426f, 0.5f, 0.9063f,    1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.3420f, 0.5f, 0.9396f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.4426f, 0.5f, 0.9063f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 -0.3420f,  0.0f, 0.9360f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 -0.3420f,  0.5f, 0.9360f,   1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 -0.4426f,  0.5f, 0.9063f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 -0.4426f,  0.0f, 0.9063f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

			 //3
			 //130 y 135
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, 0.0f, 0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			  0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 -0.6427f, 0.0f, 0.7660f,    1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 -0.6427f, 0.5f, 0.7660f,    1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			  0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 -0.7071f, 0.0f, 0.7071f,    1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			 -0.7071f, 0.5f, 0.7071f,    1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			  0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 -0.6427f, 0.5f, 0.7660f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 -0.7071f, 0.5f, 0.7071f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			-0.6427f,  0.0f, 0.7660f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			-0.6427f,  0.5f, 0.7660f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			-0.7071f,  0.5f, 0.7071f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			-0.7071f,  0.0f, 0.7071f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			//170 y 175 
			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		   -0.9848f, 0.0f, 0.1763f,    1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
		   -0.9848f, 0.5f, 0.1736f,    1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f,          0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			-0.9961f, 0.0f, 0.0871f,    1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			-0.9961f, 0.5f, 0.0871f,    1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.9848f, 0.5f, 0.1763f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.9961f, 0.5f, 0.0871f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 -0.9848f,  0.0f, 0.1736f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 -0.9848f,  0.5f, 0.1736f,   1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 -0.9961f,  0.5f, 0.0871f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
			 -0.9961f,  0.0f, 0.0871f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

			//4
			//190 y 195
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, 0.0f, -0.866f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.0f, 0.0f,0.0f,        0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
			-0.9848f, 0.0f, -0.1736f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			-0.9848f, 0.5f, -0.1736f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f,        0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			-0.9659f, 0.0f, -0.2581f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			-0.9659f, 0.5f, -0.2581f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.9848f, 0.5f, -0.1736f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.9659f, 0.5f, -0.2581f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			-0.9848f, 0.0f, -0.1736f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.9848f, 0.5f, -0.1736f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.9659f, 0.5f, -0.2581f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.9659f, 0.0f, -0.2581f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			//230 y 235
			 0.0f, 0.0f, 0.0f,         0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			-0.6427f, 0.0f, -0.7660f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			-0.6427f, 0.5f, -0.7660f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,         0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
			-0.5735f, 0.0f, -0.8191f,  1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
			-0.5735f, 0.5f, -0.8191f,  1.0f, 1.0f,   0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f,         0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
			-0.6427f, 0.5f, -0.7660f,  1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
			-0.5735f, 0.5f, -0.8191f,  1.0f, 1.0f,   0.0f, -1.0f, 0.0f,

			-0.6427f, 0.0f, -0.7660f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.6427f, 0.5f, -0.7660f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5735f, 0.5f, -0.8191f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.5735f, 0.0f, -0.8191f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

			 //5
			//250 y 255 grados	
			 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			-0.5f, 0.0f,-0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			 0.5f, 0.0f,-0.866f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			 0.0f, 0.0f, 0.0f,   0.0f,  0.0f,     0.0f, 0.0f, 1.0f,
			-0.3420f, 0.0f, -0.9396f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.3420f, 0.5f, -0.9396f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
			-0.2581f, 0.0f, -0.9659f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			-0.2581f, 0.5f, -0.9659f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,
			-0.3420f, 0.5f, -0.9396f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
			-0.2581f, 0.5f, -0.9659f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

			-0.3420f, 0.0f, -0.9396f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.3420f, 0.5f, -0.9396f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.2581f, 0.5f, -0.9659f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.2581f, 0.0f, -0.9659f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			//290 y 295
			0.0f, 0.0f, 0.0f,        0.0f, 0.0f,    0.0f, 0.0f, 1.0f,
			0.3420f, 0.0f, -0.9396f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			0.3420f, 0.5f, -0.9396f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,

			 0.0f, 0.0f, 0.0f,       0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			0.4226f, 0.0f, -0.9063f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			0.4226f, 0.5f, -0.9063f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

			0.0f, 0.0f, 0.0f,        0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			0.3420f, 0.5f, -0.9396f, 1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
			0.4226f, 0.5f, -0.9063f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

			0.3420f, 0.0f, -0.9396f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.3420f, 0.5f, -0.9396f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.4226f, 0.5f, -0.9063f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.4226f, 0.0f, -0.9063f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		   //6
			//310 y 315
		  0.0f, 0.0f, 0.0f,     0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
		  0.5f, 0.0f, -0.866f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
		  1.0f, 0.0f, 0.0f,     1.0f, 1.0f,  0.0f, -1.0f, 0.0f,

		 0.0f, 0.0f, 0.0f,         0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.6427f, 0.0f, -0.7660f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.6427f, 0.5f, -0.7660f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

		 0.0f, 0.0f, 0.0f,        0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		 0.7071f, 0.0f, -0.7071f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		 0.7071f, 0.5f, -0.7071f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

		 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		 0.6427f, 0.5f, -0.7660f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		 0.7071f, 0.5f, -0.7071f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		 0.6427f, 0.0f, -0.7660f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		 0.6427f, 0.5f, -0.7660f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		 0.7071f, 0.5f, -0.7071f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		 0.7071f, 0.0f, -0.7071f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		
		//350 y 355 grados
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.9848f, 0.0f, -0.1736f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.9848f, 0.5f, -0.1736f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		0.9961f, 0.0f, -0.0871f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		0.9961f, 0.5f, -0.0871f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, 0.0f,        0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.9848f, 0.5f, -0.1736f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.9961f, 0.5f, -0.0871f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		0.9848f, 0.0f, -0.1736f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.9848f, 0.5f, -0.1736f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.9961f, 0.5f, -0.0871f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		0.9961f, 0.0f, -0.0871f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,

	};
	//9
	Mesh *BaseTriangulo = new Mesh();
	BaseTriangulo->CreateMesh(BaseTriangulo_vertices, BaseTriangulo_indices, 1392, 198);
	meshList.push_back(BaseTriangulo);
}
//-------------------------------------------------------//
void creaBrazos()
{
	unsigned int brazo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};


	GLfloat brazo_vertices[] = {
		//1 er
		0.9961f, 0.0f,  -0.0871f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.9961f, 0.15f, -0.0871f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.9848f, 0.15f,  0.1736f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		0.9848f, 0.0f,   0.1736f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

		4.0057f, -2.0f,  -0.3502f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		4.0057f, -1.85f, -0.3502f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		4.0208f, -1.85f, -0.0925f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		4.0208f, -2.0f,  -0.0925f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

		0.9848f,  0.0f,   0.1736f,  0.0f, 0.0f,  0.0f, 0.0f, -0.3f,
		0.9848f,  0.15f,  0.1736f,  1.0f, 0.0f,  0.0f, 0.0f, -0.3f,
		4.0208f, -1.85f, -0.0925f,  1.0f, 1.0f,  0.0f, 0.0f, -0.3f,
		4.0208f, -2.0f,  -0.0925f,  0.0f, 1.0f,  0.0f, 0.0f, -0.3f,

		0.9961f,  0.0f,  -0.0871f,  0.0f, 0.0f,  0.0f, 0.0f, 0.3f,
		0.9961f,  0.15f, -0.0871f,  1.0f, 0.0f,  0.0f, 0.0f, 0.3f,
		4.0057f, -1.85f, -0.3502f,  1.0f, 1.0f,  0.0f, 0.0f, 0.3f,
		4.0057f, -2.0f,  -0.3502f,  0.0f, 1.0f,  0.0f, 0.0f, 0.3f,

		0.9848f,  0.15f,  0.1736f,  0.0f, 0.0f,  -0.5f, -0.5f, -0.02f,
		0.9961f,  0.15f, -0.0871f,  1.0f, 0.0f,  -0.5f, -0.5f, -0.02f,
		4.0057f, -1.85f, -0.3502f,  1.0f, 1.0f, -0.5f, -0.5f, -0.02f,
		4.0208f, -1.85f, -0.0925f,  0.0f, 1.0f,  -0.5f, -0.5f, -0.02f,

		4.0208f, -2.0f, -0.0925f,  0.0f, 0.0f,  0.5f, 0.5f, -0.02f,
		4.0057f, -2.0f, -0.3502f,  1.0f, 0.0f,  0.5f, 0.5f, -0.02f,
		0.9961f,  0.0f, -0.0871f,  1.0f, 1.0f,  0.5f, 0.5f, -0.02f,
		0.9848f,  0.0f,  0.1736f,  0.0f, 1.0f,  0.5f, 0.5f, -0.02f,

	};
	//calcAverageNormals(brazo_indices, 6*6, brazo_vertices, 32*6, 8, 5);
	Mesh *brazo = new Mesh();
	brazo->CreateMesh(brazo_vertices, brazo_indices, 32 * 6, 6 * 6);
	meshList.push_back(brazo);

}

void myData() {

		GLfloat vertices[] = {
			//Position				//Color
			//Cara de enfrente
		 0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,    // top right
		 0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,    // bottom right
		-0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,    // bottom left
		-0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,    // top left 

		//Cara de atrás
		-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,    // top right
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,    // bottom right
		 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,    // bottom left
		 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,    // top left 

		//Cara de arriba
		 0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f,    // top right
		 0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,    // bottom right
		-0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,    // bottom left
		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,    // top left 

		//Cara de abajo
		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,    // top right
		 0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,    // bottom right
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,    // bottom left
		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,    // top left 

		//Cara de la derecha
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,    // top right
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    // bottom right
		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,    // bottom left
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,    // top left 

		 //Cara de la izquierda
		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,    // top right
		-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    // bottom right
		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,    // bottom left
		-0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,    // top left 
		};

		GLfloat indices[] =	//I am not using index for this session
		{
			0
		};

		Mesh_Poli *Mydata = new Mesh_Poli();
		Mydata->CreateMesh_Poli(vertices, indices,8*4*6,0);
		meshPoliList.push_back(Mydata);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}
//RECORRIDO

//Recorrido
float rX, rY, rZ;
bool recorrido = false;
int cont = 0;
bool recorridoMont = false;

///////////////////////////////KEYFRAMES/////////////////////
bool animacion = false;

//NEW// Keyframes
float posXdragon = -60.0, posYdragon = 10.0, posZDragon = 20;
float movAvion_x = 0.0f, movAvion_y = 0.0f, movAvion_z = 0.0f;
float sqX = 0.0f, sqY = 0.0f, sqZ = 0.0f;
float posXsq = 0.0, posYsq = 0.0, posZsq = 50.0f;

float giroDragon = 0.0f, giroDragon2 = 0.0f;
float rotAlas = 0.0f;
//bool recorridoSq = 0;

#define MAX_FRAMES 20
int i_max_steps = 90;
int i_curr_steps = 5;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_z;		//Variable para PosicionZ
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float movAvion_zInc;		//Variable para IncrementoZ
	float giroDragon;
	float giroDragonInc;
	float giroDragon2;
	float giroDragon2Inc; 
	float rotAlas;
	float rotAlasInc;
	float sqX;
	float sqY;
	float sqZ;
	float sqX_inc;
	float sqY_inc;
	float sqZ_inc;


}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 20;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	//printf("frameindex %d\n", FrameIndex);
	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].movAvion_z = movAvion_z;
	KeyFrame[FrameIndex].giroDragon = giroDragon;
	KeyFrame[FrameIndex].giroDragon2 = giroDragon2;
	KeyFrame[FrameIndex].rotAlas = rotAlas;
	KeyFrame[FrameIndex].sqX = sqX;
	KeyFrame[FrameIndex].sqY = sqY;
	KeyFrame[FrameIndex].sqZ = sqZ;

	FrameIndex++;
}

void resetElements(void)
{
	
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	movAvion_z = KeyFrame[0].movAvion_z;
	giroDragon= KeyFrame[0].giroDragon;
	giroDragon2 = KeyFrame[0].giroDragon2;
	rotAlas = KeyFrame[0].rotAlas;
	sqX = KeyFrame[0].sqX;
	sqY = KeyFrame[0].sqY;
	sqZ = KeyFrame[0].sqZ;

}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].movAvion_zInc = (KeyFrame[playIndex + 1].movAvion_z - KeyFrame[playIndex].movAvion_z) / i_max_steps;
	KeyFrame[playIndex].giroDragonInc = (KeyFrame[playIndex + 1].giroDragon - KeyFrame[playIndex].giroDragon) / i_max_steps;
	KeyFrame[playIndex].giroDragon2Inc = (KeyFrame[playIndex + 1].giroDragon2 - KeyFrame[playIndex].giroDragon2) / i_max_steps;
	KeyFrame[playIndex].rotAlasInc = (KeyFrame[playIndex + 1].rotAlas - KeyFrame[playIndex].rotAlas) / i_max_steps;
	KeyFrame[playIndex].sqX_inc = (KeyFrame[playIndex + 1].sqX - KeyFrame[playIndex].sqX) / i_max_steps;
	KeyFrame[playIndex].sqY_inc = (KeyFrame[playIndex + 1].sqY - KeyFrame[playIndex].sqY) / i_max_steps;
	KeyFrame[playIndex].sqZ_inc = (KeyFrame[playIndex + 1].sqZ - KeyFrame[playIndex].sqZ) / i_max_steps;

}

void animate(void)
{
	//Movimiento del objeto
	if (play)
	{	
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			//printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				//printf("Frame index= %d\n", FrameIndex);
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aquí\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//printf("se quedó aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			movAvion_z += KeyFrame[playIndex].movAvion_zInc;
			giroDragon += KeyFrame[playIndex].giroDragonInc;
			giroDragon2 += KeyFrame[playIndex].giroDragon2Inc;
			rotAlas += KeyFrame[playIndex].rotAlasInc;
			sqX += KeyFrame[playIndex].sqX_inc;
			sqY += KeyFrame[playIndex].sqY_inc;
			sqZ += KeyFrame[playIndex].sqZ_inc;

			i_curr_steps++;
		}

	}
}

/* FIN KEYFRAMES*/

float movRecorrido() {
	float Reco[] = { -0.365465, 0.278234, 128.682175
   , -0.616343, 0.469231, 127.777542
   , -0.752767, 0.573092, 127.285614
   , -1.026232, 0.781285, 126.299538
   , -1.026232, 0.781285, 126.299538
   , -1.112031, 0.846605, 125.990158
   , -1.461327, 1.112529, 124.730644
   , -1.948754, 1.483614, 122.973038
   , -2.299326, 1.750510, 121.708916
   , -2.740139, 2.098551, 119.849342
   , -2.972240, 2.258693, 118.655281
   , -3.365858, 2.560009, 116.442383
   , -3.471861, 2.661280, 115.773102
   , -3.711913, 2.890615, 114.257477
   , -3.885397, 3.056355, 113.162148
   , -4.042408, 3.206356, 112.170815
   , -4.353299, 3.529828, 110.029060
   , -4.473593, 3.701820, 108.884537
   , -4.551463, 3.813154, 108.143661
   , -4.631977, 3.928271, 107.377617
   , -4.722271, 4.057369, 106.518532
   , -4.857934, 4.251336, 105.227783
   , -5.022285, 4.486320, 103.664093
   , -5.056556, 4.535318, 103.338038
   , -5.160316, 4.683670, 102.350815
   , -5.324276, 4.918094, 100.790833
   , -5.324276, 4.918094, 100.790834
   , -5.459138, 5.110917, 99.507698
   , -5.523116, 5.110917, 99.193237
   , -5.611937, 5.107028, 98.756668
   , -5.893230, 5.048222, 93.967636
   , -5.879997, 4.992981, 93.097544
   , -6.538341, 4.893899, 91.116644
   , -6.621172, 4.892251, 90.946815
   , -7.259467, 4.871477, 89.638123
   , -7.957078, 4.669010, 88.20780
   , -8.667078, 4.441386, 86.75208
 , -8.816249, 4.393562, 86.446236
   , -9.614968, 4.137494, 84.80861
   , -9.693785, 4.112225, 84.64701
   , -10.312939, 3.913725, 83.377563
   , -10.393941, 3.887756, 83.211487
   , -10.469992, 3.863375, 83.055557
   , -11.253526, 3.611041, 80.757599
   , -11.253526, 3.611041, 80.757599
   , -11.643114, 3.477711, 79.251167
   , -11.643114, 3.477711, 79.251167
   , -12.449708, 3.184769, 76.132294
   , -12.600256, 3.126873, 75.550171
   , -12.853838, 3.029352, 74.569641
   , -12.853838, 3.029352, 74.569641
   , -13.017173, 2.966538, 73.938072
   , -13.017173, 2.966538, 73.938072
   , -13.425824, 2.809383, 72.357941
   , -13.425824, 2.809383, 72.357941
   , -13.425824, 2.809383, 72.357941
   , -13.702333, 2.703045, 71.288765
   , -13.702333, 2.703045, 71.288765
   , -14.645091, 2.432866, 67.958191
   , -14.645091, 2.432866, 67.958191
   , -15.074125, 2.340474, 66.509796
   , -15.074125, 2.340474, 66.509796
   , -15.074125, 2.340474, 66.509796
   , -15.427739, 2.268700, 65.390961
   , -15.427739, 2.268700, 65.390961
   , -15.550605, 2.245618, 65.034134
   , -15.951004, 2.170397, 63.871288
   , -15.951004, 2.170397, 63.871288
   , -17.042057, 2.234251, 61.740860
   , -17.042057, 2.234251, 61.740860
 , -17.042057, 2.234251, 61.740860
   , -17.272730, 2.219050, 61.371708
   , -17.272730, 2.219050, 61.371708
   , -17.272730, 2.219050, 61.371708
   , -18.400919, 2.226977, 59.709740
   , -18.400919, 2.226977, 59.709740
 , -18.896406, 2.253896, 59.119240
   , -18.896406, 2.253896, 59.119240
   , -18.896406, 2.253896, 59.119240
   , -20.449671, 2.297379, 57.250931
   , -20.833660, 2.281226, 56.768192
   , -20.833660, 2.281226, 56.768192
   , -21.622967, 2.248024, 55.775898
   , -21.622967, 2.248024, 55.775898
   , -22.006920, 2.231874, 55.293201
  , 23.018387, 2.189327, 54.021610
   , -23.392927, 2.173572, 53.550747
   , -24.149048, 2.141765, 52.600174
   , -24.149048, 2.141765, 52.600174
   , -24.525429, 2.125933, 52.126999
   , -26.731903, 2.033118, 49.353085
   , -28.306932, 1.966865, 47.373001
   , -28.306932, 1.966865, 47.373001
   , -28.306932, 1.966865, 47.373001
   , -28.851433, 1.943961, 46.688473
   , -29.641132, 1.910742, 45.695686
   , -30.737514, 1.864623, 44.317345
   , -30.737514, 1.864623, 44.317345
   , -30.737514, 1.864623, 44.317345
   , -32.676460, 1.783062, 41.879753
   , -32.676460, 1.783062, 41.879753
   , -32.676460, 1.783062, 41.879753
   , -33.209923, 1.760622, 41.209099
   , -33.209923, 1.760622, 41.209099
   , -34.832409, 1.692373, 39.169357
   , -34.832409, 1.692373, 39.169357
   , -34.832409, 1.692373, 39.169357
   , -35.643803, 1.658242, 38.149292
   , -35.643803, 1.658242, 38.149292
   , -35.643803, 1.658242, 38.149292
   , -36.411278, 1.625958, 37.184441
   , -36.411278, 1.625958, 37.184441
   , -36.411278, 1.625958, 37.184441
   , -36.411278, 1.625958, 37.184441
   , -36.411278, 1.625958, 37.184441
   , -37.660675, 1.843564, 36.167011
   , -39.870731, 2.300558, 34.871082
   , -39.870731, 2.300558, 34.871082
   , -40.227180, 2.373503, 34.661118
   , -40.967140, 2.524931, 34.225246
   , -40.967140, 2.524931, 34.225246
   , -41.689522, 2.672762, 33.799732
   , -43.774357, 3.009296, 32.571671
   , -43.774357, 3.009296, 32.571671
   , -43.774357, 3.009296, 32.571671
   , -43.774357, 3.009296, 32.571671
   , -46.176857, 3.185511, 30.981495
   , -46.691605, 2.931363, 29.902300
   , -46.691605, 2.931363, 29.902300
   , -46.691605, 2.931363, 29.902300
   , -46.691605, 2.931363, 29.902300
   , -47.295647, 2.674525, 28.576847
   , -47.295647, 2.674525, 28.576847
   , -47.295647, 2.674525, 28.576847
   , -47.295647, 2.674525, 28.576847
   , -47.295647, 2.674525, 28.576847
   , -47.295647, 2.674525, 28.576847
   , -47.720703, 2.493792, 27.644146
   , -47.820267, 2.451457, 27.425671
   , -48.290329, 2.251586, 26.394209
   , -49.011494, 1.944947, 24.811756
   , -49.011494, 1.944947, 24.811756
   , -49.011494, 1.944947, 24.81175
   , -50.083939, 1.393499, 21.702005
   , -50.083939, 1.393499, 21.702005
   , -50.083939, 1.393499, 21.702005
 , -50.171345, 1.335428, 21.323404
   , -50.586834, 1.047418, 21.395555
   , -50.586834, 1.047418, 21.395555
   , -50.586834, 1.047418, 21.395555
 , -50.590492, 1.014438, 21.814585
   , -50.151554, 0.824581, 25.674416
   , -48.056660, 0.737987, 26.804007
   , -48.056660, 0.737987, 26.804007
   , -45.987484, 0.663889, 27.273815
   , -45.987484, 0.663889, 27.273815
 , -45.387203, 0.642187, 27.434660
   , -45.387203, 0.642187, 27.434660
   , -44.083401, 0.594939, 27.796236
   , -44.083401, 0.594939, 27.796236
   , -44.083401, 0.594939, 27.796236
   , -43.260284, 0.565072, 28.028509
   , -43.260284, 0.565072, 28.028509
   , -42.035446, 0.520346, 28.40297
   , -42.035446, 0.520346, 28.402979
   , -40.453758, 0.462588, 28.886549
   , -40.453758, 0.462588, 28.886549
   , -40.135807, 0.450978, 28.983755
   , -38.838341, 0.403599, 29.380432
   , -38.838341, 0.403599, 29.380432
   , -38.838341, 0.403599, 29.380432
   , -38.489403, 0.390857, 29.487114
   , -37.827835, 0.366699, 29.689375
   , -35.511848, 0.299735, 30.397444
   , -33.529907, 0.354005, 31.003386
   , -31.900461, 0.398623, 31.501556
   , -31.900461, 0.398623, 31.501556
   , -31.900461, 0.398623, 31.501556
   , -31.900461, 0.398623, 31.501556
   , -31.900461, 0.398623, 31.501556
   , -30.721281, 0.430912, 31.862070
   , -30.721281, 0.430912, 31.862070
   , -30.325680, 0.441744, 31.983017
   , -30.325680, 0.441744, 31.983017
   , -29.542755, 0.463183, 32.222382
   , -29.542755, 0.463183, 32.222382
   , -29.542755, 0.463183, 32.22238
   , -27.303205, 0.524507, 32.907078
   , -27.303205, 0.524507, 32.907078
   , -27.303205, 0.524507, 32.907078
   , -27.303205, 0.524507, 32.907078
   , -26.135111, 0.556492, 33.264202
   , -26.135111, 0.556492, 33.264202
 , -26.135111, 0.556492, 33.264202
   , -25.318893, 0.578842, 33.513744
   , -24.543846, 0.600065, 33.750698
   , -23.577600, 0.626523, 34.046108
   , -23.577600, 0.626523, 34.046108
   , -23.577600, 0.626523, 34.046108
   , -22.804379, 0.647696, 34.282509
   , -22.804379, 0.647696, 34.282509
   , -22.804379, 0.647696, 34.282509
   , -21.213114, 0.691269, 34.769005
   , -20.431503, 0.712671, 35.007969
   , -20.431503, 0.712671, 35.007969
   , -20.431503, 0.712671, 35.007969
   , -20.431503, 0.712671, 35.007969
   , -19.465805, 0.739114, 35.303211
   , -19.465805, 0.739114, 35.303211
   , -19.465805, 0.739114, 35.303211
   , -19.465805, 0.739114, 35.303211
   , -19.465805, 0.739114, 35.303211
   , -18.698195, 0.760133, 35.537891
  , -18.698195, 0.760133, 35.537891
   , -18.698195, 0.760133, 35.537891
   , -18.698195, 0.760133, 35.537891
   , -18.102203, 0.776453, 35.720104
   , -18.102203, 0.776453, 35.720104
   , -18.102203, 0.776453, 35.720104
   , -16.813656, 0.811736, 36.114052
   , -16.813656, 0.811736, 36.114052
   , -16.813656, 0.811736, 36.114052
   , -16.813656, 0.811736, 36.114052
   , -16.636860, 0.816577, 36.168102
   , -15.666639, 0.843144, 36.464733
   , -15.666639, 0.843144, 36.464733
  , -15.666639, 0.843144, 36.464733
   , -15.666639, 0.843144, 36.464733
   , -15.666639, 0.843144, 36.464733
   , -15.666639, 0.843144, 36.464733
   , -15.453632, 0.848977, 36.529858
   , -14.295932, 0.880678, 36.883801
   , -13.293729, 0.908120, 37.1902
   , -12.338167, 0.934286, 37.482346
   , -12.338167, 0.934286, 37.482346
   , -12.338167, 0.934286, 37.482346
   , -12.338167, 0.934286, 37.482346
   , -12.338167, 0.934286, 37.482346
   , -11.549665, 0.955877, 37.723415
   , -11.549665, 0.955877, 37.723415
   , -11.549665, 0.955877, 37.723415
   , -11.549665, 0.955877, 37.723415
   , -11.549665, 0.955877, 37.723415
   , -11.549665, 0.955877, 37.723415
   , -10.394517, 0.987508, 38.076576
   , -10.394517, 0.987508, 38.076576
   , -8.492987, 1.039576, 38.657940
   , -8.492987, 1.039576, 38.657940
   , -7.888827, 1.056120, 38.842651
   , -7.888827, 1.056120, 38.842651
   , -7.888827, 1.056120, 38.842651
   , -7.888827, 1.056120, 38.842651
   , -7.888827, 1.056120, 38.842651
   , -4.855048, 1.139192, 39.770176
   , -4.855048, 1.139192, 39.770176
   , -4.855048, 1.139192, 39.770176
   , -4.070666, 1.160670, 40.009987
   , -4.070666, 1.160670, 40.009987
   , -4.070666, 1.160670, 40.009987
  , -4.070666, 1.160670, 40.009987
   , -3.853246, 1.166624, 40.076458
   , -3.134651, 1.186301, 40.296154
   , -3.134651, 1.186301, 40.296154
   , -3.134651, 1.186301, 40.296154
   , -3.134651, 1.186301, 40.296154
   , -2.142440, 1.213470, 40.599503
   , -2.142440, 1.213470, 40.599503
   , -2.142440, 1.213470, 40.599503
   , -2.142440, 1.213470, 40.599503
  ,0.808083, 1.294262, 41.501568
  , 0.808083, 1.294262, 41.501568
  ,0.808083, 1.294262, 41.501568
  ,0.808083, 1.294262, 41.501568
  ,0.808083, 1.294262, 41.501568
  ,1.886541, 1.323793, 41.831284
  ,1.886541, 1.323793, 41.831284
  ,1.886541, 1.323793, 41.831284
  ,1.886541, 1.323793, 41.831284
  ,1.886541, 1.323793, 41.831284
  ,1.886541, 1.323793, 41.831284
  ,2.723546, 1.346712, 42.087185
  ,2.723546, 1.346712, 42.087185
  ,2.723546, 1.346712, 42.087185
  ,8.069444, 1.419798, 43.264549
  ,8.069444, 1.419798, 43.264549
  ,8.069444, 1.419798, 43.264549
  ,8.069444, 1.419798, 43.264549
  ,9.295653, 1.331232, 43.581669
  ,9.295653, 1.331232, 43.581669
  ,10.257877, 1.322559, 43.830521
  ,10.257877, 1.322559, 43.830521
  ,10.736016, 1.318249, 43.954174
  ,11.155621, 1.314466, 44.062691
  ,13.894497, 1.289778, 44.771011
  ,14.273293, 1.286364, 44.868977
  ,15.287108, 1.277225, 45.131168
  ,15.287108, 1.277225, 45.131168
  ,16.115681, 1.269756, 45.345455
  ,16.115681, 1.269756, 45.345455
  ,16.115681, 1.269756, 45.345455
  ,16.632633, 1.265097, 45.479149
  ,16.632633, 1.265097, 45.47914
  ,18.173334, 1.251209, 45.877602
  ,18.173334, 1.251209, 45.877602
  ,19.277113, 1.241259, 46.163055
  ,19.277113, 1.241259, 46.163055
  ,20.139109, 1.233489, 46.385983
  ,22.902950, 1.208576, 47.100761
  ,22.902950, 1.208576, 47.100761
  ,22.902950, 1.208576, 47.100761
  ,25.273653, 1.187134, 47.744896
  ,25.273653, 1.187134, 47.744896
  ,25.273653, 1.187134, 47.744896
  ,25.746710, 1.182828, 47.885021
  ,25.746710, 1.182828, 47.885021
  ,26.849874, 1.172788, 48.211796
  ,27.330099, 1.168417, 48.354046
  ,27.972424, 1.162571, 48.544312
  ,31.076468, 1.141430, 49.471207
  ,31.076468, 1.141430, 49.471207
  ,31.890194, 1.141430, 49.719986
  ,31.890194, 1.141430, 49.719986
  ,31.890194, 1.141430, 49.719986
  ,32.734402, 1.145116, 49.978085
  ,32.734402, 1.145116, 49.978085
  ,32.734402, 1.145116, 49.978085
  ,33.514885, 1.166487, 50.216702
  ,33.514885, 1.166487, 50.216702
  ,36.297146, 1.242672, 51.067326
  ,36.297146, 1.242672, 51.067326
  ,36.297146, 1.242672, 51.067326
  ,36.696686, 1.253613, 51.189476
  ,37.485477, 1.275212, 51.430634
  ,37.485477, 1.275212, 51.430634
  ,37.485477, 1.275212, 51.4306
  ,40.300705, 1.352299, 52.291332
  ,40.300705, 1.352299, 52.291332
  ,40.300705, 1.352299, 52.291332
  ,40.300705, 1.352299, 52.291332
  ,41.620834, 1.388448, 52.694931
  ,41.620834, 1.388448, 52.694931
  ,41.620834, 1.388448, 52.694931
  ,43.107544, 1.429157, 53.149464
  ,43.107544, 1.429157, 53.149464
  ,43.107544, 1.429157, 53.149464
  ,43.107544, 1.429157, 53.149464
  ,43.107544, 1.429157, 53.149464
  ,44.128456, 1.477251, 52.840004
  ,44.128456, 1.477251, 52.840004
  ,44.128456, 1.477251, 52.840004
  ,44.170410, 1.491267, 52.440838
  ,44.287254, 1.525532, 51.466602
  ,44.27254, 1.525532, 51.466602
  ,44.287254, 1.525532, 51.466602
  ,44.330669, 1.534242, 51.136845
  ,43.080681, 1.677424, 49.432529
  ,43.080681, 1.677424, 49.432529
  ,43.080681, 1.677424, 49.432529
  ,42.424671, 1.739571, 49.072498
  ,42.424671, 1.739571, 49.072498
  ,42.424671, 1.739571, 49.072498
  ,42.424671, 1.739571, 49.072498
  ,42.424671, 1.739571, 49.072498
  ,42.424671, 1.739571, 49.072498
  ,41.370743, 1.848176, 48.670864
  ,41.370743, 1.848176, 48.670864
  ,41.370743, 1.848176, 48.670864
  ,40.741127, 1.915918, 48.356949
  ,40.741127, 1.915918, 48.356949
  ,40.741127, 1.915918, 48.356949
  ,38.627777, 2.145332, 47.256809
  ,38.627777, 2.145332, 47.256809
  ,38.627777, 2.145332, 47.256809
  ,38.627777, 2.145332, 47.256809
  ,35.436291, 2.856925, 43.045685
  ,35.436291, 2.856925, 43.045685
  ,35.436291, 2.856925, 43.045685
 ,35.317387, 3.013900, 42.00209
  ,35.205242, 3.161955, 41.017799
  ,35.205242, 3.161955, 41.017799
  ,35.083351, 3.322877, 39.947968
  ,35.083351, 3.322877, 39.947968
  ,35.083351, 3.322877, 39.947968
  ,35.021698, 3.404274, 39.406826
  ,34.926216, 3.530328, 38.568802
  ,34.926216, 3.530328, 38.568802
  ,34.804630, 3.690845, 37.501667
  ,34.804630, 3.690845, 37.501667
  ,34.674629, 3.862468, 36.360687
  ,34.610649, 3.946933, 35.799156
  ,34.362431, 4.274630, 33.620579
  ,34.362431, 4.274630, 33.6205
  ,34.300579, 4.356287, 33.077713
  ,34.300579, 4.356287, 33.0777
  ,34.172699, 4.525114, 31.955324
  ,34.172699, 4.525114, 31.955324
  ,33.975170, 4.785894, 30.221622
  ,33.913326, 4.867539, 29.67883
  ,33.770241, 5.045179, 28.422983
  ,33.705772, 5.125218, 27.85713
  ,33.644604, 5.201159, 27.32026
  ,33.644604, 5.201159, 27.320267
  ,33.202419, 5.750134, 23.439217
  ,33.202419, 5.750134, 23.439217
  ,33.141884, 5.825289, 22.907902
  ,32.892712, 6.134635, 20.720934
  ,32.892712, 6.134635, 20.720934
  ,32.892712, 6.134635, 20.720934
  ,32.714088, 6.356394, 19.153185
  ,32.714088, 6.356394, 19.153185
	};
	return Reco[cont];
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	//
	
	//
	CreateObjects();
	CreateShaders();
	CrearCubo();

	creaCarro();
	creaAsientos();
	creaHexagono();
	creaBaseTriangulo();
	creaBrazos();
	myData();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	//Texturas Carlos
	plainTexture = Texture("Textures/piso.jpg");
	plainTexture.LoadTexture();
	//Texturas Juego Giratorio
	amarilloTexture = Texture("Textures/Amarillo.png");
	amarilloTexture.LoadTexture();
	laminaCirculoTexture = Texture("Textures/Circulo.png");
	laminaCirculoTexture.LoadTextureA();
	azulTexture = Texture("Textures/PlasticoAzul.png");
	azulTexture.LoadTextureA();
	amarillo2Texture = Texture("Textures/AmarilloPuntos.png");
	amarillo2Texture.LoadTextureA();
	rojoMetalTexture = Texture("Textures/Rojo.png");
	rojoMetalTexture.LoadTextureA();
	circuitoAzulTexture = Texture("Textures/Circuitoazul.jpg");
	circuitoAzulTexture.LoadTexture();
	MetalPisoTexture = Texture("Textures/PisoCirculo.jpg");
	MetalPisoTexture.LoadTexture();
	FloresTexture = Texture("Textures/AmarilloFlores.png");
	FloresTexture.LoadTextureA();
	CajaMetalTexture = Texture("Textures/Caja.jpg");
	CajaMetalTexture.LoadTexture();
	coloresTexture = Texture("Textures/Colores.jpg");
	coloresTexture.LoadTexture();
	RayasTexture = Texture("Textures/PisoGira.jpg");
	RayasTexture.LoadTexture();

	JuegoTexture = Texture("Texture/JuegoSillasVoladoras.mtl");
	JuegoTexture.LoadTextureA();
	
	

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//Juego acuatico

	Mar = Model();
	Mar.LoadModel("Models/Mar.obj");

	Alberca = Model();
	Alberca.LoadModel("Models/ALberca.obj");


	//Juego de las sillas
	sillas = Model();
	sillas.LoadModel("Models/Silla.obj");
	
	sq1 = Model();
	sq1.LoadModel("Models/Charizard/sqCuerpo2.obj");
	sq2 = Model();
	sq2.LoadModel("Models/Charizard/sqPder.obj");
	sq3 = Model();
	sq3.LoadModel("Models/Charizard/sqPizq.obj");
	sq4 = Model();
	sq4.LoadModel("Models/Charizard/sqCola.obj");
	
	/**************/
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		2.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,
		3.0f, 3.0f,
		0.0, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		4.0f);
	spotLightCount++;
	//luz de faro
	spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f, //Color de la luz
		1.0f, 1.0f, //Intensidad de izquierda y derecha
		0.0f, 0.0f, 0.0f, //posicion
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	//luz de helicoptero
	spotLights[3] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 1.0f,
		0.0, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	
	spotLights[4] = SpotLight(1.0f, 1.0f, 0.0f,
		3.0f, 3.0f,
		0.0, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		4.0f);
	spotLightCount++;

	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);
	glm::vec3 posSquartle = glm::vec3(2.0f, 0.0f, 0.0f);

//KEYFRAMES DECLARADOS INICIALES
	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].movAvion_z = 0.0f;
	KeyFrame[0].giroDragon = 0.0f;
	KeyFrame[0].giroDragon2 = 0.0f;
	KeyFrame[0].rotAlas = 7.0f;
	KeyFrame[0].sqX = 0.0f;
	KeyFrame[0].sqY = 0.0f;
	KeyFrame[0].sqZ = 0.0f;

	KeyFrame[1].movAvion_x = 5.0f;
	KeyFrame[1].movAvion_y = 1.0f;
	KeyFrame[1].movAvion_z = 0.0f;
	KeyFrame[1].giroDragon = 10.0;
	KeyFrame[1].giroDragon2 = 0.0;
	KeyFrame[1].rotAlas = -7.0f;
	KeyFrame[1].sqX = 0.0f;
	KeyFrame[1].sqY = 1.0f;
	KeyFrame[1].sqZ = 3.0f;

	KeyFrame[2].movAvion_x = 12.0f;
	KeyFrame[2].movAvion_y = 0.0f;
	KeyFrame[2].movAvion_z = 0.0f;
	KeyFrame[2].giroDragon = 0.0f;
	KeyFrame[2].giroDragon2 = 0.0;
	KeyFrame[2].rotAlas = 7.0f;
	KeyFrame[2].sqX = 0.0f;
	KeyFrame[2].sqY = 0.0f;
	KeyFrame[2].sqZ = 5.0f;

	KeyFrame[3].movAvion_x = 18.0f;
	KeyFrame[3].movAvion_y = -1.0f;
	KeyFrame[3].movAvion_z = 0.0f;
	KeyFrame[3].giroDragon = -10.0f;
	KeyFrame[3].rotAlas = -7.0f;
	KeyFrame[3].giroDragon2 = 0.0f;
	KeyFrame[3].sqX = 0.0f;
	KeyFrame[3].sqY = 1.0f;
	KeyFrame[3].sqZ = 8.0f;

	KeyFrame[4].movAvion_x = 24.0f;
	KeyFrame[4].movAvion_y = 0.0f;
	KeyFrame[4].movAvion_z = 0.0f;
	KeyFrame[4].giroDragon = 0.0f;
	KeyFrame[4].rotAlas = 7.0;
	KeyFrame[4].giroDragon2 = 90.0f;
	KeyFrame[4].sqX = 0.0f;
	KeyFrame[4].sqY = 0.0f;
	KeyFrame[4].sqZ = 10.0f;

	KeyFrame[5].movAvion_x = 28.0f;
	KeyFrame[5].movAvion_y = 0.0f;
	KeyFrame[5].movAvion_z = -5.0f;
	KeyFrame[5].giroDragon = 0.0f;
	KeyFrame[5].rotAlas = -7.0f;
	KeyFrame[5].giroDragon2 = 90.0f;
	KeyFrame[5].sqX = 0.0f;
	KeyFrame[5].sqY = 1.0f;
	KeyFrame[5].sqZ = 12.0f;

	KeyFrame[6].movAvion_x = 28.0f;
	KeyFrame[6].movAvion_y = 3.0f;
	KeyFrame[6].movAvion_z = -8.0f;
	KeyFrame[6].giroDragon = 10.0;
	KeyFrame[6].rotAlas = 7.0f;
	KeyFrame[6].giroDragon2 = 90.0;
	KeyFrame[6].sqX = 0.0f;
	KeyFrame[6].sqY = 0.0f;
	KeyFrame[6].sqZ = 13.0f;

	KeyFrame[7].movAvion_x = 28.0f;
	KeyFrame[7].movAvion_y = 0.0f;
	KeyFrame[7].movAvion_z = -13.0f;
	KeyFrame[7].giroDragon = 0.0f;
	KeyFrame[7].rotAlas = -7.0f;
	KeyFrame[7].giroDragon2 = 90.0f;
	KeyFrame[7].sqX = 0.0f;
	KeyFrame[7].sqY = 1.0f;
	KeyFrame[7].sqZ = 14.0f;

	KeyFrame[8].movAvion_x = 28.0f;
	KeyFrame[8].movAvion_y = -1.0f;
	KeyFrame[8].movAvion_z = -15.0f;
	KeyFrame[8].giroDragon = -10.0f;
	KeyFrame[8].rotAlas = 7.0f;
	KeyFrame[8].giroDragon2 = 90.0f;
	KeyFrame[8].sqX = 0.0f;
	KeyFrame[8].sqY = 0.0f;
	KeyFrame[8].sqZ = 15.0f;

	KeyFrame[9].movAvion_x = 28.0f;
	KeyFrame[9].movAvion_y = 0.0f;
	KeyFrame[9].movAvion_z = -20.0f;
	KeyFrame[9].giroDragon = 0.0f;
	KeyFrame[9].rotAlas = -7.0f;
	KeyFrame[9].giroDragon2 = 90.0f;
	KeyFrame[9].sqX = 0.0f;
	KeyFrame[9].sqY = 1.0f;
	KeyFrame[9].sqZ = 16.0f;

	KeyFrame[10].movAvion_x = 24.0f;
	KeyFrame[10].movAvion_y = 0.0f;
	KeyFrame[10].movAvion_z = -20.0f;
	KeyFrame[10].giroDragon = 0.0f;
	KeyFrame[10].rotAlas = 7.0f;
	KeyFrame[10].giroDragon2 = 180.0f;
	KeyFrame[10].sqX = 0.0f;
	KeyFrame[10].sqY = 0.0f;
	KeyFrame[10].sqZ = 17.0f;

	KeyFrame[11].movAvion_x = 18.0f;
	KeyFrame[11].movAvion_y = 3.0f;
	KeyFrame[11].movAvion_z = -20.0f;
	KeyFrame[11].giroDragon = 10.0;
	KeyFrame[11].rotAlas = -7.0f;
	KeyFrame[11].giroDragon2 = 180.0;

	KeyFrame[12].movAvion_x = 12.0f;
	KeyFrame[12].movAvion_y = 0.0f;
	KeyFrame[12].movAvion_z = -20.0f;
	KeyFrame[12].giroDragon = 0.0f;
	KeyFrame[12].rotAlas = 7.0f;
	KeyFrame[12].giroDragon2 = 180.0f;

	KeyFrame[13].movAvion_x = 5.0f;
	KeyFrame[13].movAvion_y = -1.0f;
	KeyFrame[13].movAvion_z = -20.0f;
	KeyFrame[13].giroDragon = -10.0f;
	KeyFrame[13].rotAlas = -7.0f;
	KeyFrame[13].giroDragon2 = 180.0f;

	KeyFrame[14].movAvion_x = 0.0f;
	KeyFrame[14].movAvion_y = 0.0f;
	KeyFrame[14].movAvion_z = -20.0f;
	KeyFrame[14].giroDragon = 0.0f;
	KeyFrame[14].rotAlas = 7.0f;
	KeyFrame[14].giroDragon2 = 180.0f;


	KeyFrame[15].movAvion_x = 0.0f;
	KeyFrame[15].movAvion_y = 0.0f;
	KeyFrame[15].movAvion_z = -15.0f;
	KeyFrame[15].giroDragon = 0.0f;
	KeyFrame[15].rotAlas = -7.0f;
	KeyFrame[15].giroDragon2 = 270.0f;

	KeyFrame[16].movAvion_x = 0.0f;
	KeyFrame[16].movAvion_y = 3.0f;
	KeyFrame[16].movAvion_z = -10.0f;
	KeyFrame[16].giroDragon = 10.0;
	KeyFrame[16].rotAlas = 7.0f;
	KeyFrame[16].giroDragon2 = 270.0;

	KeyFrame[17].movAvion_x = 0.0f;
	KeyFrame[17].movAvion_y = 0.0f;
	KeyFrame[17].movAvion_z = -5.0f;
	KeyFrame[17].giroDragon = 0.0f;
	KeyFrame[17].rotAlas = -7.0f;
	KeyFrame[17].giroDragon2 = 270.0f;

	KeyFrame[18].movAvion_x = 0.0f;
	KeyFrame[18].movAvion_y = -1.0f;
	KeyFrame[18].movAvion_z = 0.0f;
	KeyFrame[18].giroDragon = -10.0f;
	KeyFrame[18].rotAlas = 7.0f;
	KeyFrame[18].giroDragon2 = 270.0f;

	KeyFrame[19].movAvion_x = 0.0f;
	KeyFrame[19].movAvion_y = 0.0f;
	KeyFrame[19].movAvion_z = 5.0f;
	KeyFrame[19].giroDragon = 0.0f;
	KeyFrame[19].rotAlas = -7.0f;
	KeyFrame[19].giroDragon2 = 270.0f;

	//DATOS PARA EL RECORRIDO Squartle

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);


	//Variables para el juego giratorio
	GiratorioMov = 0.0f;
	MovimientoGira = 0.0f;
	MovY = 0.0;
	estado = 0;

	
	float giraRueda = 0.0;
	float GiratorioMov = 0.0f;
	
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;
		if (MarOn) {
			if (MarZ1 <= 32.172)
				MarZ1 += 0.10724;
			else {
				printf("El valor de Z1 es : %f", MarZ1);
			    MarZ1 = 0.10724f;
			}
		} 
		if (MarOn) {
			if (MarZ2 < 32.172)
				MarZ2 += 0.10724;
			else
				MarZ2 = 0.10724f;
		}
		if (MarOn) {
			if (MarZ3 < 32.172)
				MarZ3 += 0.10724;
			else
				MarZ3 = 0.10724f;
		}

		//giraRueda += movOffset * deltaTime;
		if (avanza)
		{
			if (movCoche < 1.0f)
			{
				movCoche += movOffset * deltaTime*.15f;

				GiratorioMov += movOffset * deltaTime * 20.0f;
				avanza = 1;
			}
			else
			{
				avanza = 0;
			}
		}
		else
		{
			if (movCoche > -1.0f)
			{
				movCoche -= movOffset * deltaTime*.15f;
			}
			else
			{
				avanza = 1;
			}
		}


		if (horario) {
			if (dia)
			{
				nocheCont++;
				//printf("cont=%d\n", nocheCont);
				if (nocheCont > 200)
				{
					spotLights[2].SetPos(glm::vec3(11, 40.0f, 100.0f - 0.25f)); //Permite colocar una luz en cualquier lugar, es verde
					spotLights[3].SetPos(glm::vec3(-15, 40.0f, 100.0f - 0.25f));
					spotLights[4].SetPos(glm::vec3(10.5, 20.0f, 50.0f - 0.25f));
					spotLights[1].SetPos(glm::vec3(-10.5, 20.0f, 50.0f - 0.25f));
					rtSkybox = "Textures/Skybox/sgod_noche3_ft.jpg";
					lfSkybox = "Textures/Skybox/sgod_noche3_bk.jpg";
					dnSkybox = "Textures/Skybox/sgod_noche3_dn.jpg";
					upSkybox = "Textures/Skybox/sgod_noche3_up.jpg";
					bkSkybox = "Textures/Skybox/sgod_noche3_rt.jpg";
					ftSkybox = "Textures/Skybox/sgod_noche3_lf.jpg";

					noche = true;
					nocheCont = 0;
					dia = false;

				}

			}
			if (noche)
			{
				diaCont++;
			//	printf("cont=%f\n", diaCont);
				if (diaCont > 200)
				{
					spotLights[2].SetPos(glm::vec3(0, -50.0f, 0.0f)); //Permite colocar una luz en cualquier lugar, es verde
					spotLights[3].SetPos(glm::vec3(0, -50.0f, 0.0f));
					spotLights[4].SetPos(glm::vec3(0, -50.0f, 0.0f));
					spotLights[1].SetPos(glm::vec3(0, -50.0f, 0.0f));

					rtSkybox = "Textures/Skybox/sgod_ft.tga";
					lfSkybox = "Textures/Skybox/sgod_bk.tga";
					dnSkybox = "Textures/Skybox/sgod_dn.tga";
					upSkybox = "Textures/Skybox/sgod_up.tga";
					bkSkybox = "Textures/Skybox/sgod_rt.tga";
					ftSkybox = "Textures/Skybox/sgod_lf.tga";

					dia = true;
					diaCont = 0;
					noche = false;
				}
			}

		}
		if (recorrido)
		{
			if (cont < 1227)
			{
				rX = movRecorrido();
				cont++;
				rY = movRecorrido();
				cont++;
				rZ = movRecorrido();
				cont++;
				camera.recorrido(glm::vec3(rX, rY, rZ), deltaTime);
				printf("%f, %f, %f, %d\n", rX, rY, rZ, cont);
			}
			else
			{
				cont = 0;
				printf("cont = %d\n", cont);

				recorrido = false;
			}
		}
		/*
		if (recorridoMont)
		{
			if (mov < 1227)
			{
				camera.recorrido(glm::vec3(movX() / escala + transx, movY() / escala + transy, -13.0f), deltaTime);
			}
			else
			{
				mov = 0;
				printf("cont = %d\n", mov);

				recorrido = false;
			}
		}
		*/
		if (recorrido)
		{
			if (cont < 1227)
			{
				rX = movRecorrido();
				cont++;
				rY = movRecorrido();
				cont++;
				rZ = movRecorrido();
				cont++;
				camera.recorrido(glm::vec3(rX, rY, rZ), deltaTime);
				printf("%f, %f, %f, %d\n", rX, rY, rZ, cont);
			}
			else
			{
				cont = 0;
				printf("cont = %d\n", cont);

				recorrido = false;
			}
		}


		std::vector<std::string> skyboxFaces;
		skyboxFaces.push_back(rtSkybox);
		skyboxFaces.push_back(lfSkybox);
		skyboxFaces.push_back(dnSkybox);
		skyboxFaces.push_back(upSkybox);
		skyboxFaces.push_back(bkSkybox);
		skyboxFaces.push_back(ftSkybox);

		skybox = Skybox(skyboxFaces);


		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();
		
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);

		

		// ----------------------------------------------------------------------
		// PISO
		// ----------------------------------------------------------------------

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//__________________________________________________________________________
		//________________ALBERCA________________________________________________
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.724f, 3.3f, 10.724f*2.0f));
		model = glm::scale(model, glm::vec3(5.5f, 2.0f, 4.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Alberca.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.3f, MarZ1));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.724f, 2.3f, MarZ1));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.724f*2.0f, 2.3f, MarZ1));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.3f, MarZ2));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.724f, 2.3f, MarZ2));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.724f*2.0f, 2.3f, MarZ2));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.3f, MarZ3));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.724f, 2.3f, MarZ3));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.724f*2.0f, 2.3f, MarZ3));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mar.RenderModel();

		//********************************************************************************//
		//JUEGO DE LAS SILLAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, 3.0f, -20.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		JuegoTexture.UseTexture();
		sillas.RenderModel();
		
		//Juego Giratorio

		//****************************************************************************************
		//Juego Giratorio con Primitivas//

		//Base del juego
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(12.0f, 0.4f, 12.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MetalPisoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[7]->RenderMesh();

		//Torre
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rojoMetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[7]->RenderMesh();

		//Base1
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 0.4f, 1.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rojoMetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[7]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 5.4f, 0.0f));
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		laminaCirculoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[7]->RenderMesh();

		//TorreTriangulos
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 6.5f, 0.0f));
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[8]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 7.5f, 0.0f));
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[8]->RenderMesh();

		//Brazos de juego
		//Brazo1
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 6.5f + MovY, 0.0f));
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[9]->RenderMesh();

		//Carrito 1
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(0.0f * toRadians) * 4, 4.0f + MovY, sin(0.0f*toRadians) * 4));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		circuitoAzulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Asiento 1
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(0.0f* toRadians) * 4, 4.0f + MovY, sin(0.0f* toRadians) * 4));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		//Brazo 2
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 6.5f + MovY, 0.0f));
		model = glm::rotate(model, (60 + GiratorioMov)* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[9]->RenderMesh();

		//Carrito 2
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(60.0f* toRadians) * 4, 4.0f + MovY, sin(60.0f* toRadians) * 4));
		model = glm::rotate(model, (60)* toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FloresTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Asiento 2
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(60.0f* toRadians) * 4, 4.0f + MovY, sin(60.0f* toRadians) * 4));
		model = glm::rotate(model, (60)* toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		//Brazo 3
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 6.5f + MovY, 0.0f));
		model = glm::rotate(model, (120 + GiratorioMov) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[9]->RenderMesh();

		//Carrito 3
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(120.0f* toRadians) * 4, 4.0f + MovY, sin(120.0f* toRadians) * 4));
		model = glm::rotate(model, (120) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Asiento 3
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(120.0f* toRadians) * 4, 4.0f + MovY, sin(120.0f* toRadians) * 4));
		model = glm::rotate(model, (120) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		//Brazo 4
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 6.5f + MovY, 0.0f));
		model = glm::rotate(model, (180 + GiratorioMov) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[9]->RenderMesh();

		//Carrito 4
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(180.0f* toRadians) * 4, 4.0f + MovY, sin(180.0f* toRadians) * 4));
		model = glm::rotate(model, (180) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CajaMetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Asiento 4
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(180.0f* toRadians) * 4, 4.0f + MovY, sin(180.0f* toRadians) * 4));
		model = glm::rotate(model, (180) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		//Brazo 5 
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 6.5f + MovY, 0.0f));
		model = glm::rotate(model, (240 + GiratorioMov) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[9]->RenderMesh();

		//Carrito 5
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(240.0f * toRadians) * 4, 4.0f + MovY, sin(240.0f*toRadians) * 4));
		model = glm::rotate(model, (240) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		coloresTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Asiento 5
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(240.0f*toRadians) * 4, 4.0f + MovY, sin(240.0f*toRadians) * 4));
		model = glm::rotate(model, (240) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		//Brazo 6
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::translate(model, glm::vec3(0.0f, 6.5f + MovY, 0.0f));
		model = glm::rotate(model, (300 + GiratorioMov) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.3f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarillo2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[9]->RenderMesh();

		//Carrito 6
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(300.0f*toRadians) * 4, 4.0f + MovY, sin(300.0f*toRadians) * 4));
		model = glm::rotate(model, (300) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RayasTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//Asiento 6
		model = glm::mat4(1.0);
		model = glm::translate(model, PosJuegoGira);
		model = glm::rotate(model, GiratorioMov * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::translate(model, glm::vec3(cos(300.0f*toRadians) * 4, 4.0f + MovY, sin(300.0f*toRadians) * 4));
		model = glm::rotate(model, (300) * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();
		
		//PRUEBA POLIGON
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshPoliList[0]->RenderMesh_Poli(0,24);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if(reproduciranimacion<1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
			//	printf("presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;
			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}	
		
	if (keys[GLFW_KEY_Q])
	{
		recorrido = true;
	}

	if (keys[GLFW_KEY_M])
	{
		recorridoMont = true;
	}
	if (keys[GLFW_KEY_B])
	{
		MarOn = true;
		int i = 0;
	}

}
