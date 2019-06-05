/*----------------------------------------------------------------------------------*/
/*---------------------------------  Proyecto Final --------------------------------*/
/*-----------------------------------    2019-2   ----------------------------------*/
/*------------- Computación Gráfica e Interacción Humano Computadora ---------------*/
/*----------------------------------------------------------------------------------*/
/*--------------------------- Balcázar Rodríguez Adan Alí --------------------------*/
/*------------------------------ Cuevas Salgado Carlos -----------------------------*/
/*------------------------ Díaz Gutiérrez Elvira Angela Belem ----------------------*/
/*---------------------------- Visual Studio Versión 2017 --------------------------*/
/*----------------------------------------------------------------------------------*/
/*---------------------------------- Instructivo: ----------------------------------*/
/*
Con las siguientes teclas se puede mover todo el modelo:
	D: Mueve el modelo a la izquierda
	A: Mueve el modelo a la derecha
	W: Acerca el modelo
	S: Aleja el modelo

Con las siguientes teclas se cambia el modo de iluminación ambiental:
	1: Modo Día
	2: Modo Atardecer
	3: Modo Noche

Con la siguiente teclas se activan algunas animaciones:
	P: Montaña Rusa
	O: Vuelo del Zapdos
	I: Carritos Chocones

Con el Mouse se puede mover la cámara desde la cual se observa el modelo
	Al mover el mouse a la derecha, la cámara gira a la derecha
	Al mover el mouse a la izquierda, la cámara gira a la izquierda
	Al mover el mouse hacia arriba, la cámara gira hacia arriba
	Al mover el mouse hacia abajo, la cámara gira hacia abajo
*/

// ------------------ Librerías ------------------

//#define STB_IMAGE_IMPLEMENTATION
#include <glew.h>
#include <glfw3.h>
#include <stb_image.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>

#include "esfera.h"
#include "camera.h"
#include "Model.h"

// Other Libs
#include "SOIL2/SOIL2.h"
using namespace std;

// Librería de audio --> irrKlang
#include <irrKlang.h>
using namespace irrklang;


// ------------------ Prototipos de Funciones Básicas ------------------

void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void myData(void);
void display(Shader, Model[]);
void getResolution(void);
void animate(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool);

// ------------------ Prototipos de Funciones de Renderizado ------------------

void montañaRusa(glm::mat4 model_loc, Shader lightingShader, Shader lampShader, Shader modelShader, Model logo);
void riel(glm::mat4 model_loc, float escala_x, float rotacion, Shader lightingShader);
void carro(glm::mat4 model_loc, Shader lightingShader);
void silla(glm::mat4 model_loc, int lado, int seguridad, Shader lightingShader);

void ruedaDeLaFortuna(glm::mat4 model_loc, Shader lightingShader, Shader lampShader);
void cupula(glm::mat4 model_loc, Shader lightingShader);
void anillo(glm::mat4 model_loc, Shader lightingShader);

Esfera esfera(1.0f); // Objeto para renderizar esferas


// ------------------ Variables y funciones para la cámara y la pantalla ------------------

// Variables para el tamaño de la ventana
int SCR_WIDTH = 3800;
int SCR_HEIGHT = 7600;

GLFWmonitor *monitors;
GLuint VBO, VAO, EBO;

// Variables para cambiar el color del fondo
GLclampf bgColorRed = 1.0;    //Color de fondo (Rojo)
GLclampf bgColorGreen = 1.0;  //Color de fondo (Verde)
GLclampf bgColorBlue = 1.0;   //Color de fondo (Azul)

// Cámara
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double	lastX = 0.0f,
		lastY = 0.0f;
bool firstMouse = true;

glm::vec3 posicionCamara;  //Posición actual de la cámara
glm::vec3 frenteCamara;    //Dirección de la cámara

// Timing
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;

	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;
}

// ------------------------- Variables para el sonido -------------------------

// Para cada sonido en paralelo que se desee reproducir se debe crear su propia variable

// Se inician los motores de Audio con parámetros por defecto
ISoundEngine* musicaDeFondo = createIrrKlangDevice();
ISoundEngine* efectoDeSonido = createIrrKlangDevice();

// Se reproduce la música de fondo. 
// ISound* es un holder para los sonidos traidos por un motor de audio. 
const char* cancionDeFondo = "Audio/MusicaRelajantePokemon.mp3";
//ISound* cancion = musicaDeFondo->play3D(cancionDeFondo, vec3df(0, 0, -1), true, false, true);



// ---------------- Variables y funciones para el renderizado del entorno ----------------

int enFeria = 1;           // Se renderiza la feria
int enCasaTerror = 0;      // Se renderiza lo que está adentro de la casa del terror
int enShowAcuatico = 0;    // Se renderiza lo que está en el show acuático

int FPresionado = 0;  //Varible de control para monitorear que la tecla de interacción se presiona una sola vez

int compruebaUbicacionCasaTerror()
{
	if( posicionCamara.x > 30.0f && posicionCamara.x < 40.0f )
	{
		if( posicionCamara.y > -11.54f && posicionCamara.y < 4.0f )
		{
			if( posicionCamara.z > -84.0f  && posicionCamara.z < -71.6f)
				return 1;  //La cámara está frente a la puerta de la casa del terror
		}
	}

	return 0; //La cámara no está frente a la puerta de la casa del terror
}

// ---- Dentro de la Casa del Terror ----

// Librero

int libreroDePie = 1;   // Variable para saber en qué momento cae el librero
int libreroSuena = 1;   // Variable para que se reproduzca el sonido del librero cayendo

void compruebaUbicacionLibrero()
{
	if (posicionCamara.x > -3741.88f && posicionCamara.x < -2095.60f)
	{
		if (posicionCamara.z > -12643.91f  && posicionCamara.z < -10518.48f)
			libreroDePie = 0; //Se llegó al punto en el que el librero se cae
	}
}

// Llave
int llaveTomada = 0;  //Originalmente, la llave no se encuentra en el inventario

int compruebaUbicacionLlave()
{
	if (posicionCamara.x > -3481.0f && posicionCamara.x < -2188.75f)
	{
		if (posicionCamara.z > -6663.0f  && posicionCamara.z < -5717.6f)
			return 1;  //La cámara está frente a la llave
	}

	return 0; //La cámara no está frente a la llave
}

// Salida

int compruebaUbicacionSalida()
{
	if (posicionCamara.x > -459.54f && posicionCamara.x < 514.94f)
	{
		if (posicionCamara.z > -15043.33f  && posicionCamara.z < -13543.55f)
			return 1;  //La cámara está frente a la llave
	}

	return 0; //La cámara no está frente a la llave
}

// Zombie

int apareceZombie = 0;

void compruebaUbicacionZombie()
{
	if (posicionCamara.x > -1738.0f && posicionCamara.x < -456.23f)
	{
		if (posicionCamara.z > -1849.33f  && posicionCamara.z < -91.23f)
			apareceZombie = 1;  //Estás demasiado cerca de la ubicación del zombie y éste aparece
	}
}

// Cuadros

int cuadrosSangrientos = 0;

void compruebaUbicacionCuadros()
{
	if (posicionCamara.x > -1623.74f && posicionCamara.x < 1341.96f)
	{
		if (posicionCamara.z > -12643.27f  && posicionCamara.z < -10621.49f)
			cuadrosSangrientos = 1;  //Estás demasiado cerca de la ubicación del zombie y éste aparece
	}
}

// Lámpara

int enciendeLampara = 0;  // La lámpara originalmente está apagada

void compruebaUbicacionLampara()
{
	if (posicionCamara.x > 1768.27 && posicionCamara.x < 2754.48f)
	{
		if (posicionCamara.z > -11450.27f  && posicionCamara.z < -10344.29f)
			enciendeLampara = 1;  //Estás demasiado cerca de la ubicación del zombie y éste aparece
	}
}

// Sirena

int ubicacionSirena = 0;  // Se ha llegado a la ubicación de la sirena
int suenaSirena = 1;      // Al pasar junto a la sirena, ésta grita sólo una vez

void compruebaUbicacionSirena()
{
	if (posicionCamara.x > 1768.27 && posicionCamara.x < 2754.48f)
	{
		if (posicionCamara.z > -11450.27f  && posicionCamara.z < -10344.29f)
			ubicacionSirena = 1;  //Se llegó hasta donde está la sirena
	}
	else
		ubicacionSirena = 0;  //No se está al lado de la sirena
}


// ------------------ Variables para la Animación ------------------

// -------- Montaña Rusa --------

float giroSilla = 0.0f;    //Ángulo de giro de las sillas de la montaña rusa
int giroSillaInversa = 0;  //Sentido de giro de las sillas de la montaña rusa

// Variables para los Keyframes
float posCarroX = 0.0f;
float posCarroY = 0.0f;
float posCarroZ = 0.0f;
float rotCarro = 0.0f;


// -------- Rueda de la Fortuna --------

float giroRueda = 0.0f;    //Ángulo de giro de la rueda


// -------- Carrusel --------

float giroCarrusel = 0.0f;    //Ángulo de giro del carrusel

int leonesInteriorSuben = 1;  //Variable de control para saber cuándo suben los leones

float posLeonesInteriorY = 0.0f;  //Movimiento en Y de los Leones Internos
float posLeonesExteriorY = 0.0f;  //Movimiento en Y de los Leones Externos

// -------- Carros Chocones --------

// Variables para los Keyframes 

// Carro Azul
float posCarroAzulX = 0.0f;
float posCarroAzulY = 0.0f;
float posCarroAzulZ = 0.0f;
float rotCarroAzul = 0.0f;

 // Carro Blanco
float posCarroBlancoX = 0.0f;
float posCarroBlancoY = 0.0f;
float posCarroBlancoZ = 0.0f;
float rotCarroBlanco = 0.0f;

  // Carro Rojo
float posCarroRojoX = 0.0f;
float posCarroRojoY = 0.0f;
float posCarroRojoZ = 0.0f;
float rotCarroRojo = 0.0f;


// -------- Casa del terror --------

int fantasmaMujerSube = 1;         //Variable de control para saber cuando sube la mujuer fantasma
float posMujerFantasmaY = 0.0f;    //Posición de la mujer fantasma

int fantasmaVuela = 1;        //Variable de control
float posFantasmaX = 0.0f;    //Posición  en X del fantasma
float posFantasmaZ = 0.0f;    //Posición  en Z del fantasma


// -------- Pokémones --------

// Variables para los Keyframes
float posZapdosX = 0.0f;
float posZapdosY = 0.0f;
float posZapdosZ = 0.0f;
float rotZapdos = 0.0f;


// --------> Variables y funciones para Animación por Keyframes 

#define MAX_FRAMES 200   //Número máximo de keyframes

int F1Presionado = 0;     //Variable para evitar flickering en el botón de guardado del carro (Tecla F1)
int F2Presionado = 0;     //Variable para evitar flickering en el botón de guardado del zapdos (Tecla F2)
int F3Presionado = 0;     //Variable para evitar flickering en el botón de guardado del carro azul (Tecla F3)
int F4Presionado = 0;     //Variable para evitar flickering en el botón de guardado del carro blanco (Tecla F4)
int F5Presionado = 0;     //Variable para evitar flickering en el botón de guardado del carro rojo (Tecla F5)

int PPresionado = 0;     //Variable para evitar flickering en el botón de play del carro (Tecla P)
int OPresionado = 0;     //Variable para evitar flickering en el botón de play del zapdos (Tecla O)
int IPresionado = 0;     //Variable para evitar flickering en el botón de play de los carros chocones (Tecla I)

typedef struct _frame    //Por cada variable de control se debe crear su variable auxiliar de incremento
{
	//Variables para GUARDAR Key Frames

	float posX;      //Posición en el eje X
	float posY;      //Posición en el eje Y
    float posZ;	     //Posición en el eje Z

	float posXInc;   //Variable de incremento para posX
	float posYInc;   //Variable de incremento para posY
    float posZInc;   //Variable de incremento para posZ

	float rot;       //Rotación del modelo
	float rotInc;    //Variable de incremento de rot

}FRAME;


// --- Variables para los keyframes del carro de la montaña rusa ---
FRAME keyFrameCarro[MAX_FRAMES];              //Se crea el objeto del tipo keyframe
int frameIndexCarro = 0;                      //Varible que cuenta el número de frames que se tienen en keyFrameCarro
int* frameIndexCarroPtr = &frameIndexCarro;   
int i_curr_steps_Carro = 0;                   // Contador para recorrer cada fotograma entre keyframes
int i_max_steps_Carro = 7;                    // Número de fotogramas entre keyframes

bool playCarro = false;   //Variable para dar inicio a la animación
int  playIndexCarro = 0;  //Variable para ir recorriendo cada keyFrame


// --- Variables para los keyframes del Zapdos ---
FRAME keyFrameZapdos[MAX_FRAMES];              //Se crea el objeto del tipo keyframe
int frameIndexZapdos = 0;                      //Varible que cuenta el número de frames que se tienen en keyFrameCarro
int* frameIndexZapdosPtr = &frameIndexZapdos;  
int i_curr_steps_Zapdos = 0;                    // Contador para recorrer cada fotograma entre keyframes
int i_max_steps_Zapdos = 12;                    // Número de fotogramas entre keyframes

bool playZapdos = false;   //Variable para dar inicio a la animación
int  playIndexZapdos = 0;  //Variable para ir recorriendo cada keyFrame


// --- Variables para los keyframes del Carro Azul ---
FRAME keyFrameCarroAzul[MAX_FRAMES];                 //Se crea el objeto del tipo keyframe
int frameIndexCarroAzul = 0;                         //Varible que cuenta el número de frames que se tienen en keyFrameCarro
int* frameIndexCarroAzulPtr = &frameIndexCarroAzul;
int i_curr_steps_CarroAzul = 0;                      // Contador para recorrer cada fotograma entre keyframes
int i_max_steps_CarroAzul = 12;                      // Número de fotogramas entre keyframes

bool playCarroAzul = false;   //Variable para dar inicio a la animación
int  playIndexCarroAzul = 0;  //Variable para ir recorriendo cada keyFrame


// --- Variables para los keyframes del Carro Blanco ---
FRAME keyFrameCarroBlanco[MAX_FRAMES];                  //Se crea el objeto del tipo keyframe
int frameIndexCarroBlanco = 0;                          //Varible que cuenta el número de frames que se tienen en keyFrameCarro
int* frameIndexCarroBlancoPtr = &frameIndexCarroBlanco;
int i_curr_steps_CarroBlanco = 0;                       // Contador para recorrer cada fotograma entre keyframes
int i_max_steps_CarroBlanco = 12;                       // Número de fotogramas entre keyframes

bool playCarroBlanco = false;   //Variable para dar inicio a la animación
int  playIndexCarroBlanco = 0;  //Variable para ir recorriendo cada keyFrame


// --- Variables para los keyframes del Carro Rojo ---
FRAME keyFrameCarroRojo[MAX_FRAMES];                 //Se crea el objeto del tipo keyframe
int frameIndexCarroRojo = 0;                         //Varible que cuenta el número de frames que se tienen en keyFrameCarro
int* frameIndexCarroRojoPtr = &frameIndexCarroRojo;
int i_curr_steps_CarroRojo = 0;                      // Contador para recorrer cada fotograma entre keyframes
int i_max_steps_CarroRojo = 12;                      // Número de fotogramas entre keyframes

bool playCarroRojo = false;   //Variable para dar inicio a la animación
int  playIndexCarroRojo = 0;  //Variable para ir recorriendo cada keyFrame


void saveFrame(FRAME KeyFrame[], int* FrameIndex, float posX, float posY, float posZ, float rot, string archivo)
{
	printf("FrameIndex = %d\n", FrameIndex);

	// ---- Se guardan los datos del keyframe ----

	KeyFrame[*FrameIndex].posX = posX;
	KeyFrame[*FrameIndex].posY = posY;
    KeyFrame[*FrameIndex].posZ = posZ;
	KeyFrame[*FrameIndex].rot  = rot;

	(*FrameIndex)++;   // Aumenta la cuenta global de KeyFrames

	// ---- Se guarda el KeyFrame en el archivo ----

	ofstream file(archivo, ios::app);    // Se abre el archivo
	file << posX << 'f' << ' ' << posY << 'f' << ' ' << posZ << 'f' << ' ' << rot << 'f' << endl;  // Se escriben los datos
	file.close();  // Se cierra el archivo
}

void resetElements(FRAME KeyFrame[], float posX, float posY, float posZ, float rot)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
    posZ = KeyFrame[0].posZ;

	rot = KeyFrame[0].rot;
}

void interpolation(FRAME KeyFrame[], int PlayIndex, int i_max_steps)
{
	KeyFrame[PlayIndex].posXInc = (KeyFrame[PlayIndex + 1].posX - KeyFrame[PlayIndex].posX) / i_max_steps;
	KeyFrame[PlayIndex].posYInc = (KeyFrame[PlayIndex + 1].posY - KeyFrame[PlayIndex].posY) / i_max_steps;
    KeyFrame[PlayIndex].posZInc = (KeyFrame[PlayIndex + 1].posZ - KeyFrame[PlayIndex].posZ) / i_max_steps;

	KeyFrame[PlayIndex].rotInc = (KeyFrame[PlayIndex + 1].rot - KeyFrame[PlayIndex].rot) / i_max_steps;
}

void LoadKeyFrames(FRAME KeyFrame[], int *FrameIndex, string archivo) 
{
	ifstream file(archivo);  // Se abre el archivo

	if (!file.is_open())     // Si ocurre un error, se notifica
		std::cout << "No se encontro el archivo: " << archivo << '\n';

	// Se limpian todos los KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;

		KeyFrame[i].posXInc = 0;
		KeyFrame[i].posYInc = 0;
		KeyFrame[i].posZInc = 0;

		KeyFrame[i].rot = 0;
		KeyFrame[i].rotInc = 0;
	}

	//Variable para ir llenando los datos de cada KeyFrame
	int frameActual = 0; 

	//Variables para leer los datos de cada línea del archivo
	string posX;         
	string posY;
	string posZ;
	string rotacion;

	while (file.good()) 
	{
		getline(file, posX, ' ');       //Se lee cada dato
		getline(file, posY, ' ');
		getline(file, posZ, ' ');
		getline(file, rotacion, '\n');

		if (file.good())
		{
			KeyFrame[frameActual].posX = stof(posX);      //Cast de string a float
			KeyFrame[frameActual].posY = stof(posY);
			KeyFrame[frameActual].posZ = stof(posZ);
			KeyFrame[frameActual].rot = stof(rotacion);

			frameActual++;  //Aumenta el contador

			(*FrameIndex)++;   //Aumenta la cuenta global de KayFrames
		}
	}

	file.close();  //Se cierra el archivo
}


// ------------------ Variables para la iluminación ------------------

// Coordenadas de Iluminación                    -- Luz tipo Posicional por Defecto --
glm::vec3 lightPosition(-20.0f, 115.0f, 30.0f);   //Posición de la Luz
glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);     //Dirección de la Luz

// Para controlar la intensidad de la luz
float opacidad = 32.0f;   //Variable de control

int CeroPresionado = 0;   //Variables para corroborar el estado de las teclas
int NuevePresionado = 0;

// Para controlar el color de la luz
float R = 1.0f;        //Variables de control
float G = 1.0f;
float B = 1.0f;

int RPresionado = 0;   //Variables para corroborar el estado de las teclas
int GPresionado = 0;
int BPresionado = 0;

//Para controlar la fuente de iluminación --> Direccional por defecto
char Light_VertxShader[100] = "shaders/shader_texture_light_dir.vs";
char Light_FragShader[100] = "shaders/shader_texture_light_dir.fs";

//La fuente de luz se encuentra en la ubicación del sol o foco
glm::vec3 Light_Position = lightPosition;

//La dirección de la luz es la que definimos arriba
glm::vec3 Light_Direction = lightDirection;

int modoNoche = 0;      //Variable para indicar cuando es de noche
int modoAtardecer = 0;  //Variable para indicar cuando es de tarde
int modoDia = 1;        //Variable para indicar cuando es de día

// --- Estas variables son para cuando se usa luz de tipo reflector (Linterna) ---
//  -> Esta define el tamaño del círculo de luz. Aumentar su valor para que el círculo sea más grande
float Light_CutOff = glm::cos(glm::radians(12.5f));

int Luz_Reflector = 0;  //Por defecto, la luz es de tipo Posicional, no de Reflector


// ------------------ Variables y funciones para Texturas ------------------

// --- Difusas ---

// Materiales
unsigned int t_metal;
unsigned int t_metal_amarillo;
unsigned int t_terracota;
unsigned int t_madera;
unsigned int t_madera_lisa;
unsigned int t_puerta;

// Colores
unsigned int t_negro;
unsigned int t_cafe;
unsigned int t_rojo;
unsigned int t_naranja;
unsigned int t_amarillo;
unsigned int t_verde;
unsigned int t_azul;
unsigned int t_azul_rey;
unsigned int t_morado;
unsigned int t_rosa;
unsigned int t_blanco;
unsigned int montRusa;
unsigned int plainTexture;
// --- Especulares ---

// Materiales
unsigned int t_metal_brillo;
unsigned int t_metal_amarillo_brillo;
unsigned int t_terracota_brillo;
unsigned int t_madera_brillo;
unsigned int t_madera_lisa_brillo;
unsigned int t_puerta_brillo;

// Colores
unsigned int t_negro_brillo;
unsigned int t_blanco_brillo;


// --- Skyboxes ---

// Skybox Día
unsigned int t_skybox_dia;

// Skybox Tarde
unsigned int t_skybox_tarde;

// Skybox Noche
unsigned int t_skybox_noche;


unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

//Valores a colocar si la extensión de la textura es:
// .jpg ----> 0
// .png ----> 1
void LoadTextures()
{
	// Texturas de Materiales
	t_metal = generateTextures("Texturas/Metal.png", 1);
	t_metal_brillo = generateTextures("Texturas/Metal_Especular.png", 1);

	t_metal_amarillo = generateTextures("Texturas/Metal_Amarillo.png", 1);
	t_metal_amarillo_brillo = generateTextures("Texturas/Metal_Amarillo_Especular.png", 1);

	t_terracota = generateTextures("Texturas/Terracota.png", 1);
	t_terracota_brillo = generateTextures("Texturas/Terracota_Especular.png", 1);

	t_madera = generateTextures("Texturas/Madera.png", 1);
	t_madera_brillo = generateTextures("Texturas/Madera_Especular.png", 1);

	t_madera_lisa = generateTextures("Texturas/MaderaLisa.png", 1);
	t_madera_lisa_brillo = generateTextures("Texturas/MaderaLisa_Especular.png", 1);

	t_puerta = generateTextures("Texturas/Puerta.png", 1);
	t_puerta_brillo = generateTextures("Texturas/Puerta_Especular.png", 1);

	// Texturas de Colores
	t_negro = generateTextures("Texturas/Negro.png", 1);
	t_cafe = generateTextures("Texturas/Cafe.png", 1);
	t_rojo = generateTextures("Texturas/Rojo.png", 1);
	t_naranja = generateTextures("Texturas/Naranja.png", 1);
	t_amarillo = generateTextures("Texturas/Amarillo.png", 1);
	t_verde = generateTextures("Texturas/Verde.png", 1);
	t_azul = generateTextures("Texturas/Azul.png", 1);
	t_azul_rey = generateTextures("Texturas/Azul_rey.png", 1);
	t_morado = generateTextures("Texturas/Morado.png", 1);
	t_rosa = generateTextures("Texturas/Rosa.png", 1);
	t_blanco = generateTextures("Texturas/Blanco.png", 1);

	t_negro_brillo = generateTextures("Texturas/Negro_Especular.png", 1);
	t_blanco_brillo = generateTextures("Texturas/Blanco_Especular.png", 1);
	plainTexture = generateTextures("Texturas/piso.jpg",0); 
	montRusa = generateTextures("Texturas/p_03_amarillo.jpg",0);

	// Texturas del Skybox (Día)
	t_skybox_dia = generateTextures("Texturas/skybox_tarde.png", 1);

	// Texturas del Skybox (Tarde)
	t_skybox_tarde = generateTextures("Texturas/skybox_tarde.png", 1);

	// Texturas del Skybox (Noche)
	t_skybox_noche = generateTextures("Texturas/skybox_noche.png", 1);


	// ------ Se enlazan las texturas con su respectiva unidad ------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, t_metal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, t_metal_brillo);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, t_metal_amarillo);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, t_metal_amarillo_brillo);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, t_terracota);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, t_terracota_brillo);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, t_madera);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, t_madera_brillo);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, t_madera_lisa);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, t_madera_lisa_brillo);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, t_puerta);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, t_puerta_brillo);

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, t_negro);
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, t_cafe);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, t_rojo);
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, t_naranja);
	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, t_amarillo);
	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_2D, t_verde);
	glActiveTexture(GL_TEXTURE19);
	glBindTexture(GL_TEXTURE_2D, t_azul);
	glActiveTexture(GL_TEXTURE20);
	glBindTexture(GL_TEXTURE_2D, t_azul_rey);
	glActiveTexture(GL_TEXTURE21);
	glBindTexture(GL_TEXTURE_2D, t_morado);
	glActiveTexture(GL_TEXTURE22);
	glBindTexture(GL_TEXTURE_2D, t_rosa);
	glActiveTexture(GL_TEXTURE23);
	glBindTexture(GL_TEXTURE_2D, t_blanco);

	glActiveTexture(GL_TEXTURE24);
	glBindTexture(GL_TEXTURE_2D, t_negro_brillo);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, t_blanco_brillo);

	// Skybox (Dia)
	glActiveTexture(GL_TEXTURE26);
	glBindTexture(GL_TEXTURE_2D, t_skybox_dia);

	// Skybox (Tarde)
	glActiveTexture(GL_TEXTURE27);
	glBindTexture(GL_TEXTURE_2D, t_skybox_tarde);
	
	// Skybox (Noche)
	glActiveTexture(GL_TEXTURE28);
	glBindTexture(GL_TEXTURE_2D, t_skybox_noche);

	// Skybox (Noche)
	glActiveTexture(GL_TEXTURE29);
	glBindTexture(GL_TEXTURE_2D, t_skybox_noche);

	glActiveTexture(GL_TEXTURE30);
	glBindTexture(GL_TEXTURE_2D, plainTexture);

	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, montRusa);

}

void myData()
{	
	float vertices[] = {

		// ------------ CUBO ------------
		// Vértices: 0-24

		// Posición           //Normales            // UV's

		//Cara de la izquierda
		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,    // top right
		-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    // bottom right
		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,    // bottom left
		-0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,    // top left 

		//Cara de la derecha
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,    // top right
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    // bottom right
		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,    // bottom left
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,    // top left

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


		// ------------ Cilindro ------------
		// Vértices: 24-96 (72 vértices)

		// Posición                // Normales            // UV's

		//Cara 1
		0.0f,   0.5f, 0.45f,       0.0f, 0.0f, 1.0f,      1.0f, 1.0f,    // top right
		0.0f,  -0.5f, 0.45f,       0.0f, 0.0f, 1.0f,      1.0f, 0.0f,    // bottom right
	   -0.25f, -0.5f, 0.38f,       0.0f, 0.0f, 1.0f,      0.0f, 0.0f,    // bottom left
	   -0.25f,  0.5f, 0.38f,       0.0f, 0.0f, 1.0f,      0.0f, 1.0f,    // top left 

		//Cara 2
	   -0.25f,  0.5f, 0.38f,      -1.0f, 0.0f, 1.0f,      1.0f, 1.0f,    // top right
	   -0.25f, -0.5f, 0.38f,      -1.0f, 0.0f, 1.0f,      1.0f, 0.0f,    // bottom right
	   -0.38f, -0.5f, 0.25f,      -1.0f, 0.0f, 1.0f,      0.0f, 0.0f,    // bottom left
	   -0.38f,  0.5f, 0.25f,      -1.0f, 0.0f, 1.0f,      0.0f, 1.0f,    // top left 

		//Cara 3
	   -0.38f,  0.5f,  0.25f,     -1.0f, 0.0f, 0.0f,      1.0f, 1.0f,    // top right
	   -0.38f, -0.5f,  0.25f,     -1.0f, 0.0f, 0.0f,      1.0f, 0.0f,    // bottom right
	   -0.45f, -0.5f,  0.0f,      -1.0f, 0.0f, 0.0f,      0.0f, 0.0f,    // bottom left
	   -0.45f,  0.5f,  0.0f,      -1.0f, 0.0f, 0.0f,      0.0f, 1.0f,    // top left 

		//Cara 4
	   -0.45f,  0.5f,  0.0f,      -1.0f, 0.0f, 0.0f,      1.0f, 1.0f,    // top right
	   -0.45f, -0.5f,  0.0f,      -1.0f, 0.0f, 0.0f,      1.0f, 0.0f,    // bottom right
	   -0.38f, -0.5f, -0.25f,     -1.0f, 0.0f, 0.0f,      0.0f, 0.0f,    // bottom left
	   -0.38f,  0.5f, -0.25f,     -1.0f, 0.0f, 0.0f,      0.0f, 1.0f,    // top left 

		//Cara 5
	   -0.38f,  0.5f, -0.25f,     -1.0f, 0.0f, -1.0f,     1.0f, 1.0f,    // top right
	   -0.38f, -0.5f, -0.25f,     -1.0f, 0.0f, -1.0f,     1.0f, 0.0f,    // bottom right
	   -0.25f, -0.5f, -0.38f,     -1.0f, 0.0f, -1.0f,     0.0f, 0.0f,    // bottom left
	   -0.25f,  0.5f, -0.38f,     -1.0f, 0.0f, -1.0f,     0.0f, 1.0f,    // top left

		//Cara 6
	   -0.25f,  0.5f, -0.38f,      0.0f, 0.0f, -1.0f,     1.0f, 1.0f,    // top right
	   -0.25f, -0.5f, -0.38f,      0.0f, 0.0f, -1.0f,     1.0f, 0.0f,    // bottom right
		0.0f,  -0.5f, -0.45f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f,    // bottom left
		0.0f,   0.5f, -0.45f,      0.0f, 0.0f, -1.0f,     0.0f, 1.0f,    // top left

		//Cara 7
		0.0f,   0.5f, -0.45f,      0.0f, 0.0f, -1.0f,     1.0f, 1.0f,    // top right
		0.0f,  -0.5f, -0.45f,      0.0f, 0.0f, -1.0f,     1.0f, 0.0f,    // bottom right
		0.25f, -0.5f, -0.38f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f,    // bottom left
		0.25f,  0.5f, -0.38f,      0.0f, 0.0f, -1.0f,     0.0f, 1.0f,    // top left

		//Cara 8
		0.25f,  0.5f, -0.38f,      1.0f, 0.0f, -1.0f,     1.0f, 1.0f,    // top right
		0.25f, -0.5f, -0.38f,      1.0f, 0.0f, -1.0f,     1.0f, 0.0f ,   // bottom right
		0.38f, -0.5f, -0.25f,      1.0f, 0.0f, -1.0f,     0.0f, 0.0f,    // bottom left
		0.38f,  0.5f, -0.25f,      1.0f, 0.0f, -1.0f,     0.0f, 1.0f,    // top left

		//Cara 9
		0.38f,  0.5f, -0.25,       1.0f, 0.0f, 0.0f,      1.0f, 1.0f,    // top right
		0.38f, -0.5f, -0.25,       1.0f, 0.0f, 0.0f,      1.0f, 0.0f ,   // bottom right
		0.45f, -0.5f, 0.0f,        1.0f, 0.0f, 0.0f,      0.0f, 0.0f,    // bottom left
		0.45f,  0.5f, 0.0f,        1.0f, 0.0f, 0.0f,      0.0f, 1.0f,    // top left

		//Cara 10
		0.45f,  0.5f, 0.0f,        1.0f, 0.0f, 0.0f,      1.0f, 1.0f,    // top right
		0.45f, -0.5f, 0.0f,        1.0f, 0.0f, 0.0f,      1.0f, 0.0f ,   // bottom right
		0.38f, -0.5f, 0.25f,       1.0f, 0.0f, 0.0f,      0.0f, 0.0f,    // bottom left
		0.38f,  0.5f, 0.25f,       1.0f, 0.0f, 0.0f,      0.0f, 1.0f,    // top left

		//Cara 11
		0.38f,  0.5f, 0.25f,       1.0f, 0.0f, 1.0f,      1.0f, 1.0f,    // top right
		0.38f, -0.5f, 0.25f,       1.0f, 0.0f, 1.0f,      1.0f, 0.0f ,   // bottom right
		0.25f, -0.5f, 0.38f,       1.0f, 0.0f, 1.0f,      0.0f, 0.0f,    // bottom left
		0.25f,  0.5f, 0.38f,       1.0f, 0.0f, 1.0f,      0.0f, 1.0f,    // top left

		//Cara 12
		0.25f,  0.5f, 0.38f,       0.0f, 0.0f, 1.0f,      1.0f, 1.0f,    // top right
		0.25f, -0.5f, 0.38f,       0.0f, 0.0f, 1.0f,      1.0f, 0.0f ,   // bottom right
		0.0f,  -0.5f, 0.45f,       0.0f, 0.0f, 1.0f,      0.0f, 0.0f,    // bottom left
		0.0f,   0.5f, 0.45f,       0.0f, 0.0f, 1.0f,      0.0f, 1.0f,    // top left

		//Cara de arriba
		 0.0f,   0.5f,  0.45f,     0.0f, 1.0f, 0.0f,      0.5f,  0.0f,   // 1
		-0.25f,  0.5f,  0.38f,     0.0f, 1.0f, 0.0f,      0.25f, 0.12f,  // 2
		-0.38f,  0.5f,  0.25f,     0.0f, 1.0f, 0.0f,      0.12f, 0.25f,  // 3
		-0.45f,  0.5f,  0.0f,      0.0f, 1.0f, 0.0f,      0.0f,  0.5f,   // 4 
		-0.38f,  0.5f, -0.25f,     0.0f, 1.0f, 0.0f,      0.12f, 0.75f,  // 5
		-0.25f,  0.5f, -0.38f,     0.0f, 1.0f, 0.0f,      0.25f, 0.88f,  // 6
		 0.0f,   0.5f, -0.45f,     0.0f, 1.0f, 0.0f,      0.5f,  1.0f,   // 7 
		 0.25f,  0.5f, -0.38f,     0.0f, 1.0f, 0.0f,      0.75f, 0.88f,  // 8 
		 0.38f,  0.5f, -0.25f,     0.0f, 1.0f, 0.0f,      0.88f, 0.75f,  // 9 
		 0.45f,  0.5f,  0.0f,      0.0f, 1.0f, 0.0f,      1.0f,  0.5f,   // 10 
		 0.38f,  0.5f,  0.25f,     0.0f, 1.0f, 0.0f,      0.88f, 0.25f,  // 11
		 0.25f,  0.5f,  0.38f,     0.0f, 1.0f, 0.0f,      0.75f, 0.12f,  // 12

		//Cara de abajo
		 0.0f,   -0.5f,  0.45f,    0.0f, -1.0f, 0.0f,     0.5f,  0.0f,   // 1
		-0.25f,  -0.5f,  0.38f,    0.0f, -1.0f, 0.0f,     0.25f, 0.12f,  // 2
		-0.38f,  -0.5f,  0.25f,    0.0f, -1.0f, 0.0f,     0.12f, 0.25f,  // 3
		-0.45f,  -0.5f,  0.0f,     0.0f, -1.0f, 0.0f,     0.0f,  0.5f,   // 4 
		-0.38f,  -0.5f, -0.25f,    0.0f, -1.0f, 0.0f,     0.12f, 0.75f,  // 5
		-0.25f,  -0.5f, -0.38f,    0.0f, -1.0f, 0.0f,     0.25f, 0.88f,  // 6
		 0.0f,   -0.5f, -0.45f,    0.0f, -1.0f, 0.0f,     0.5f,  1.0f,   // 7 
		 0.25f,  -0.5f, -0.38f,    0.0f, -1.0f, 0.0f,     0.75f, 0.88f,  // 8 
		 0.38f,  -0.5f, -0.25f,    0.0f, -1.0f, 0.0f,     0.88f, 0.75f,  // 9 
		 0.45f,  -0.5f,  0.0f,     0.0f, -1.0f, 0.0f,     1.0f,  0.5f,   // 10 
		 0.38f,  -0.5f,  0.25f,    0.0f, -1.0f, 0.0f,     0.88f, 0.25f,  // 11
		 0.25f,  -0.5f,  0.38f,    0.0f, -1.0f, 0.0f,     0.75f, 0.12f,  // 12


		// ------------ SILLA ------------
		// Vértices: 96-156 (60 vértices) Tubo: 96-136 Costado Der: 136-146  Costado Izq: 146-156

		// Posición             // Normales           // UV's

		//Cara 1
		 0.5f,  0.25f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,     // top right
		 0.5f, -0.3f,  -0.4f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,     // bottom right
		-0.5f, -0.3f,  -0.4f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,     // bottom left
		-0.5f,  0.25f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,     // top left 

		//Cara 2
		 0.5f,  0.45f, -0.4f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,     // top right
		 0.5f,  0.25f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,     // bottom right
		-0.5f,  0.25f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,     // bottom left
		-0.5f,  0.45f, -0.4f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,     // top left 

		//Cara 3
		 0.5f,  0.45f, -0.35f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      // top right
	     0.5f,  0.45f, -0.4f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      // bottom right
	    -0.5f,  0.45f, -0.4f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,      // bottom left
		-0.5f,  0.45f, -0.35f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,      // top left 

		//Cara 4
		 0.5f, -0.2f,  -0.3f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,      // top right
	     0.5f,  0.45f, -0.35f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,      // bottom right
	    -0.5f,  0.45f, -0.35f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,      // bottom left
		-0.5f, -0.2f,  -0.3f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,      // top left 

		//Cara 5
		 0.5f, -0.25f, -0.1f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      // top right
	     0.5f, -0.2f,  -0.3f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      // bottom right
	    -0.5f, -0.2f,  -0.3f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,      // bottom left
		-0.5f, -0.25f, -0.1f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,      // top left 

		//Cara 6
		 0.5f, -0.2f,   0.3f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,      // top right
	     0.5f, -0.25f, -0.1f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,      // bottom right
	    -0.5f, -0.25f, -0.1f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,      // bottom left
		-0.5f, -0.2f,   0.3f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,      // top left 

		//Cara 7
		 0.5f, -0.25f,  0.3f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,      // top right
	     0.5f, -0.2f,   0.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,      // bottom right
	    -0.5f, -0.2f,   0.3f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,      // bottom left
		-0.5f, -0.25f,  0.3f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,      // top left 

		//Cara 8
		 0.5f, -0.4f,   0.2f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,      // top right
	     0.5f, -0.25f,  0.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,      // bottom right
	    -0.5f, -0.25f,  0.3f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,      // bottom left
		-0.5f, -0.4f,   0.2f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,      // top left 

		//Cara 9
		 0.5f, -0.5f,  -0.2f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,     // top right
	     0.5f, -0.4f,   0.2f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,     // bottom right
	    -0.5f, -0.4f,   0.2f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,     // bottom left
		-0.5f, -0.5f,  -0.2f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,     // top left 

		//Cara 10
		 0.5f, -0.3f, -0.4f,    0.0f, -1.0f, 0.0f,    1.0f, 1.0f,    // top right
		 0.5f, -0.5f, -0.2f,    0.0f, -1.0f, 0.0f,    1.0f, 0.0f,    // bottom right
		-0.5f, -0.5f, -0.2f,    0.0f, -1.0f, 0.0f,    0.0f, 0.0f,    // bottom left
		-0.5f, -0.3f, -0.4f,    0.0f, -1.0f, 0.0f,    0.0f, 1.0f,    // top left 

		//Cara de la derecha
		 0.5f, -0.3f,  -0.4f,   1.0f, 0.0f, 0.0f,    0.9f,  0.1f,    // 1
		 0.5f,  0.25f, -0.5f,   1.0f, 0.0f, 0.0f,    1.0f,  0.75f,   // 2
		 0.5f,  0.45f, -0.4f,   1.0f, 0.0f, 0.0f,    0.9f,  0.95f,   // 3
		 0.5f,  0.45f, -0.35f,  1.0f, 0.0f, 0.0f,    0.85f, 0.95f,   // 4
		 0.5f, -0.2f,  -0.3f,   1.0f, 0.0f, 0.0f,    0.8f,  0.3f,    // 5
		 0.5f, -0.25f, -0.1f,   1.0f, 0.0f, 0.0f,    0.6f,  0.25f,   // 6
		 0.5f, -0.2f,   0.3f,   1.0f, 0.0f, 0.0f,    0.15f, 0.3f,    // 7
		 0.5f, -0.25f,  0.3f,   1.0f, 0.0f, 0.0f,    0.15f, 0.25f,   // 8
		 0.5f, -0.4f,   0.2f,   1.0f, 0.0f, 0.0f,    0.3f,  0.1f,    // 9
		 0.5f, -0.5f,  -0.2f,   1.0f, 0.0f, 0.0f,    0.7f,  0.0f,    // 10

		 //Cara de la izquierda
		-0.5f, -0.3f,  -0.4f,  -1.0f, 0.0f, 0.0f,    0.9f,  0.1f,    // 1
		-0.5f,  0.25f, -0.5f,  -1.0f, 0.0f, 0.0f,    1.0f,  0.75f,   // 2
		-0.5f,  0.45f, -0.4f,  -1.0f, 0.0f, 0.0f,    0.9f,  0.95f,   // 3
		-0.5f,  0.45f, -0.35f, -1.0f, 0.0f, 0.0f,    0.85f, 0.95f,   // 4
		-0.5f, -0.2f,  -0.3f,  -1.0f, 0.0f, 0.0f,    0.8f,  0.3f,    // 5
		-0.5f, -0.25f, -0.1f,  -1.0f, 0.0f, 0.0f,    0.6f,  0.25f,   // 6
		-0.5f, -0.2f,   0.3f,  -1.0f, 0.0f, 0.0f,    0.15f, 0.3f,    // 7
		-0.5f, -0.25f,  0.3f,  -1.0f, 0.0f, 0.0f,    0.15f, 0.25f,   // 8
		-0.5f, -0.4f,   0.2f,  -1.0f, 0.0f, 0.0f,    0.3f,  0.1f,    // 9
		-0.5f, -0.5f,  -0.2f,  -1.0f, 0.0f, 0.0f,    0.7f,  0.0f,    // 10


		// ------------ GUARDA ------------
		// Vértices: 156-186 (30 vértices) Tubo: 156-176   Costado Der: 176-181  Costado Izq: 181-186

		// Posición              // Normales          // UV's

		//Cara 1
		 0.5f,  0.25f, -0.45f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,    // top right
		 0.5f, -0.3f,  -0.35f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,    // bottom right
		-0.5f, -0.3f,  -0.35f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,    // bottom left
		-0.5f,  0.25f, -0.45f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,    // top left 

		//Cara 2
		 0.5f,  0.45f, -0.15f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,    // top right
		 0.5f,  0.25f, -0.45f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,    // bottom right
		-0.5f,  0.25f, -0.45f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,    // bottom left
		-0.5f,  0.45f, -0.15f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,    // top left 

		//Cara 3
		 0.5f, -0.35f,  0.5f,    0.0f, 1.0f, -1.0f,   1.0f, 1.0f,    // top right
	     0.5f,  0.45f, -0.15f,   0.0f, 1.0f, -1.0f,   1.0f, 0.0f,    // bottom right
	    -0.5f,  0.45f, -0.15f,   0.0f, 1.0f, -1.0f,   0.0f, 0.0f,    // bottom left
		-0.5f, -0.35f,  0.5f,    0.0f, 1.0f, -1.0f,   0.0f, 1.0f,    // top left 

		//Cara 4
		 0.5f, -0.5f,   0.05f,   0.0f, 1.0f, 1.0f,    1.0f, 1.0f,    // top right
	     0.5f, -0.35f,  0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f,    // bottom right
	    -0.5f, -0.35f,  0.5f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f,    // bottom left
		-0.5f, -0.5f,   0.05f,   0.0f, 1.0f, 1.0f,    0.0f, 1.0f,    // top left 

		//Cara 5
		 0.5f, -0.3f,  -0.35f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,    // top right
	     0.5f, -0.5f,   0.05f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,    // bottom right
	    -0.5f, -0.5f,   0.05f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,    // bottom left
		-0.5f, -0.3f,  -0.35f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,    // top left 

		//Cara de la derecha
		 0.5f, -0.3f,  -0.35f,   1.0f, 0.0f, 0.0f,    0.85f, 0.2f,    // 1
		 0.5f,  0.25f, -0.45f,   1.0f, 0.0f, 0.0f,    0.95f, 0.75f,   // 2
		 0.5f,  0.45f, -0.15f,   1.0f, 0.0f, 0.0f,    0.65f, 0.95f,   // 3
		 0.5f, -0.35f,  0.5f,    1.0f, 0.0f, 0.0f,    0.05f, 0.15f,   // 4
		 0.5f, -0.5f,   0.05f,   1.0f, 0.0f, 0.0f,    0.45f,  0.0f,   // 5

		 //Cara de la izquierda
		-0.5f, -0.3f,  -0.35f,  -1.0f, 0.0f, 0.0f,    0.85f, 0.2f,    // 1
		-0.5f,  0.25f, -0.45f,  -1.0f, 0.0f, 0.0f,    0.95f, 0.75f,   // 2
		-0.5f,  0.45f, -0.15f,  -1.0f, 0.0f, 0.0f,    0.65f, 0.95f,   // 3
		-0.5f, -0.35f,  0.5f,   -1.0f, 0.0f, 0.0f,    0.05f, 0.15f,   // 4
		-0.5f, -0.5f,   0.05f,  -1.0f, 0.0f, 0.0f,    0.45f,  0.0f,   // 5


		// ------------ SKYBOX (Cubo sin Normales y Con las 6 caras en una misma textura) ------------
		// Vértices: 186-210 (24 vértices)

		// Posición            //Normales           // UV's

		//Cara de la izquierda
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.3334f, 0.5f,   // top right
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.3334f, 0.0f,   // bottom right
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      // bottom left
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.0f, 0.5f,      // top left 

		//Cara de la derecha
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.6667f, 0.5f,   // top right
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.6667f, 0.0f,   // bottom right
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.3334f, 0.0f,   // bottom left
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.3334f, 0.5f,   // top left 

		//Cara de arriba
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      // top right
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.6667f, 0.0f,   // bottom right
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.6667f, 0.5f,   // bottom left
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    1.0f, 0.5f,      // top left 

		//Cara de abajo
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.3334f, 1.0f,   // top right
	     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.3334f, 0.5f,   // bottom right
	    -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.0f, 0.5f,      // bottom left
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      // top left 

		//Cara de enfrente
		 0.5f,  0.5f, 0.5f,    1.0f, 1.0f, 1.0f,    0.6667f, 1.0f,   // top right
		 0.5f, -0.5f, 0.5f,    1.0f, 1.0f, 1.0f,    0.6667f, 0.5f,   // bottom right
		-0.5f, -0.5f, 0.5f,    1.0f, 1.0f, 1.0f,    0.3334f, 0.5f,   // bottom left
		-0.5f,  0.5f, 0.5f,    1.0f, 1.0f, 1.0f,    0.3334f, 1.0f,   // top left 

		//Cara de atrás
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      // top right
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    1.0f, 0.5f,      // bottom right
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.6667f, 0.5f,   // bottom left
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,    0.6667f, 1.0f,   // top left 


		// ------------ Casa Terror (Interno) ------------
		// Vértices: 210-234  (24 vértices)

		// Posición           //Normales            // UV's

		//Cara de la izquierda
		-0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 2.0f,    // top right
		-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    // bottom right
		-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,    // bottom left
		-0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 2.0f,    // top left 

		//Cara de la derecha
		 0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,    1.0f, 2.0f,    // top right
		 0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    // bottom right
		 0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,    // bottom left
		 0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,    0.0f, 2.0f,    // top left

		//Cara de arriba
		 0.5f,  0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 2.0f,    // top right
		 0.5f,  0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,    // bottom right
		-0.5f,  0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,    // bottom left
		-0.5f,  0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 2.0f,    // top left 

		//Cara de abajo
		 0.5f, -0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 2.0f,    // top right
	     0.5f, -0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,    // bottom right
	    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,    // bottom left
		-0.5f, -0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 2.0f,    // top left 

		//Cara de enfrente
		 0.5f,  0.5f, 0.5f,    0.0f, 0.0f, -1.0f,   1.0f, 2.0f,    // top right
		 0.5f, -0.5f, 0.5f,    0.0f, 0.0f, -1.0f,   1.0f, 0.0f,    // bottom right
		-0.5f, -0.5f, 0.5f,    0.0f, 0.0f, -1.0f,   0.0f, 0.0f,    // bottom left
		-0.5f,  0.5f, 0.5f,    0.0f, 0.0f, -1.0f,   0.0f, 2.0f,    // top left 

		//Cara de atrás
		-0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  1.0f,   1.0f, 2.0f,    // top right
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  1.0f,   1.0f, 0.0f,    // bottom right
		 0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  1.0f,   0.0f, 0.0f,    // bottom left
		 0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  1.0f,   0.0f, 2.0f,    // top left

	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void animate(void)
{
	// --------------- Animación de la Montaña Rusa ---------------

	if (playCarro)
	{
		// Animación de las sillas 
		if (giroSillaInversa)  //Giro en sentido horario
		{
			giroSilla -= 7.5f;

			if (giroSilla <= 0.0f)
				giroSillaInversa = 0;
		}
		else  //Giro en sentido antihorario
		{
			giroSilla += 7.5f;

			if (giroSilla >= 1080.0f)  //3 vueltas: 360 * 3 = 1080
				giroSillaInversa = 1;
		}

		// Animación del carro
		if (i_curr_steps_Carro >= i_max_steps_Carro) //¿Fin de la animación entre cuadros (frames)?
		{
			playIndexCarro++;
			if (playIndexCarro > frameIndexCarro - 2)	//¿Fin de toda la animación?
			{
				printf("Termina la animacion\n");
				playIndexCarro = 0;
				playCarro = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps_Carro = 0; //Se resetea el contador
				interpolation(keyFrameCarro, playIndexCarro, i_max_steps_Carro);  //Interpolación
			}
		}
		else
		{
			// Se dibuja la animación
			posCarroX += keyFrameCarro[playIndexCarro].posXInc;
			posCarroY += keyFrameCarro[playIndexCarro].posYInc;
		    posCarroZ += keyFrameCarro[playIndexCarro].posZInc;

			rotCarro += keyFrameCarro[playIndexCarro].rotInc;

			i_curr_steps_Carro++;
		}

		//printf("playIndexCarro = %d\n", playIndexCarro);
	}


	// --------------- Animación de la Rueda de la Fortuna ---------------

	giroRueda += 1.0f;  //Giro en sentido antihorario


	// --------------- Animación del Carrusel ---------------

	giroCarrusel -= 1.0f;  //Giro en sentido antihorario

	// Movimiento de los Leones Interiores
	if (leonesInteriorSuben)
	{
		posLeonesInteriorY += 0.02f;

		if (posLeonesInteriorY >= 1.0f)
			leonesInteriorSuben = 0;
	}
	else
	{
		posLeonesInteriorY -= 0.02f;

		if (posLeonesInteriorY <= 0.0f)
			leonesInteriorSuben = 1;
	}

	// Movimiento de los Leones Exteriores
	if (leonesInteriorSuben)   //Si los leones interiores suben, los exteriores bajan
		posLeonesExteriorY -= 0.02f;
	else
		posLeonesExteriorY += 0.02f;


	// --------------- Animación de la Casa del Terror --------------- 

	if (fantasmaMujerSube)
	{
		posMujerFantasmaY += 0.02f;

		if (posMujerFantasmaY >= 2.0f)
			fantasmaMujerSube = 0;
	}
	else
	{
		posMujerFantasmaY -= 0.02f;

		if (posMujerFantasmaY <= 0.0f)
			fantasmaMujerSube = 1;
	}

	if (fantasmaVuela)
	{
		posFantasmaX -= 0.1f;
		posFantasmaZ += 0.1f;

		if (posFantasmaZ >= 20.0f)
			fantasmaVuela = 0;
	}
	else
	{
		posFantasmaX += 0.1f;
		posFantasmaZ -= 0.1f;

		if (posFantasmaZ <= 0.0f)
			fantasmaVuela = 1;
	}


	// --------------- Animación del Zapdos ---------------

	if (playZapdos)
	{
		if (i_curr_steps_Zapdos >= i_max_steps_Zapdos) //¿Fin de la animación entre cuadros (frames)?
		{
			playIndexZapdos++;
			if (playIndexZapdos > frameIndexZapdos- 2)	//¿Fin de toda la animación?
			{
				printf("Termina la animacion\n");
				playIndexZapdos = 0;
				playZapdos = false;

				// Se vuelve a poner el volumen de la música de fondo como estaba
				musicaDeFondo->setSoundVolume(0.25);
			}
			else //Next frame interpolations
			{
				i_curr_steps_Zapdos = 0; //Se resetea el contador
				interpolation(keyFrameZapdos, playIndexZapdos, i_max_steps_Zapdos);  //Interpolación
			}
		}
		else
		{
			// Se dibuja la animación
			posZapdosX += keyFrameZapdos[playIndexZapdos].posXInc;
			posZapdosY += keyFrameZapdos[playIndexZapdos].posYInc;
			posZapdosZ += keyFrameZapdos[playIndexZapdos].posZInc;

			rotZapdos += keyFrameZapdos[playIndexZapdos].rotInc;

			i_curr_steps_Zapdos++;
		}
		//printf("playIndexZapdos = %d\n", playIndexZapdos);
	}


	// --------------- Animación del Carro Azul (Carrito Chocón) ---------------

	if (playCarroAzul)
	{
		if (i_curr_steps_CarroAzul >= i_max_steps_CarroAzul)   //¿Fin de la animación entre cuadros (frames)?
		{
			playIndexCarroAzul++;
			if (playIndexCarroAzul > frameIndexCarroAzul - 2)  //¿Fin de toda la animación?
			{
				printf("Termina la animacion\n");
				playIndexCarroAzul = 0;
				playCarroAzul = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps_CarroAzul = 0;   //Se resetea el contador
				interpolation(keyFrameCarroAzul, playIndexCarroAzul, i_max_steps_CarroAzul);  //Interpolación
			}
		}
		else
		{
			// Se dibuja la animación
			posCarroAzulX += keyFrameCarroAzul[playIndexCarroAzul].posXInc;
			posCarroAzulY += keyFrameCarroAzul[playIndexCarroAzul].posYInc;
			posCarroAzulZ += keyFrameCarroAzul[playIndexCarroAzul].posZInc;

			rotCarroAzul += keyFrameCarroAzul[playIndexCarroAzul].rotInc;

			i_curr_steps_CarroAzul++;
		}
		//printf("playIndexCarroAzul = %d\n", playIndexCarroAzul);
	}


	// --------------- Animación del Carro Blanco (Carrito Chocón) ---------------

	if (playCarroBlanco)
	{
		if (i_curr_steps_CarroBlanco >= i_max_steps_CarroBlanco)   //¿Fin de la animación entre cuadros (frames)?
		{
			playIndexCarroBlanco++;
			if (playIndexCarroBlanco > frameIndexCarroBlanco - 2)  //¿Fin de toda la animación?
			{
				printf("Termina la animacion\n");
				playIndexCarroBlanco = 0;
				playCarroBlanco = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps_CarroBlanco = 0;   //Se resetea el contador
				interpolation(keyFrameCarroBlanco, playIndexCarroBlanco, i_max_steps_CarroBlanco);  //Interpolación
			}
		}
		else
		{
			// Se dibuja la animación
			posCarroBlancoX += keyFrameCarroBlanco[playIndexCarroBlanco].posXInc;
			posCarroBlancoY += keyFrameCarroBlanco[playIndexCarroBlanco].posYInc;
			posCarroBlancoZ += keyFrameCarroBlanco[playIndexCarroBlanco].posZInc;

			rotCarroBlanco += keyFrameCarroBlanco[playIndexCarroBlanco].rotInc;

			i_curr_steps_CarroBlanco++;
		}
		//printf("playIndexCarroBlanco = %d\n", playIndexCarroBlanco);
	}


	// --------------- Animación del Carro Rojo (Carrito Chocón) ---------------

	if (playCarroRojo)
	{
		if (i_curr_steps_CarroRojo >= i_max_steps_CarroRojo)   //¿Fin de la animación entre cuadros (frames)?
		{
			playIndexCarroRojo++;
			if (playIndexCarroRojo > frameIndexCarroRojo - 2)  //¿Fin de toda la animación?
			{
				printf("Termina la animacion\n");
				playIndexCarroRojo = 0;
				playCarroRojo = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps_CarroRojo = 0;   //Se resetea el contador
				interpolation(keyFrameCarroRojo, playIndexCarroRojo, i_max_steps_CarroRojo);  //Interpolación
			}
		}
		else
		{
			// Se dibuja la animación
			posCarroRojoX += keyFrameCarroRojo[playIndexCarroRojo].posXInc;
			posCarroRojoY += keyFrameCarroRojo[playIndexCarroRojo].posYInc;
			posCarroRojoZ += keyFrameCarroRojo[playIndexCarroRojo].posZInc;

			rotCarroRojo += keyFrameCarroRojo[playIndexCarroRojo].rotInc;

			i_curr_steps_CarroRojo++;
		}
		//printf("playIndexCarroRojo = %d\n", playIndexCarroRojo);
	}
}

void display(Shader modelShader, Model modelo[])
{
	//Matriz de modelo, vista t proyección
	glm::mat4 model = glm::mat4(1.0f);		//Initialize Matrix, Use this matrix for individual models
	glm::mat4 model_temp = glm::mat4(1.0f); //Maztriz auxiliar para la ubicación de cada elemento del modelo
	glm::mat4 model_loc = glm::mat4(1.0f);  //Matriz axuliar para la ubicación absoluta de todo el modelo

	glm::mat4 view = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection

	//Use "projection" to include Camera
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20000.0f);
	view = camera.GetViewMatrix();

	//Se habilita el shader de modelos
	modelShader.use();

	// Se pasan las matrices al shader de modelos
	modelShader.setMat4("model", model);
	modelShader.setMat4("view", view);
	modelShader.setMat4("projection", projection);

	//Se abre el shader de focos de luz y se habilita
	Shader lampShader("shaders/shader_lamp.vs", "shaders/shader_lamp.fs");  
	lampShader.use();

	// Se pasan las matrices al shader de focos de luz
	lampShader.setMat4("model", model);
	lampShader.setMat4("view", view);
	lampShader.setMat4("projection", projection);


	//Shader projectionShader("shaders/shader_light.vs", "shaders/shader_light.fs");
	//Shader projectionShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs");

	// ------ Aquí se selecciona el tipo de luz a utilizar, descomentar la línea que se va a ocupar -----

	//Shader lightingShader("shaders/shader_texture_light_dir.vs", "shaders/shader_texture_light_dir.fs");    //Directional
	//Shader lightingShader("shaders/shader_texture_light_pos.vs", "shaders/shader_texture_light_pos.fs");    //Positional
	//Shader lightingShader("shaders/shader_texture_light_spot.vs", "shaders/shader_texture_light_spot.fs");  //Spotlight

	Shader lightingShader(Light_VertxShader, Light_FragShader);             //Light_VertxShader y Light_FragShader son variables que pueden tener cualquiera de las rutas anteriores

	//Se habilita el Shader de iluminación
	lightingShader.use();

	// Se pasan las matrices al shader de iluminación
	lightingShader.setMat4("model", model);
	lightingShader.setMat4("view", view);
	lightingShader.setMat4("projection", projection);

	// ------ Aquí se selecciona el tipo de luz a utilizar, descomentar la línea que se va a ocupar -----

	//	Directional - Luz direccional, se manda el sentido en el que apunta la fuente de luz
	//lightingShader.setVec3("light.direction", lightDirection);

	//	Positional - Luz posicional, se manda la ubicación de la fuente de luz
	//lightingShader.setVec3("light.position", lightPosition);

	//	Spotlight - Luz de reflector, la luz se encuentra en la cámara
	//lightingShader.setVec3("light.position", camera.Position);
	//lightingShader.setVec3("light.direction", camera.Front);
	//lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));

	// Mientras la Luz sea de tipo reflector, ésta se estará actualizando constantemente en base a la posición de la cámara
	if (Luz_Reflector == 1)
	{
		Light_Position = camera.Position;
		Light_Direction = camera.Front;
		Light_CutOff = glm::cos(glm::radians(12.5f));
	}

	lightingShader.setVec3("light.position", Light_Position);
	lightingShader.setVec3("light.direction", Light_Direction);
	lightingShader.setFloat("light.cutOff", Light_CutOff);

	lightingShader.setVec3("viewPos", camera.Position);

	// Propiedades de la luz --- Luz Variable
	lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
	lightingShader.setVec3("light.diffuse", R, G, B);
	lightingShader.setVec3("light.specular", R, G, B);

	//Para la Luz Posicional y de Reflector
	//lightingShader.setFloat("light.constant", 1.0f);
	//lightingShader.setFloat("light.linear", 0.09f);
	//lightingShader.setFloat("light.quadratic", 0.032f);
	lightingShader.setFloat("light.constant", 0.0005f);
	lightingShader.setFloat("light.linear", 0.0005f);
	lightingShader.setFloat("light.quadratic", 0.0005f);

	// Brillo del material
	lightingShader.setFloat("material_shininess", opacidad);


	// ------------------------- Zona de Dibujo -------------------------

	if( enFeria )
	{
		// ------------ Skybox ------------
		glBindVertexArray(VAO);
		lightingShader.use();
		model_loc = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));    //Ubicación del skybox
		model_loc = glm::scale(model_loc, glm::vec3(2000.0f));                       //Escala de todos los lados del cubo
		lightingShader.setMat4("model", model_loc);

		// Se elige el skybox
		// Nota: El skybox debe tener un poco más de brillo en su luz ambiental que todo lo demás
		if (modoDia)
		{
			lightingShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
			lightingShader.setInt("material_diffuse", t_skybox_tarde);
		}
		if (modoAtardecer)
		{
			lightingShader.setVec3("light.ambient", 0.85f, 0.85f, 0.85f);
			lightingShader.setInt("material_diffuse", t_skybox_tarde);
		}
		if (modoNoche)
		{
			lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
			lightingShader.setInt("material_diffuse", t_skybox_noche);
		}

		glDrawArrays(GL_QUADS, 186, 24);

		// Se modifica la luz ambiental para todo lo demás
		if (modoDia)
			lightingShader.setVec3("light.ambient", 0.75f, 0.75f, 0.75f);
		if (modoAtardecer)
			lightingShader.setVec3("light.ambient", 0.45f, 0.45f, 0.45f);
		if (modoNoche)
			lightingShader.setVec3("light.ambient", 0.05f, 0.05f, 0.05f);

		// ------------ Montaña Rusa ------------
		lightingShader.use();
		model_loc = glm::translate(glm::mat4(1.0f), glm::vec3(55.0f, 10.0f, -5.0f));             //Ubicación de la montaña rusa
		model_loc = glm::rotate(model_loc, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, .0f));    //Rotación de la montaña rusa
		model_loc = glm::scale(model_loc, glm::vec3(2.0f));                                      //Escala de toda la montaña
		montañaRusa(model_loc, lightingShader, lampShader, modelShader, modelo[0]);              //Se dibuja la montaña

		// ------------ Rueda de la Fortuna ------------
		glBindVertexArray(VAO);
		lightingShader.use();
		model_loc = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 32.0f, 65.0f));            //Ubicación de la rueda de la fortuna
		model_loc = glm::rotate(model_loc, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));    //Rotación de la rueda de la fortuna
		model_loc = glm::scale(model_loc, glm::vec3(2.0f));                                      //Escala de toda la rueda
		ruedaDeLaFortuna(model_loc, lightingShader, lampShader);                                 //Se dibuja la rueda

		// ------------- Sol / Luna ---------------
		lampShader.use();

		if (modoDia)
			lampShader.setVec3("color", 1.00000f, 1.00000f, 0.20000f);  //Amarillo (Sol de medio día)
		if (modoAtardecer)
			lampShader.setVec3("color", 1.00000f, 0.53333f, 0.06667f);  //Naranja (Sol de la tarde)
		if (modoNoche)
			lampShader.setVec3("color", 1.00000f, 1.00000f, 0.79216f);  //Blanco (Luna)

		model = glm::translate(glm::mat4(1.0f), lightPosition);
		model = glm::scale(model, glm::vec3(2.0f));
		lampShader.setMat4("model", model);
		esfera.render();       //Sol redondo


		// ------------------- MODELOS -------------------
		modelShader.use();


		//  ------------- Carrusel -------------

		// --- Base ---
		model_loc = glm::mat4(1.0f);
		model_loc = glm::translate(model_loc, glm::vec3(-200.0f, -11.5f, 165.0f));   //Ubicación de todo el carrusel
		model_loc = glm::scale(model_loc, glm::vec3(4.0f));                       //Escala de todo el carrusel
		model = model_loc;
		modelShader.setMat4("model", model);
		modelo[31].Draw(modelShader);

		// --- Estructura ---
		model = model_loc;
		model = glm::rotate(model, glm::radians(giroCarrusel), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		modelShader.setMat4("model", model);
		modelo[32].Draw(modelShader);

		// --- Leones Interior ---
		model = model_loc;
		model = glm::translate(model, glm::vec3(0.0f, posLeonesInteriorY, 0.0f) );
		model = glm::rotate(model, glm::radians(giroCarrusel), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		modelShader.setMat4("model", model);
		modelo[33].Draw(modelShader);

		// --- Leones Exterior ---
		model = model_loc;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));  //Los leones exteriores inician arriba
		model = glm::translate(model, glm::vec3(0.0f, posLeonesExteriorY, 0.0f) );
		model = glm::rotate(model, glm::radians(giroCarrusel), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		modelShader.setMat4("model", model);
		modelo[34].Draw(modelShader);

		//  ------------- Carros Chocones -------------

		// --- Base ---
		model_loc = glm::mat4(1.0f);
		model_loc = glm::translate(model_loc, glm::vec3(-225.0f, -23.0f, -70.0f));     //Ubicación de todo el juego
		model = model_loc;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));   //Orientación de la base
		model = glm::scale(model, glm::vec3(26.5f, 30.0f, 26.5f));                      //Escala de la base
		modelShader.setMat4("model", model);
		modelo[35].Draw(modelShader);

		// --- Carro Azul Rey (Carro Animado) ---
		model = model_loc;
		model = glm::translate(model, glm::vec3(-40.0f, 12.25f, 45.0f));                         //Ubicación inicial del carro
		model = glm::translate(model, glm::vec3(posCarroAzulX, posCarroAzulY, posCarroAzulZ));   //Ubicación durante la animación
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));            //Rotación inicial del carro
		model = glm::rotate(model, glm::radians(rotCarroAzul), glm::vec3(0.0f, 1.0f, 0.0f));     //Orientación del coche en la animación
		model = glm::scale(model, glm::vec3(1.8f, 2.1f, 1.8f));
		modelShader.setMat4("model", model);
		modelo[36].Draw(modelShader);

		// --- Carro Blanco (Carro Animado) ---
		model = model_loc;
		model = glm::translate(model, glm::vec3(40.0f, 12.25f, 45.0f));                                //Ubicación del carro
		model = glm::translate(model, glm::vec3(posCarroBlancoX, posCarroBlancoY, posCarroBlancoZ));   //Ubicación durante la animación
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));                 //Rotación inicial del carro
		model = glm::rotate(model, glm::radians(rotCarroBlanco), glm::vec3(0.0f, 1.0f, 0.0f));         //Orientación del coche en la animación
		model = glm::scale(model, glm::vec3(1.8f, 2.1f, 1.8f));
		modelShader.setMat4("model", model);
		modelo[37].Draw(modelShader);

		// --- Carro Rojo (Carro Animado) ---
		model = model_loc;
		model = glm::translate(model, glm::vec3(40.0f, 12.25f, -40.0f));                         //Ubicación del carro
		model = glm::translate(model, glm::vec3(posCarroRojoX, posCarroRojoY, posCarroRojoZ));   //Ubicación durante la animación
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));           //Rotación inicial del carro
		model = glm::rotate(model, glm::radians(rotCarroRojo), glm::vec3(0.0f, 1.0f, 0.0f));     //Orientación del coche en la animación
		model = glm::scale(model, glm::vec3(1.8f, 2.1f, 1.8f));
		modelShader.setMat4("model", model);
		modelo[38].Draw(modelShader);

		// --- Carro Rosa ---
		model = model_loc;
		model = glm::translate(model, glm::vec3(-40.0f, 12.25f, -27.5f));              //Ubicación del carro
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));  //Orientación del coche
		model = glm::scale(model, glm::vec3(1.8f, 2.1f, 1.8f));
		modelShader.setMat4("model", model);
		modelo[39].Draw(modelShader);

		// --- Carro Verde ---
		model = model_loc;
		model = glm::translate(model, glm::vec3(-40.0f, 12.25f, -40.0f));              //Ubicación del carro
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));  //Orientación del coche
		model = glm::scale(model, glm::vec3(1.8f, 2.1f, 1.8f));
		modelShader.setMat4("model", model);
		modelo[40].Draw(modelShader);

		//  ------------- Pasto -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-75.0f, -20.0f, 40.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.75f, 1.75f, 0.8f));
		modelShader.setMat4("model", model);
		modelo[1].Draw(modelShader);

		// ------------- Estadio Pokemon -------------
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(70.0f, -70.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.00305f));
		modelShader.setMat4("model", model);
		modelo[2].Draw(modelShader);

		// ------------- Squirtle -------------
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-50.0f, 13.5f, -15.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		modelShader.setMat4("model", model);
		modelo[3].Draw(modelShader);

		// ------------- Charmander -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-77.5f, -13.5f, -2.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		modelShader.setMat4("model", model);
		modelo[4].Draw(modelShader);

		// ------------- Vaporeon ------------- 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-57.5f, -13.5f, -5.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		modelShader.setMat4("model", model);
		modelo[5].Draw(modelShader);

		// ------------- Pikachu ------------- 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-85.0f, -13.5f, 15.0f));
		model = glm::scale(model, glm::vec3(0.075f));
		modelShader.setMat4("model", model);
		modelo[6].Draw(modelShader);

		// ------------- Zapdos -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-60.0f, 20.0f, 15.0f));
		model = glm::translate(model, glm::vec3(posZapdosX, posZapdosY, posZapdosZ));
		model = glm::rotate(model, glm::radians(rotZapdos), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		modelShader.setMat4("model", model);
		modelo[7].Draw(modelShader);

		// ------------- Casa del Terror -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(52.0f, -15.0f, -92.0f));
		model = glm::rotate(model, glm::radians(-235.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		modelShader.setMat4("model", model);
		modelo[8].Draw(modelShader);

		if (modoNoche)
		{
			// ------------- Mujer Fantasma -------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(19.0f, -10.5f, -80.0f));
			model = glm::translate(model, glm::vec3(0.0f, posMujerFantasmaY, 0.0f));
			model = glm::rotate(model, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.125f));
			modelShader.setMat4("model", model);
			modelo[9].Draw(modelShader);

			// ------------- Árbol Fantasma -------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(85.0f, -13.0f, -85.0f));
			model = glm::translate(model, glm::vec3(posFantasmaX, 0.0f, posFantasmaZ));
			model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.5f));
			modelShader.setMat4("model", model);
			modelo[10].Draw(modelShader);
		}

		// ------------- Castillo -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(40.0f, -12.0f, 105.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f));
		modelShader.setMat4("model", model);
		modelo[12].Draw(modelShader);

		// ------------- Mickey -------------
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 30.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		modelShader.setMat4("model", model);
		modelo[13].Draw(modelShader);

		// ------------- Mickey1 -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(37.5f, -12.0f, 110.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.18f));
		modelShader.setMat4("model", model);
		modelo[14].Draw(modelShader);

		// ------------- Minnie -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(12.5f, -12.0f, 95.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.0f));
		modelShader.setMat4("model", model);
		modelo[15].Draw(modelShader);

		// ------------- Minnie1 -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(37.5f, -12.5f, 97.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.085f));
		modelShader.setMat4("model", model);
		modelo[16].Draw(modelShader);

		if (!modoNoche)  //El hombre y el carro de helados no se renderizan de noche
		{
			// ------------- Hombre -------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-125.0f, -13.5f, 40.0f));
			model = glm::scale(model, glm::vec3(0.065f));
			modelShader.setMat4("model", model);
			modelo[17].Draw(modelShader);

			// ------------- Carro helados -------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-115.0f, -13.5f, 40.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.065f));
			modelShader.setMat4("model", model);
			modelo[18].Draw(modelShader);
		}

		// ------------- Gato Negro -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(19.0f, -11.8f, -70.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		modelShader.setMat4("model", model);
		modelo[19].Draw(modelShader);

		// ------------- Perro -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(19.0f, -11.5f, 60.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.09f));
		modelShader.setMat4("model", model);
		modelo[20].Draw(modelShader);

		// ------------- Gato -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-75.0f, -9.0f, 60.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		modelShader.setMat4("model", model);
		modelo[21].Draw(modelShader);

		// ------------- Juego Basket -------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(35.0f, -12.5f, 50.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f));
		modelShader.setMat4("model", model);
		modelo[22].Draw(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(35.0f, -12.5f, 60.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f));
		modelShader.setMat4("model", model);
		modelo[22].Draw(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(35.0f, -12.5f, 70.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f));
		modelShader.setMat4("model", model);
		modelo[22].Draw(modelShader);

		if (!modoNoche)  //La mujer y el vendedor de comida no se renderizan de noche
		{
			// ------------- Puesto de Comida -------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-75.0f, -11.5f, 125.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.1f));
			modelShader.setMat4("model", model);
			modelo[23].Draw(modelShader);

			// ------------- Vendedor de Comida -------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-75.0f, -11.5f, 135.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.1f));
			modelShader.setMat4("model", model);
			modelo[24].Draw(modelShader);

			// ------------- Mujer -------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-75.0f, -7.5f, 120.0f));
			model = glm::scale(model, glm::vec3(5.0f));
			modelShader.setMat4("model", model);
			modelo[25].Draw(modelShader);
		}

		// ------------- Árboles (Tipo 1) -------------

		// Árbol redondeado
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -10.0f, -80.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.25f, 0.15f));
		modelShader.setMat4("model", model);
		modelo[26].Draw(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-120.0f, -10.5f, 117.5f));
		model = glm::scale(model, glm::vec3(0.20f, 0.40f, 0.20f));
		modelShader.setMat4("model", model);
		modelo[26].Draw(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-145.0f, -10.5f, 210.0f));
		model = glm::scale(model, glm::vec3(0.20f, 0.40f, 0.20f));
		modelShader.setMat4("model", model);
		modelo[26].Draw(modelShader);

		// ------------- Árboles (Tipo 3) -------------

		// Pino con franjas
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(85.0f, -13.0f, -65.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.3f, 1.0f));
		modelShader.setMat4("model", model);
		modelo[27].Draw(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-150.0f, -13.0f, 10.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.75f, 1.0f));
		modelShader.setMat4("model", model);
		modelo[27].Draw(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(15.0f, -11.0f, 200.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.75f, 1.0f));
		modelShader.setMat4("model", model);
		modelo[27].Draw(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-210.0f, -11.5f, 205.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.60f, 1.0f));
		modelShader.setMat4("model", model);
		modelo[27].Draw(modelShader);

		// ------------- Árbol (Tipo 4) -------------

		// Árbol con base
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-25.0f, -12.0f, 60.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		modelShader.setMat4("model", model);
		modelo[28].Draw(modelShader);

		// ------------- Batman -------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(30.0f, -11.5f, 25.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f));
		modelShader.setMat4("model", model);
		modelo[30].Draw(modelShader);

		glBindVertexArray(0);
	}

	if (enCasaTerror)
	{
		// ------------- Casa del Terror por dentro -------------
	
		glBindVertexArray(VAO);
		lightingShader.use();

		// ------------------- Muros -------------------

		lightingShader.setInt("material_diffuse", t_madera);          //Texturas de todos los muros
		lightingShader.setInt("material_specular", t_madera_brillo);

		// ---- Contorno de la casa ----
		model_loc = glm::mat4(1.0f);
		model_loc = glm::translate(model_loc, glm::vec3(0.0f, 500.0f, -7500.0f));
		model = model_loc;
		model = glm::scale(model, glm::vec3(7502.0f, 3000.0f, 15008.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 210, 24);

		// ---- Muro 1 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(-682.0f, 0.0f, 5393.5f));
		model = glm::scale(model, glm::vec3(6138.0f, 3000.0f, 469.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 2 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(2216.0f, 0.0f, 4221.0f));
		model = glm::scale(model, glm::vec3(341.0f, 3000.0f, 1876.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 3 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(-2898.5f, 0.0f, -234.5f));
		model = glm::scale(model, glm::vec3(1705.0f, 3000.0f, 469.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 4 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(-1875.5f, 0.0f, 1641.5f));
		model = glm::scale(model, glm::vec3(341.0f, 3000.0f, 4221.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 5 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(-1023.0f, 0.0f, -2579.5f));
		model = glm::scale(model, glm::vec3(2046.0f, 3000.0f, 469.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 6 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(-170.5f, 0.0f, 703.5f));
		model = glm::scale(model, glm::vec3(341.0f, 3000.0f, 6097.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 7 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(1875.5f, 0.0f, 1641.5f));
		model = glm::scale(model, glm::vec3(3751.0f, 3000.0f, 469.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 8 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(-1193.5f, 0.0f, -5393.5f));
		model = glm::scale(model, glm::vec3(5115.0f, 3000.0f, 469.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 9 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(1534.5f, 0.0f, -3048.5f));
		model = glm::scale(model, glm::vec3(341.0f, 3000.0f, 5159.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ---- Muro 10 ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(3239.5f, 0.0f, -2579.5f));
		model = glm::scale(model, glm::vec3(1023.0f, 3000.0f, 469.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 0, 24);

		// ------------------- Puertas -------------------

		// ---- Puerta Delantera ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(0.0f, -600.0f, 7490.0f));
		model = glm::scale(model, glm::vec3(1000.0f, 1750.0f, 1.0f));

		lightingShader.setInt("material_diffuse", t_puerta);
		lightingShader.setInt("material_specular", t_puerta_brillo);
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 20, 4);

		// ---- Puerta Trasera ----
		model = model_loc;
		model = glm::translate(model, glm::vec3(0.0f, -600.0f, -7490.0f));
		model = glm::scale(model, glm::vec3(1000.0f, 1750.0f, 1.0f));

		lightingShader.setInt("material_diffuse", t_puerta);
		lightingShader.setInt("material_specular", t_puerta_brillo);
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 20, 4);

		// ------------------- Monstruos y Modelos -------------------

		modelShader.use();

		// ---- Librero ----

		compruebaUbicacionLibrero(); //Se verifica si el librero ya debe caer o no

		if (libreroDePie)
		{
			model = model_loc;
			model = glm::translate(model, glm::vec3(-3580.5f, -1500.0f, -1407.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.5f, 8.0f, 7.5f));
			modelShader.setMat4("model", model);
			modelo[42].Draw(modelShader);
		}
		else
		{
			if (libreroSuena)  //El sonido del librero cayendo sólo se reproduce una vez
			{
				libreroSuena = 0;

				//  Se reproduce el sonido de la caída
				efectoDeSonido->setSoundVolume(1.0f);
				efectoDeSonido->play3D("Audio/MaderaCayendo.mp3", vec3df(0, 0, -1));
			}

			model = model_loc;
			model = glm::translate(model, glm::vec3(-3580.5f, -1300.0f, -1407.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(4.5f, 8.0f, 7.5f));
			modelShader.setMat4("model", model);
			modelo[42].Draw(modelShader);
		}

		// ---- Escritorio ----

		model = model_loc;
		model = glm::translate(model, glm::vec3(-2898.5f, -800.0f, 938.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 18.0f, 20.0f));
		modelShader.setMat4("model", model);
		modelo[43].Draw(modelShader);

		// ---- Llave ----

		if (!llaveTomada)   //Si aún no se toma la llave, ésta se renderiza
		{
			model = model_loc;
			model = glm::translate(model, glm::vec3(-2898.5f, -525.0f, 1100.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(475.0f));
			modelShader.setMat4("model", model);
			modelo[44].Draw(modelShader);
		}

		if (llaveTomada)   //Si ya se tomó la llave, aparece la araña y la sirena
		{
			// ---- Mujer Espectro ----
			model = model_loc;
			model = glm::translate(model, glm::vec3(-3069.0f, -1500.0f, -6566.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1000.0f));
			modelShader.setMat4("model", model);
			modelo[41].Draw(modelShader);

			// // ---- Araña ----
			model = model_loc;
			model = glm::translate(model, glm::vec3(1193.5f, -1500.0f, 4455.5f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(475.0f));
			modelShader.setMat4("model", model);
			modelo[45].Draw(modelShader);

			// ---- Sirena ----
			model = model_loc;
			model = glm::translate(model, glm::vec3(3239.5f, -1500.0f, -3517.5f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(800.0f));
			modelShader.setMat4("model", model);
			modelo[46].Draw(modelShader);

			compruebaUbicacionSirena(); // Se verifica que ya se haya llegado hasta donde está la sirena

			if ( ubicacionSirena && suenaSirena )  //La sirena sólo grita la primera vez que se pasa junto a ella
			{
				suenaSirena = 0;

				//  Se reproduce el sonido de la caída
				efectoDeSonido->setSoundVolume(1.0f);
				efectoDeSonido->play3D("Audio/GritoMujer.mp3", vec3df(0, 0, -1));
			}
		}

		// ---- Zombie ----

		compruebaUbicacionZombie();

		if (apareceZombie) //El zombie sólo aparece si te acercas demasiado
		{
			model = model_loc;
			model = glm::translate(model, glm::vec3(-2898.5f, -1500.0f, 6566.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(10.0f));
			modelShader.setMat4("model", model);
			modelo[47].Draw(modelShader);
		}

		// ---- Cuadros ----

		compruebaUbicacionCuadros();

		if (!cuadrosSangrientos) //Los cuadros son normales
		{
			// Cuadro 1
			model = model_loc;
			model = glm::translate(model, glm::vec3(1278.75f, 2000.0f, -4000.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(450.0f));
			modelShader.setMat4("model", model);
			modelo[48].Draw(modelShader);
		}
		if (cuadrosSangrientos) //Los cuadros se vuelven sangrientos
		{
			// Cuadro 1
			model = model_loc;
			model = glm::translate(model, glm::vec3(1278.75f, 2000.0f, -4000.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(450.0f));
			modelShader.setMat4("model", model);
			modelo[49].Draw(modelShader);

			// Cuadro 2
			model = model_loc;
			model = glm::translate(model, glm::vec3(85.25f, 2000.0f, -469.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(450.0f));
			modelShader.setMat4("model", model);
			modelo[50].Draw(modelShader);

			// Cuadro 3
			model = model_loc;
			model = glm::translate(model, glm::vec3(1875.5f, 2000.0f, 1289.75f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(450.0f));
			modelShader.setMat4("model", model);
			modelo[51].Draw(modelShader);
		}

		// ---- Lampara ----

		model = model_loc;
		model = glm::translate(model, glm::vec3(1705.0f, -550.0f, -4924.5f));
		model_temp = model;    //Se guarda la ubicación de la lámpara

		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(850.0f));
		modelShader.setMat4("model", model);
		modelo[52].Draw(modelShader);

		compruebaUbicacionLampara();  //Se verifica si ya se llegó hasta dondo está la lámpara

		if (enciendeLampara) //Se ha llegado hasta la lámpara. La linterna falla
		{
			// ---- Se cambia el tipo de iluminación ----

			// Luz Posicional
			strcpy_s(Light_VertxShader, "shaders/shader_texture_light_pos.vs");
			strcpy_s(Light_FragShader, "shaders/shader_texture_light_pos.fs");

			Light_Position = lightPosition = glm::vec3(model_temp[3][0] + 350.0f, model_temp[3][1] + 1250.0f, model_temp[3][2]);  //Ubicación de la lámpara

			Luz_Reflector = 0;  //La luz ya no es de tipo reflector
		}
		
		glBindVertexArray(0);
	}

}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//To Enable capture of our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glewInit();

	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	esfera.init();
	glEnable(GL_DEPTH_TEST);

	// Se crea el shader para cargar los modelos
	Shader modelShader("Shaders/modelLoading.vs", "Shaders/modelLoading.fs");
	
	// Se cargan los modelos a utilizar
	Model batarang((GLchar *)"Models/Batarang/INJ_iOS_WEAPON_Batman's_Batarang_Dawn_Of_Justice.obj", 0);
	Model pasto((char *)"Models/Pasto/pasto.obj", 1);
	Model estadioPokemon((char *)"Models/Stadium/10093_Wembley_stadion_V3_Iteration0.obj", 1);
	Model Squirtle((char *)"Models/Squirtle/squirtle-pokemon-go.obj", 1);
	Model Charmander((char *)"Models/Charmander/charmander.obj", 1);
	Model Vaporeon((char *)"Models/Vaporeon/vaporeon.obj", 1);
	Model Pikachu((char *)"Models/Pikachu/pikachu.obj", 1);
	Model Zapdos((char *)"Models/Zapdos/zapados-pokemon-go.obj", 1);
	Model Casona((char *)"Models/Cabania/farmhouse_obj.obj", 1);
	Model womanGhost((char *)"Models/womanGhost/skghostmesh.obj", 1);
	Model treeGhost((char *)"Models/Tree Ghost/Tree Ghost.obj", 0);
	Model arbusto((char *)"Models/Arbusto/karafuruueki.obj", 0);
	Model Castillo((char *)"Models/Castillo/Castle OBJ.obj", 1);
	Model Mickey((char *)"Models/Mickey Mouse/Mickey Mouse.obj", 1);
	Model Mickey1((char *)"Models/mickeyHT/mickey.obj", 1);
	Model Minnie((char *)"Models/Minnie Mouse/MinnieMouse.obj", 1);
	Model Minnie1((char *)"Models/Minnie Mouse-Princess/Minnie Mouse.obj", 0);
	Model Hombre((char *)"Models/Business_Man/GTP_BMan_Jack_07_Stg_Lsn_Adl_Ccs_Gry_Mgr.obj", 1);
	Model CarritoHelados((char *)"Models/CarritoHelados/13924_Ice_Cream_vending_Cart_v2_l2.obj", 1);
	Model GatoNegro((char *)"Models/Gato2/12222_Cat_v1_l3.obj", 1);
	Model Perro((char *)"Models/perro/12226_Dog_v2_l3.obj", 1);
	Model Gato((char *)"Models/Gato/cat.obj", 0);
	Model juegoBasket((char *)"Models/Game Basketball/basketball.obj", 1);
	Model puestoComida((char *)"Models/Puesto_comida/city_stall.obj", 1);
	Model vendedorComida((char *)"Models/VendedorComida/BIO-I_PC_N.P.C_Food_Vendor.obj", 0);
	Model Mujer((char *)"Models/mujer/Scan-17.obj", 1);
	Model Tree((char *)"Models/Tree/N64_Tree.obj", 1);
	Model Tree3((char *)"Models/Tree3/n64tree.obj", 1);
	Model Tree4((char *)"Models/Tree4/Basic Tree 1.obj", 1);
	Model Batman2((char *)"Models/Batman2/B-AO_iOS_HERO_Bruce_Wayne_Batman_Long_Halloween.obj", 0);

	Model CarruselBase((char *)"Models/Carrusel-Leon/CarruselBase.obj", 0);
	Model CarruselEstructura((char *)"Models/Carrusel-Leon/CarruselEstructura.obj", 0);
	Model CarruselLeonesInterior((char *)"Models/Carrusel-Leon/CarruselLeonesInterior.obj", 0);
	Model CarruselLeonesExterior((char *)"Models/Carrusel-Leon/CarruselLeonesExterior.obj", 0);

	Model baseCarrosChocones((char *)"Models/baseCarrosChocones/BaseCarros.obj", 0);
	Model carroAzulRey((char *)"Models/carrosChocones/CarroAzulrey.obj", 0);
	Model carroBlanco((char *)"Models/carrosChocones/CarroBlanco.obj", 0);
	Model carroMorado((char *)"Models/carrosChocones/CarroMorado.obj", 0);
	Model carroNegro((char *)"Models/carrosChocones/CarroNegro.obj", 0);
	Model carroRojo((char *)"Models/carrosChocones/CarroRojo.obj", 0);
	Model carroRosa((char *)"Models/carrosChocones/CarroRosa.obj", 0);
	Model carroVerde((char *)"Models/carrosChocones/CarroVerde.obj", 0);

	Model mujerEspectro((char *)"Models/mujerEspectro/Michiko Yamada (Ghost, Kitchen).obj", 0);
	Model librero((char *)"Models/librero/bookcase_monster.obj", 0);
	Model escritorio((char *)"Models/escritorio/Hotel Desk.obj", 0);
	Model llave((char *)"Models/llave/Key.obj", 0);
	Model spider((char *)"Models/Spider/DF_PC_BOSS_Giant_Spider.obj", 0);
	Model siren((char *)"Models/Siren/DF_PC_ENEMY_Siren.obj", 0);
	Model zombie((char *)"Models/Zombie/Zombie.obj", 0);
	Model cuadro1((char *)"Models/Cuadro1/Lady with Marten Picture.obj", 0);
	Model cuadroSangriento1((char *)"Models/CuadroSangriento1/Lady with Marten Picture.obj", 0);
	Model cuadroSangriento2((char *)"Models/CuadroSangriento2/Turban Girl Picture.obj", 0);
	Model cuadroSangriento3((char *)"Models/CuadroSangriento3/Noblewoman Picture.obj", 0);
	Model lampara((char *)"Models/Lampara/locDeco_townHall_wallLamp.obj", 0);

	//Se guardan todos los modelos en un arreglo
	Model modelo[] = { 
		batarang, pasto, estadioPokemon, Squirtle, Charmander, Vaporeon, Pikachu, 
		Zapdos, Casona, womanGhost, treeGhost, arbusto, Castillo, Mickey, Mickey1,
		Minnie, Minnie1, Hombre, CarritoHelados, GatoNegro, Perro, Gato, juegoBasket, 
		puestoComida, vendedorComida, Mujer, Tree, Tree3, Tree4, Tree4, Batman2, 

		CarruselBase, CarruselEstructura, CarruselLeonesInterior, CarruselLeonesExterior,
		baseCarrosChocones, carroAzulRey, carroBlanco, carroRojo, carroRosa, carroVerde,

		mujerEspectro, librero, escritorio, llave, spider, siren, zombie,
		cuadro1, cuadroSangriento1, cuadroSangriento2, cuadroSangriento3, lampara
	};


	/*
	Model batarang((GLchar *)"Models/Batarang/INJ_iOS_WEAPON_Batman's_Batarang_Dawn_Of_Justice.obj", 0);
	Model Casona((char *)"Models/Cabania/farmhouse_obj.obj", 1);
	Model pasto((char *)"Models/Pasto/pasto.obj", 1);

	Model mujerEspectro((char *)"Models/mujerEspectro/Michiko Yamada (Ghost, Kitchen).obj", 0);
	Model librero((char *)"Models/librero/bookcase_monster.obj", 0);
	Model escritorio((char *)"Models/escritorio/Hotel Desk.obj", 0);
	Model llave((char *)"Models/llave/Key.obj", 0);
	Model spider((char *)"Models/Spider/DF_PC_BOSS_Giant_Spider.obj", 0);
	Model siren((char *)"Models/Siren/DF_PC_ENEMY_Siren.obj", 0);
	Model zombie((char *)"Models/Zombie/Zombie.obj", 0);
	Model cuadro1((char *)"Models/Cuadro1/Lady with Marten Picture.obj", 0);
	Model cuadroSangriento1((char *)"Models/CuadroSangriento1/Lady with Marten Picture.obj", 0);
	Model cuadroSangriento2((char *)"Models/CuadroSangriento2/Turban Girl Picture.obj", 0);
	Model cuadroSangriento3((char *)"Models/CuadroSangriento3/Noblewoman Picture.obj", 0);
	Model lampara((char *)"Models/Lampara/locDeco_townHall_wallLamp.obj", 0);

	Model modelo[] = {
		batarang, pasto, batarang, batarang,
		batarang, batarang, batarang, batarang, Casona, batarang, batarang,
		mujerEspectro, librero, escritorio, llave, spider, siren, zombie,
		cuadro1, cuadroSangriento1, cuadroSangriento2, cuadroSangriento3, lampara
	};
	*/

	// ----- Se reproduce la canción de fondo -----
	musicaDeFondo->setSoundVolume(0.20);  //El volumen se pone al 25%
	musicaDeFondo->play3D(cancionDeFondo, vec3df(0, 0, -1), true, false, true);

	// Inicialización de KeyFrames
	LoadKeyFrames(keyFrameCarro, frameIndexCarroPtr, "KeyFramesCarro.txt");
	LoadKeyFrames(keyFrameZapdos, frameIndexZapdosPtr, "KeyFramesZapdos.txt");
	LoadKeyFrames(keyFrameCarroAzul, frameIndexCarroAzulPtr, "KeyFramesCarroAzul.txt");
	LoadKeyFrames(keyFrameCarroBlanco, frameIndexCarroBlancoPtr, "KeyFramesCarroBlanco.txt");
	LoadKeyFrames(keyFrameCarroRojo, frameIndexCarroRojoPtr, "KeyFramesCarroRojo.txt");

    // render loop
    // While the windows is not closed
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        // input
        // -----
        my_input(window);
		animate();

        // render
        // Backgound color
        glClearColor(bgColorRed, bgColorGreen, bgColorBlue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Mi función de dibujo
		display(modelShader, modelo);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window)
{
	// Cerrar la ventana
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Movimiento de la cámara
	if (enFeria)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, (float)deltaTime * 15);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, (float)deltaTime * 15);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, (float)deltaTime * 15);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, (float)deltaTime * 15);
	}
	if (enCasaTerror)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, (float)deltaTime * 800);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, (float)deltaTime * 800);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, (float)deltaTime * 800);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, (float)deltaTime * 800);
	}

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		system("Proyecto-CGeHIC-20192\\proyecto.exe");
		//system("\\Proyecto-CGeHIC-20192\\proyecto.exe");
		printf("ABriendo");

	}

	posicionCamara = camera.Position;
	frenteCamara = camera.Front;

    //printf("\nPosicion Camara: (%f, %f, %f)", posicionCamara.x, posicionCamara.y, posicionCamara.z);
	//printf("\nFrente Camara: (%f, %f, %f)", frenteCamara.x, frenteCamara.y, frenteCamara.z);


	// --------------- Selección del entorno ---------------

	// --- Casa del terror ---
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && (FPresionado == 0))
	{
		if (compruebaUbicacionCasaTerror() && modoNoche)  // Sólo se puede entrar a la casa del terror durante la noche
		{
			enFeria = 0;
			enCasaTerror = 1;
			enShowAcuatico = 0;

			// --------- Se cambia el tipo de iluminación ---------

			strcpy_s(Light_VertxShader, "shaders/shader_texture_light_spot.vs");
			strcpy_s(Light_FragShader, "shaders/shader_texture_light_spot.fs");
			Luz_Reflector = 1;  //La luz ahora es de tipo reflector

			// --------- Se cambia la música de fondo ---------

			// Si la canción actual es diferente a la nueva canción, ésta se cambia
			cancionDeFondo = "Audio/MusicaSuspenso.mp3";

			musicaDeFondo->stopAllSounds();
			musicaDeFondo->play3D(cancionDeFondo, vec3df(0, 0, -1), true);

			// Suena cómo se cierra la pueta al entrar
			efectoDeSonido->setSoundVolume(2.0f);
			efectoDeSonido->play3D("Audio/PuertaCerrandose.mp3", vec3df(0, 0, -1));
		}
		else if (compruebaUbicacionCasaTerror() && !modoNoche) // Si se trata de ingresar durante el día o la tarde, la puerta suena cerrada
		{
			const char* sonidoPuerta = "Audio/LockedDoor1.mp3";
			
			int valor = 1 + rand() % 4;  //Valor aleatorio entre 1 y 4

			if (valor == 1)
				sonidoPuerta = "Audio/LockedDoor1.mp3";
			if (valor == 2)
				sonidoPuerta = "Audio/LockedDoor2.mp3";
			if (valor == 3)
				sonidoPuerta = "Audio/LockedDoor3.mp3";
			if (valor == 4)
				sonidoPuerta = "Audio/LockedDoor4.mp3";

			efectoDeSonido->setSoundVolume(2.0f);
			efectoDeSonido->play3D(sonidoPuerta, vec3df(0, 0, -1));
		}

		if( compruebaUbicacionLlave() )  //Función para recoger la llave de la cassa del terror
		{
			llaveTomada = 1;  //La llave se recoge

			// Se reproduce el sonido de que se tomaron las llaves
			efectoDeSonido->setSoundVolume(2.0f);
			efectoDeSonido->play3D("Audio/AgarrarLlave.mp3", vec3df(0, 0, -1));
		}


		if (compruebaUbicacionSalida() && !llaveTomada)  //Si se llegó a la salida sin la llave, la puerta suena cerrada
		{
			const char* sonidoPuerta = "Audio/LockedDoor1.mp3";

			int valor = 1 + rand() % 4;  //Valor aleatorio entre 1 y 4

			if (valor == 1)
				sonidoPuerta = "Audio/LockedDoor1.mp3";
			if (valor == 2)
				sonidoPuerta = "Audio/LockedDoor2.mp3";
			if (valor == 3)
				sonidoPuerta = "Audio/LockedDoor3.mp3";
			if (valor == 4)
				sonidoPuerta = "Audio/LockedDoor4.mp3";

			efectoDeSonido->setSoundVolume(2.0f);
			efectoDeSonido->play3D(sonidoPuerta, vec3df(0, 0, -1));
		}
		if (compruebaUbicacionSalida() && llaveTomada)  //Si se llegó a la salida con la llave, se puede salir
		{
			//Se regresa a la feria
			enFeria = 1;          
			enCasaTerror = 0;
			enShowAcuatico = 0;

			// ---------- Variables de control ----------

			// Se deja de tener la llave
			llaveTomada = 0;

			// El librero vuelve a estar de pie
			libreroDePie = 1;
			libreroSuena = 1;
			
			// La lámpara se apaga y la sirena vuelve a sonar
			enciendeLampara = 0;
			suenaSirena = 1;

			// El zombie vuelve a desaparecer
			apareceZombie = 0;

			// ---------- Cámara ----------

			// La cámara se ubica justo a la salida trasera de la casa del terror en la feria
			camera.Position.x = 93.10f;
			camera.Position.y = -4.20f;
			camera.Position.z = -129.87f;

			// ---------- Sonido ----------

			// Se reproduce el sonido de que se abre la puerta
			efectoDeSonido->setSoundVolume(2.0f);
			efectoDeSonido->play3D("Audio/PuertaAbriendose.mp3", vec3df(0, 0, -1));

			// Se cambia la música de fondo 
			musicaDeFondo->stopAllSounds();
			musicaDeFondo->play3D("Audio/MusicaNoche.mp3", vec3df(0, 0, -1));

			// ---------- Iluminación ----------

			// Se seleccionan los shaders a utilizar
			strcpy_s(Light_VertxShader, "shaders/shader_texture_light_dir.vs");
			strcpy_s(Light_FragShader, "shaders/shader_texture_light_dir.fs");

			// Posición y dirección de la Luz
			Light_Direction = lightDirection = glm::vec3(-1.0f, -1.0f, 0.0f);
			Light_Position = lightPosition = glm::vec3(-160.0f, 100.0f, 30.0f);

			Luz_Reflector = 0;   //La luz ya no es de tipo reflector
		}

		FPresionado = 1; //La tecla está presionada
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
		FPresionado = 0;


	// --------------- Animación por keyframes ---------------

	// Movimiento (Traslación)
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		posCarroX  -= 0.1f;
		posZapdosX -= 0.1f;

		posCarroAzulX -= 0.1f;
		posCarroBlancoX -= 0.1f;
		posCarroRojoX -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		posCarroX  += 0.1f;
		posZapdosX += 0.1f;

		posCarroAzulX += 0.1f;
		posCarroBlancoX += 0.1f;
		posCarroRojoX += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		posCarroY -= 0.1f;
		posZapdosY -= 0.1f;

		posCarroAzulZ += 0.1f;
		posCarroBlancoZ += 0.1f;
		posCarroRojoZ += 0.1f;
	}	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		posCarroY += 0.1f;
		posZapdosY += 0.1f;

		posCarroAzulZ -= 0.1f;
		posCarroBlancoZ -= 0.1f;
		posCarroRojoZ -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		posZapdosZ -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS)
	{
		posZapdosZ += 0.1f;
	}

	// Movimiento (Rotación)
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		rotCarro--;
		rotZapdos--;

		rotCarroAzul--;
		rotCarroBlanco--;
		rotCarroRojo--;
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		rotCarro++;
		rotZapdos++;

		rotCarroAzul++;
		rotCarroBlanco++;
		rotCarroRojo++;
	}

	// Para iniciar la animación de la montaña rusa
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && (PPresionado == 0))
	{
		if (playCarro == false && (frameIndexCarro > 1))
		{
			//resetElements(keyFrameCarro, posCarroX, posCarroY, posCarroZ, rotCarro);

			//Se resetean los elementos
			posCarroX = keyFrameCarro[0].posX;
			posCarroY = keyFrameCarro[0].posY;
			posCarroZ = keyFrameCarro[0].posZ;
			rotCarro = keyFrameCarro[0].rot;

			interpolation(keyFrameCarro, playIndexCarro, i_max_steps_Carro); //Primer Interpolation	

			playCarro = true;
			playIndexCarro = 0;
			i_curr_steps_Carro = 0;
		}
		else
			playCarro = false;

		PPresionado = 1;   //La tecla ya está presionada
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
		PPresionado = 0;

	// Para guardar el keyframe del carro de la montaña rusa
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && (F1Presionado == 0))
	{
		if (frameIndexCarro < MAX_FRAMES)
			saveFrame(keyFrameCarro, frameIndexCarroPtr, posCarroX, posCarroY, posCarroZ, rotCarro, "KeyFramesCarro.txt");

		F1Presionado = 1;   //La tecla ya está presionada
	}
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
		F1Presionado = 0;


	// Para iniciar la animación del zapdos (No se puede realizar en modo noche)
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && (OPresionado == 0) && (!modoNoche))
	{
		if (playZapdos == false && (frameIndexZapdos > 1))
		{
			//resetElements(keyFrameZapdos, posZapdosX, posZapdosY, posZapdosZ, rotZapdos);}

			//Se resetean los elementos
			posZapdosX = keyFrameZapdos[0].posX;
			posZapdosY = keyFrameZapdos[0].posY;
			posZapdosZ = keyFrameZapdos[0].posZ;
			rotZapdos = keyFrameZapdos[0].rot;

			interpolation(keyFrameZapdos, playIndexZapdos, i_max_steps_Zapdos); //Primer Interpolation	

			playZapdos= true;
			playIndexZapdos = 0;
			i_curr_steps_Zapdos = 0;
		}
		else
			playZapdos = false;

		OPresionado = 1;   //La tecla ya está presionada


		// --------- Se cambia la música de fondo ---------

		// Se pone el volumen de la música de fondo al mínimo
		musicaDeFondo->setSoundVolume(0);

		// Si la canción de batalla pokémon no se está reproduciendo, se reproduce
		if (!efectoDeSonido->isCurrentlyPlaying("Audio/BatallaPokemon.mp3"))
		{
			efectoDeSonido->setSoundVolume(0.5f);
			efectoDeSonido->play3D("Audio/BatallaPokemon.mp3", vec3df(0, 0, -1));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
		OPresionado = 0;

	// Para guardar el keyframe del zapdos
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && (F2Presionado == 0))
	{
		if (frameIndexZapdos < MAX_FRAMES)
			saveFrame(keyFrameZapdos, frameIndexZapdosPtr, posZapdosX, posZapdosY, posZapdosZ, rotZapdos, "KeyFramesZapdos.txt");

		F2Presionado = 1;   //La tecla ya está presionada
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE)
		F2Presionado = 0;


	// Para iniciar la animación de los carritos chocones
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && (IPresionado == 0))
	{
		IPresionado = 1;   //La tecla ya está presionada

		// Carro Azul
		if (playCarroAzul == false && (frameIndexCarroAzul > 1))
		{
			//Se resetean los elementos
			posCarroAzulX = keyFrameCarroAzul[0].posX;
			posCarroAzulY = keyFrameCarroAzul[0].posY;
			posCarroAzulZ = keyFrameCarroAzul[0].posZ;
			rotCarroAzul = keyFrameCarroAzul[0].rot;

			interpolation(keyFrameCarroAzul, playIndexCarroAzul, i_max_steps_CarroAzul); //Primer Interpolation	

			playCarroAzul = true;
			playIndexCarroAzul = 0;
			i_curr_steps_CarroAzul = 0;
		}
		else
			playCarroAzul = false;

		// Carro Blanco
		if (playCarroBlanco == false && (frameIndexCarroBlanco > 1))
		{
			//Se resetean los elementos
			posCarroBlancoX = keyFrameCarroBlanco[0].posX;
			posCarroBlancoY = keyFrameCarroBlanco[0].posY;
			posCarroBlancoZ = keyFrameCarroBlanco[0].posZ;
			rotCarroBlanco = keyFrameCarroBlanco[0].rot;

			interpolation(keyFrameCarroBlanco, playIndexCarroBlanco, i_max_steps_CarroBlanco); //Primer Interpolation	

			playCarroBlanco = true;
			playIndexCarroBlanco = 0;
			i_curr_steps_CarroBlanco = 0;
		}
		else
			playCarroBlanco = false;

		// Carro Rojo
		if (playCarroRojo == false && (frameIndexCarroRojo > 1))
		{
			//Se resetean los elementos
			posCarroRojoX = keyFrameCarroRojo[0].posX;
			posCarroRojoY = keyFrameCarroRojo[0].posY;
			posCarroRojoZ = keyFrameCarroRojo[0].posZ;
			rotCarroRojo = keyFrameCarroRojo[0].rot;

			interpolation(keyFrameCarroRojo, playIndexCarroRojo, i_max_steps_CarroRojo); //Primer Interpolation	

			playCarroRojo = true;
			playIndexCarroRojo = 0;
			i_curr_steps_CarroRojo = 0;
		}
		else
			playCarroRojo = false;

	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE)
		IPresionado = 0;

	// Para guardar el keyframe del Carro Azul
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && (F3Presionado == 0))
	{
		if (frameIndexCarroAzul < MAX_FRAMES)
			saveFrame(keyFrameCarroAzul, frameIndexCarroAzulPtr, posCarroAzulX, posCarroAzulY, posCarroAzulZ, rotCarroAzul, "KeyFramesCarroAzul.txt");

		F3Presionado = 1;   //La tecla ya está presionada
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE)
		F3Presionado = 0;

	// Para guardar el keyframe del Carro Blanco
	if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS && (F4Presionado == 0))
	{
		if (frameIndexCarroBlanco < MAX_FRAMES)
			saveFrame(keyFrameCarroBlanco, frameIndexCarroBlancoPtr, posCarroBlancoX, posCarroBlancoY, posCarroBlancoZ, rotCarroBlanco, "KeyFramesCarroBlanco.txt");

		F4Presionado = 1;   //La tecla ya está presionada
	}
	if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_RELEASE)
		F4Presionado = 0;

	// Para guardar el keyframe del Carro Rojo
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && (F5Presionado == 0))
	{
		if (frameIndexCarroRojo < MAX_FRAMES)
			saveFrame(keyFrameCarroRojo, frameIndexCarroRojoPtr, posCarroRojoX, posCarroRojoY, posCarroRojoZ, rotCarroRojo, "KeyFramesCarroRojo.txt");

		F5Presionado = 1;   //La tecla ya está presionada
	}
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE)
		F5Presionado = 0;


	// --------------- Tipo de Iluminación ---------------

	// Luz Direccional - Modo Día
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && enFeria)
	{
		// Se seleccionan los shaders a utilizar
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_dir.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_dir.fs");

		// Posición y dirección de la Luz
		Light_Direction = lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
		Light_Position = lightPosition = glm::vec3(-20.0f, 115.0f, 30.0f);

		Luz_Reflector = 0;   //La luz ya no es de tipo reflector

		//Se cambia el color de fondo (Blanco)
		bgColorRed = 1.0f;
		bgColorGreen = 1.0f;
		bgColorBlue = 1.0f;

		//Se cambia el color de la luz (Blanco)
		R = 1.0f;
		G = 1.0f;
		B = 1.0f;

		modoDia = 1;         //Modo dia On
		modoAtardecer = 0;   //Modo atardecer Off
		modoNoche = 0;       //Modo noche Off


		// --------- Se cambia la música de fondo ---------

		// Si la canción actual es diferente a la nueva canción, ésta se cambia
		if (strcmp(cancionDeFondo, "Audio/MusicaRelajantePokemon.mp3") != 0)
		{
			cancionDeFondo = "Audio/MusicaRelajantePokemon.mp3";

			musicaDeFondo->stopAllSounds();
			musicaDeFondo->play3D(cancionDeFondo, vec3df(0, 0, -1), true);
		}
	}

	// Luz Direccional - Modo Atardecer
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && enFeria)
	{
		// Se seleccionan los shaders a utilizar
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_dir.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_dir.fs");

		// Posición y dirección de la Luz
		Light_Direction = lightDirection = glm::vec3(-1.0f, -1.0f, 0.0f);
		Light_Position = lightPosition = glm::vec3(120.0f, 100.0f, 30.0f);

		Luz_Reflector = 0;  //La luz ya no es de tipo reflector

		//Se cambia el color de fondo (Blanco)
		bgColorRed = 1.0f;
		bgColorGreen = 1.0f;
		bgColorBlue = 1.0f;

		//Se cambia el color de la luz (Naranja)
		R = 1.00000f;
		G = 0.37254f;
		B = 0.00000f;

		modoDia = 0;         //Modo dia Off
		modoAtardecer = 1;   //Modo atardecer On
		modoNoche = 0;       //Modo noche Off


		// --------- Se cambia la música de fondo ---------

		// Si la canción actual es diferente a la nueva canción, ésta se cambia
		if (strcmp(cancionDeFondo, "Audio/LifeIsBeatiful.mp3") != 0)
		{
			cancionDeFondo = "Audio/LifeIsBeatiful.mp3";

			musicaDeFondo->stopAllSounds();
			musicaDeFondo->play3D(cancionDeFondo, vec3df(0, 0, -1), true);
		}
	}

	// Luz Direccional - Modo Noche
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && enFeria )
	{
		// Se seleccionan los shaders a utilizar
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_dir.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_dir.fs");

		// Posición y dirección de la Luz
		Light_Direction = lightDirection = glm::vec3(-1.0f, -1.0f, 0.0f);
		Light_Position = lightPosition = glm::vec3(-160.0f, 100.0f, 30.0f);

		Luz_Reflector = 0;   //La luz ya no es de tipo reflector

		//Se cambia el color de fondo (Negro)
		bgColorRed = 0.0f;
		bgColorGreen = 0.0f;
		bgColorBlue = 0.0f;

		//Se cambia el color de la luz (Amarillo Claro)
		R = 0.92549f;
		G = 0.92941f;
		B = 0.67843f;

		modoDia = 0;         //Modo dia Off
		modoAtardecer = 0;   //Modo atardecer Off
		modoNoche = 1;       //Modo noche On


		// --------- Se cambia la música de fondo ---------

		// Si la canción actual es diferente a la nueva canción, ésta se cambia
		if (strcmp(cancionDeFondo, "Audio/MusicaNoche.mp3") != 0)
		{
			cancionDeFondo = "Audio/MusicaNoche.mp3";
			
			musicaDeFondo->stopAllSounds();
			musicaDeFondo->play3D(cancionDeFondo, vec3df(0, 0, -1), true);
		}
	}

	/*
	// Luz Posicional - Modo día
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_pos.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_pos.fs");

		Light_Position = lightPosition = glm::vec3(-20.0f, 85.0f, 30.0f);

		Luz_Reflector = 0;  //La luz ya no es de tipo reflector

		//Se cambia el color de fondo (Blanco)
		bgColorRed   = 1.0f;
		bgColorGreen = 1.0f;
		bgColorBlue  = 1.0f;

		//Se cambia el color de la luz (Blanco)
		R = 1.0f;
		G = 1.0f;
		B = 1.0f;

		modoDia = 1;         //Modo dia On
		modoAtardecer = 0;   //Modo atardecer Off
		modoNoche = 0;       //Modo noche Off
	}

	// Luz Posicional - Modo atardecer
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_pos.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_pos.fs");

		Light_Position = lightPosition = glm::vec3(120.0f, 90.0f, 30.0f);

		Luz_Reflector = 0;  //La luz ya no es de tipo reflector

		//Se cambia el color de fondo (Blanco)
		bgColorRed = 1.0f;
		bgColorGreen = 1.0f;
		bgColorBlue = 1.0f;

		//Se cambia el color de la luz
		R = 1.00000f;
		G = 0.53333f;
		B = 0.06667f;

		modoDia = 0;         //Modo dia Off
		modoAtardecer = 1;   //Modo atardecer On
		modoNoche = 0;       //Modo noche Off
	}

	// Luz Posicional - Modo noche
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_pos.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_pos.fs");

		Light_Position = lightPosition = glm::vec3(-160.0f, 90.0f, 30.0f);

		Luz_Reflector = 0;  //La luz ya no es de tipo reflector

		//Se cambia el color de fondo (Negro)
		bgColorRed = 0.0f;
		bgColorGreen = 0.0f;
		bgColorBlue = 0.0f;

		//Se cambia el color de la luz
		R = 0.92549f;
		G = 0.92941f;
		B = 0.67843f;

		modoDia = 0;         //Modo dia Off
		modoAtardecer = 0;   //Modo atardecer Off
		modoNoche = 1;       //Modo noche On
	}

	// Luz de Reflector
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_spot.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_spot.fs");

		Luz_Reflector = 1;  //La luz es de tipo reflector

		//Light_Position = glm::vec3(20.0f, 10.0f, 0.0f);

		//Se cambia el color de fondo (Noche)
		bgColorRed   = 0.0f;
		bgColorGreen = 0.0f;
		bgColorBlue  = 0.0f;

		modoNoche = 1;   //Modo noche on
	}

	// Luz Direccional
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		strcpy_s(Light_VertxShader, "shaders/shader_texture_light_dir.vs");
		strcpy_s(Light_FragShader, "shaders/shader_texture_light_dir.fs");
		Light_Direction = lightDirection;

		Luz_Reflector = 0;   //La luz ya no es de tipo reflector

		//Se cambia el color de fondo (Día)
		bgColorRed   = 1.0f;
		bgColorGreen = 1.0f;
		bgColorBlue  = 1.0f;

		modoNoche = 0;   //Modo noche off
	}
	

	// Instructivo:
		Con las siguientes teclas se modifica el color de la luz :
		R: La luz se vuelve más roja
		G : La luz se vuelve más verde
		B : La luz se vuelve más azul

		H : La luz se vuelve completamente blanca
		T : La luz se apaga

		Con las siguientes teclas se modifica la intensidad de la luz
		9 : Disminuye la instensidad
		0 : Aumenta la instensidad

		---> NOTA IMPORTANTE:
		Para modificar la intensidad de la luz y el color se debe presionar
		varias veces la tecla para ir aumentando, si la tecla se deja presionada,
		ésta se tomará como un solo incremento.

	// --------------- Intensidad de la Luz ---------------
	
	// Aumenta
	if ((glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) && (CeroPresionado == 0))
	{
		opacidad -= 1;
		CeroPresionado = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE)
		CeroPresionado = 0;

	// Disminuye
	if ((glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) && (NuevePresionado == 0))
	{
		opacidad += 1;
		NuevePresionado = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE)
		NuevePresionado = 0;


	// --------------- Color de la luz ---------------

	// Aumenta el Rojo
	if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (RPresionado == 0))
	{
		R += 0.1f;
		RPresionado = 1;
		
		if (R > 1.1f)
			R = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		RPresionado = 0;

	// Aumenta el Verde
	if ((glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) && (GPresionado == 0))
	{
		G += 0.1f;
		GPresionado = 1;

		if (G > 1.1f)
			G = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
		GPresionado = 0;

	// Aumenta el Azul
	if ((glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) && (BPresionado == 0))
	{
		B += 0.1f;
		BPresionado = 1;

		if (B > 1.1f)
			B = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
		BPresionado = 0;

	// Luz Blanca
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		R = 1.0f;
		G = 1.0f;
		B = 1.0f;
	}

	// Luz Apagada
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		R = 0.0f;
		G = 0.0f;
		B = 0.0f;
	}
	*/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
    // Set the Viewport to the size of the created window
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset*3, yoffset*3);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


// ------------------> Implementación de funciones de Renderizado <------------------

void montañaRusa(glm::mat4 model_loc, Shader lightingShader, Shader lampShader, Shader modelShader, Model logo)
{
	glm::mat4 model;                   //Matriz para ir dibujando cada elemento
	glm::mat4 temp = glm::mat4(1.0f);  //Matriz auxiliar

	Shader shader("shaders/shader_lamp.vs", "shaders/shader_lamp.fs");   //Shader vacío para elegir entre cuál utilizar


	//  ------------- Rieles -------------

	// Modo de iluminación
	if (modoNoche)
	{
		lampShader.use();                               //Se activa este shader        
		lampShader.setVec3("color", 1.0f, 1.0f, 0.0f);  //Color de la estructura en modo noche  
		shader = lampShader;                            //Se selecciona este shader
	}
	else
	{
		lightingShader.use();                                                //Se activa este shader  
		lightingShader.setInt("material_diffuse", t_metal_amarillo);         //Texturas de la estructura en modo día
		lightingShader.setInt("material_specular", t_metal_amarillo_brillo);
		shader = lightingShader;                                             //Se selecciona este shader
	}

	// Riel 1
	model = glm::translate(model_loc, glm::vec3(19.75f, 16.5f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 2
	model = glm::translate(model_loc, glm::vec3(14.875f, 13.625f, -5.0f)); //Ubicación del Riel
	riel(model, 11.0f, 32.65f, shader);    //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 3
	model = glm::translate(model_loc, glm::vec3(10.0f, 10.75f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 4
	model = glm::translate(model_loc, glm::vec3(8.6f, 11.625f, -5.0f)); //Ubicación del Riel
	riel(model, 3.2f, -38.5f, shader);     //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 5
	model = glm::translate(model_loc, glm::vec3(7.2f, 12.5f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 6
	model = glm::translate(model_loc, glm::vec3(4.8f, 9.95f, -5.0f)); //Ubicación del Riel
	riel(model, 7.15f, 50.0f, shader);     //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 7
	model = glm::translate(model_loc, glm::vec3(2.4f, 7.4f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 8
	model = glm::translate(model_loc, glm::vec3(0.0f, 9.95f, -5.0f)); //Ubicación del Riel
	riel(model, 7.15f, -50.0f, shader);    //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 9
	model = glm::translate(model_loc, glm::vec3(-2.4f, 12.5f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 10
	model = glm::translate(model_loc, glm::vec3(-3.95f, 11.625f, -5.0f)); //Ubicación del Riel
	riel(model, 3.35f, 36.0f, shader);     //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 11
	model = glm::translate(model_loc, glm::vec3(-5.5f, 10.75f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 12
	model = glm::translate(model_loc, glm::vec3(-8.25f, 11.25, -5.0f)); //Ubicación del Riel
	riel(model, 4.85f, -12.40f, shader);   //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 13
	model = glm::translate(model_loc, glm::vec3(-11.0f, 11.75, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 14
	model = glm::translate(model_loc, glm::vec3(-12.80f, 9.35f, -5.0f)); //Ubicación del Riel
	riel(model, 6.3f, 56.7f, shader);      //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 15
	model = glm::translate(model_loc, glm::vec3(-14.45f, 6.3f, -5.0f)); //Ubicación del Riel
	riel(model, 1.5f, 90.0f, shader);      //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 16
	model = glm::translate(model_loc, glm::vec3(-11.35f, 2.90f, -5.0f)); //Ubicación del Riel
	riel(model, 8.90f, -43.65f, shader);   //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 17
	model = glm::translate(model_loc, glm::vec3(-8.0f, 0.0f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 18
	model = glm::translate(model_loc, glm::vec3(-2.8f, 2.7f, -5.0f)); //Ubicación del Riel
	riel(model, 11.35f, 29.0f, shader);    //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 19
	model = glm::translate(model_loc, glm::vec3(2.4f, 5.4f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z
	 
	// Riel 20
	model = glm::translate(model_loc, glm::vec3(6.2f, 3.825f, -5.0f)); //Ubicación del Riel
	riel(model, 7.75f, -25.0f, shader);    //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 21
	model = glm::translate(model_loc, glm::vec3(10.0f, 2.25f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 22
	model = glm::translate(model_loc, glm::vec3(13.25f, 4.25f, -5.0f)); //Ubicación del Riel
	riel(model, 7.40f, 34.50f, shader);    //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 23
	model = glm::translate(model_loc, glm::vec3(16.5f, 6.25f, -5.0f)); //Ubicación del Riel
	riel(model, 1.0f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 24 
	model = glm::translate(model_loc, glm::vec3(19.06f, 5.375f, -5.0f)); //Ubicación del Riel
	riel(model, 4.90f, -22.25f, shader);   //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 25
	model = glm::translate(model_loc, glm::vec3(21.0f, 1.125f, -5.0f)); //Ubicación del Riel
	riel(model, 7.5f, 90.0f, shader);      //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 26
	model = glm::translate(model_loc, glm::vec3(20.05f, -3.175f, -5.0f)); //Ubicación del Riel
	riel(model, 2.95f, 40.0f, shader);     //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 27
	model = glm::translate(model_loc, glm::vec3(13.5f, -4.0f, -5.0f)); //Ubicación del Riel
	riel(model, 11.5f, 0.0f, shader);      //Modelo de referencia, Escala en X, Rotación en Z

	//Riel 28 
	model = glm::translate(model_loc, glm::vec3(5.75f, -2.25f, -5.0f)); //Ubicación del Riel
	riel(model, 6.025f, -38.5f, shader);   //Modelo de referencia, Escala en X, Rotación en Z

	//Riel 29
	model = glm::translate(model_loc, glm::vec3(2.5f, -0.5f, -5.0f)); //Ubicación del Riel
	riel(model, 2.5f, 0.0f, shader);       //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 30
	model = glm::translate(model_loc, glm::vec3(-0.40f, -1.225f, -5.0f)); //Ubicación del Riel
	riel(model, 4.0f, 22.5f, shader);      //Modelo de referencia, Escala en X, Rotación en Z

	//Riel 31
	model = glm::translate(model_loc, glm::vec3(-1.95f, -3.5f, -5.0f)); //Ubicación del Riel
	riel(model, 4.0f, 90.0f, shader);      //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 32
	model = glm::translate(model_loc, glm::vec3(0.675f, -6.0f, -5.0f)); //Ubicación del Riel
	riel(model, 6.25f, -18.5f, shader);    //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 33
	model = glm::translate(model_loc, glm::vec3(12.5f, -7.0f, -5.0f)); //Ubicación del Riel
	riel(model, 18.0f, 0.0f, shader);      //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 34
	model = glm::translate(model_loc, glm::vec3(22.10f, -5.95f, -5.0f)); //Ubicación del Riel
	riel(model, 3.20f, 52.0f, shader);     //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 35
	model = glm::translate(model_loc, glm::vec3(23.0f, 4.5f, -5.0f)); //Ubicación del Riel
	riel(model, 19.0f, 90.0f, shader);     //Modelo de referencia, Escala en X, Rotación en Z

	// Riel 36
	model = glm::translate(model_loc, glm::vec3(21.55f, 15.15f, -5.0f)); //Ubicación del Riel
	riel(model, 4.425f, -42.5f, shader);   //Modelo de referencia, Escala en X, Rotación en Z


	lightingShader.use();

	// ------------- Carro -------------

	glm::mat4 ubicacion_carro = glm::mat4(1.0f);  //Variable auxiliar para saber la posición del carro en todo momento

	lightingShader.setInt("material_diffuse", t_azul);           //Texturas del chasis
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(model_loc, glm::vec3(19.75f, 17.5f, -5.0f));                //Se parte de Riel 1
	model = glm::translate(model, glm::vec3(posCarroX, posCarroY, posCarroZ));         //Se desplaza al keyframe correspondiente
	model = glm::rotate(model, glm::radians(rotCarro), glm::vec3(0.0f, 0.0f, 1.0f));   //Rotación en X
	ubicacion_carro = model;                                                           //Obtenemos las coordenadas del carro a lo largo del recorrido
	//model = glm::scale(model, glm::vec3(40.0f, 1.0f, 14.0f));                        //Escalado del carro
	carro(model, lightingShader);                                                      //Se manda a dibujar todo el carro


	// ------------ Sillas del Carro  ------------
	
	// Frontal Izquierda 1
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(-1.25f, 0.0f, 2.325f));           //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, -1, 1, lightingShader);                                                //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda

	// Frontal Izquierda 2
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(-1.25f, 0.0f, 3.35f));            //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, 0, 1, lightingShader);                                                 //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda
	
	// Frontal Derecha 1
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(-1.25f, 0.0f, -2.325f));          //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, 1, 1, lightingShader);                                                 //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda

	// Frontal Derecha 2
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(-1.25f, 0.0f, -3.35f));           //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, 0, 1, lightingShader);                                                 //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda
	
	// Trasera Izquierda 1
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(1.25f, 0.0f, 2.325f));            //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, -1, 1, lightingShader);                                                //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda

	// Trasera Izquierda 2
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(1.25f, 0.0f, 3.35f));             //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, 0, 1, lightingShader);                                                 //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda
	
	// Trasera Derecha 1
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(1.25f, 0.0f, -2.325f));           //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, 1, 1, lightingShader);                                                 //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda

	// Trasera Derecha 2
	lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la silla
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::translate(ubicacion_carro, glm::vec3(1.25f, 0.0f, -3.35f));            //Ubicación de la silla sobre el carro
	model = glm::rotate(model, glm::radians(giroSilla), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de giro
	model = glm::scale(model, glm::vec3(0.40f));                                        //Escalado de la silla
	silla(model, 0, 1, lightingShader);                                                 //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda


	//  ------------- Bases de los Rieles -------------

	lightingShader.setInt("material_diffuse", t_metal);           //Texturas de todas las bases
	lightingShader.setInt("material_specular", t_metal_brillo);

	// --- Base 1 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(-9.7f, -2.05f, 0.0f));
	model = glm::rotate(model, glm::radians(24.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 18.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(-9.7f, -2.05f, -10.0f));
	model = glm::rotate(model, glm::radians(24.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 18.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Superior
	model = glm::translate(model_loc, glm::vec3(-13.4f, 6.4f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 10.90f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(-13.4f, 6.4f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(-13.4f, 6.4f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Base 2 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(-5.5f, -0.5f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 20.25f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(-5.5f, -0.5f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 20.25f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Superior
	model = glm::translate(model_loc, glm::vec3(-5.5f, 9.75f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 10.90f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(-5.5f, 9.75f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(-5.5f, 9.75f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Horizontal Inferior
	model = glm::translate(model_loc, glm::vec3(-5.5f, 2.25f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 10.0f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(-5.5f, 2.25f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(-5.5f, 2.25f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Base 3 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(-1.25f, -2.05f, 0.0f));
	model = glm::rotate(model, glm::radians(-24.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 18.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(-1.25f, -2.05f, -10.0f));
	model = glm::rotate(model, glm::radians(-24.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 18.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Superior
	model = glm::translate(model_loc, glm::vec3(2.4f, 6.4f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 10.90f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(2.4f, 6.4f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(2.4f, 6.4f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Base 4 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(3.0f, -6.0, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 9.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(3.0f, -6.0f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 9.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Superior
	model = glm::translate(model_loc, glm::vec3(3.0f, -1.5f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 10.90f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(3.0f, -1.5f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(3.0f, -1.5f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Horizontal Inferior
	model = glm::translate(model_loc, glm::vec3(3.0f, -6.0f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 10.0f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(3.0f, -6.0f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(3.0f, -6.0f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Base 5 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(6.0f, -2.05f, 0.0f));
	model = glm::rotate(model, glm::radians(24.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 18.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(6.0f, -2.05f, -10.0f));
	model = glm::rotate(model, glm::radians(24.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 18.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Inferior
	model = glm::translate(model_loc, glm::vec3(6.90f, -4.25f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.75f, 10.0f, 0.75f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(6.90f, -4.25f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 0.25f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(6.90f, -4.25f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 0.25f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Base 6 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(10.0f, -0.5f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 20.25f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(10.0f, -0.5f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 20.25f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Superior
	model = glm::translate(model_loc, glm::vec3(10.0f, 9.75f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 10.90f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(10.0f, 9.75f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(10.0f, 9.75f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Horizontal Intermedio
	model = glm::translate(model_loc, glm::vec3(10.0f, 3.15f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 10.0f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(10.0f, 3.15f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(10.0f, 3.15f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Horizontal Inferior
	model = glm::translate(model_loc, glm::vec3(10.0f, -4.90f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 10.0f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(10.0f, -4.90f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(10.0f, -4.90f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Base 7 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(15.25f, 2.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-19.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 27.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(15.25f, 2.5f, -10.0f));
	model = glm::rotate(model, glm::radians(-19.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 27.0f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Superior
	model = glm::translate(model_loc, glm::vec3(19.7f, 15.5f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 10.90f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(19.7f, 15.5f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(19.7f, 15.5f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Horizontal Intermedio
	model = glm::translate(model_loc, glm::vec3(16.85f, 7.10f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.75f, 10.0f, 0.75f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(16.85f, 7.10f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 0.25f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(16.85f, 7.10f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 0.25f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Horizontal Inferior
	model = glm::translate(model_loc, glm::vec3(12.75f, -4.90f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 10.0f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(12.75f, -4.90f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(12.75f, -4.90f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Base 8 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(22.0f, -3.125f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 14.75f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Trasera
	model = glm::translate(model_loc, glm::vec3(22.0f, -3.125f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 14.75f, 1.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// Horizontal Superior
	model = glm::translate(model_loc, glm::vec3(22.0f, 4.25f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 10.90f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(22.0f, 4.25f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(22.0f, 4.25f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.30f, 0.25f, 1.30f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Horizontal Inferior
	model = glm::translate(model_loc, glm::vec3(22.0f, -2.25f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.90f, 10.0f, 0.90f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -> Anillo Frontal Izquierdo 
	model = glm::translate(model_loc, glm::vec3(22.0f, -2.25f, -1.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// ->  Anillo Frontal Derecho 
	model = glm::translate(model_loc, glm::vec3(22.0f, -2.25f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.10f, 0.25f, 1.10f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);


	//  ------------- Soportes de las Bases -------------

	lightingShader.setInt("material_diffuse", t_metal);           //Texturas de todas las bases
	lightingShader.setInt("material_specular", t_metal_brillo);

	// --- Soporte Base 1, 2 y 3 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(-5.5f, -10.25f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(3.0f, 0.5f, 2.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Trasero
	model = glm::translate(model_loc, glm::vec3(-5.5f, -10.25f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(3.0f, 0.5f, 2.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Soporte Base 4 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(3.0f, -10.25f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.5f, 0.5f, 1.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Trasero
	model = glm::translate(model_loc, glm::vec3(3.0f, -10.25f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.5f, 0.5f, 1.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Soporte Base 5, 6 y 7 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(10.25f, -10.25f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(3.5f, 0.5f, 3.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Trasero
	model = glm::translate(model_loc, glm::vec3(10.25f, -10.25f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(3.5f, 0.5f, 3.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Soporte Base 8 ---
	// Frontal
	model = glm::translate(model_loc, glm::vec3(22.0f, -10.25f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.5f, 0.5f, 1.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Trasero
	model = glm::translate(model_loc, glm::vec3(22.0f, -10.25f, -10.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.5f, 0.5f, 1.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);


	// ------------- Piso -------------

	lightingShader.setInt("material_diffuse", t_terracota);           //Textura del piso
	lightingShader.setInt("material_specular", t_terracota_brillo);

	model = glm::translate(model_loc, glm::vec3(4.5f, -11.0f, -5.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(40.0f, 1.0f, 16.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24);


	// ------------- Logo Batman -------------

	// Modo de iluminación
	if (modoNoche)
	{
		lampShader.use();                               //Se activa este shader        
		lampShader.setVec3("color", 1.0f, 1.0f, 0.0f);  //Color de la estructura en modo noche  
		shader = lampShader;                            //Se selecciona este shader
	}
	else
	{
		lightingShader.use();                                                //Se activa este shader  
		lightingShader.setInt("material_diffuse", t_metal_amarillo);         //Texturas de la estructura en modo día
		lightingShader.setInt("material_specular", t_metal_amarillo_brillo);
		shader = lightingShader;                                             //Se selecciona este shader
	}

	// Óvalo Amarrillo
	model = glm::translate(model_loc, glm::vec3(2.4f, 6.4f, 0.5f));
	model = glm::scale(model, glm::vec3(3.0f, 1.5f, 0.25f));
	shader.setMat4("model", model);
	esfera.render();

	// Modo de iluminación
	if (modoNoche)
	{
		lampShader.use();                               //Se activa este shader        
		lampShader.setVec3("color", 0.0f, 0.0f, 0.0f);  //Color de la estructura en modo noche  
		shader = lampShader;                            //Se selecciona este shader
	}
	else
	{
		lightingShader.use();                                        //Se activa este shader  
		lightingShader.setInt("material_diffuse", t_negro);          //Texturas de la estructura en modo día
		lightingShader.setInt("material_specular", t_negro_brillo);
		shader = lightingShader;                                     //Se selecciona este shader
	}

	// Óvalo Negro
	model = glm::translate(model_loc, glm::vec3(2.4f, 6.4f, 0.5f));
	model = glm::scale(model, glm::vec3(3.15f, 1.65f, 0.15f));
	shader.setMat4("model", model);
	esfera.render();

	// Batarang
	modelShader.use();
	model = glm::translate(model_loc, glm::vec3(2.4f, 6.4f, 0.75f));
	model = glm::scale(model, glm::vec3(17.0f));
	modelShader.setMat4("model", model);
	logo.Draw(modelShader);
}

void riel(glm::mat4 model_loc, float escala_x, float rotacion, Shader shader)
{
	glm::mat4 model;

	//Se aplica la rotación a todo el riel
	model_loc = glm::rotate(model_loc, glm::radians(rotacion), glm::vec3(0.0f, 0.0f, 1.0f));
	
	// ------------- Riel Frontal -------------

	// Centro
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(escala_x, 1.0f, 0.25f));
	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24);

	// Superior
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.5f, 1.0f));
	model = glm::scale(model, glm::vec3(escala_x, 0.05f, 0.5f));
	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24);

	//Inferior
	model = glm::translate(model_loc, glm::vec3(0.0f, -0.5f, 1.0f));
	model = glm::scale(model, glm::vec3(escala_x, 0.05f, 0.5f));
	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24);

	// ------------- Riel Posterior -------------
	// Centro
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(escala_x, 1.0f, 0.25f));
	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24);

	// Superior
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.5f, -1.0f));
	model = glm::scale(model, glm::vec3(escala_x, 0.05f, 0.5f));
	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24);

	//Inferior
	model = glm::translate(model_loc, glm::vec3(0.0f, -0.5f, -1.0f));
	model = glm::scale(model, glm::vec3(escala_x, 0.05f, 0.5f));
	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24);
}

void carro(glm::mat4 model_loc, Shader lightingShader)
{
	glm::mat4 model;

	// ------------- Chasis-------------

	// --- Tubo Central ---
	model = glm::rotate(model_loc, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 2.5f, 1.0f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// --- Tubo Frontal ---
	model = glm::translate(model_loc, glm::vec3(-1.25f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 3.25f, 1.0f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Anillo Frontal del Centro ---
	model = glm::translate(model_loc, glm::vec3(-1.25f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.25f, 0.25f, 1.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);
	
	// --- Anillo Frontal Izquierdo ---
	model = glm::translate(model_loc, glm::vec3(-1.25f, 0.0f, 1.35f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.25f, 0.25f, 1.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);
	
	// --- Anillo Frontal Derecho ---
	model = glm::translate(model_loc, glm::vec3(-1.25f, 0.0f, -1.35f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.25f, 0.25f, 1.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Unión Frontal Izquierda ---
	model = glm::translate(model_loc, glm::vec3(-0.85f, 0.0f, 0.30f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.95f, 1.0f, 0.95f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// --- Unión Frontal Derecha ---
	model = glm::translate(model_loc, glm::vec3(-0.85f, 0.0f, -0.30f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.95f, 1.0f, 0.95f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);


	// --- Tubo Posterior ---
	model = glm::translate(model_loc, glm::vec3(1.25f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 3.25f, 1.0f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Anillo Posterior del Centro ---
	model = glm::translate(model_loc, glm::vec3(1.25f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.25f, 0.25f, 1.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);
	
	// --- Anillo Posterior Izquierdo ---
	model = glm::translate(model_loc, glm::vec3(1.25f, 0.0f, 1.35f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.25f, 0.25f, 1.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);
	
	// --- Anillo Posterior Derecho ---
	model = glm::translate(model_loc, glm::vec3(1.25f, 0.0f, -1.35f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.25f, 0.25f, 1.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Unión Posterior Izquierda ---
	model = glm::translate(model_loc, glm::vec3(0.85f, 0.0f, 0.30f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.95f, 1.0f, 0.95f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);

	// --- Unión Posterior Derecha ---
	model = glm::translate(model_loc, glm::vec3(0.85f, 0.0f, -0.30f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.95f, 1.0f, 0.95f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
}

void silla(glm::mat4 model_loc, int lado, int seguridad, Shader lightingShader)
{
	glm::mat4 model;

	model_loc = glm::rotate(model_loc, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));;

	// ------------- Asiento -------------
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.5f, 5.0f, 5.0f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 96, 40);      //Asiento
	glDrawArrays(GL_POLYGON, 136, 10);   //Costado Derecho
	glDrawArrays(GL_POLYGON, 146, 10);   //Costado Izquierdo


	// ------------- Guarda ----------------
	if (lado == 1 || lado == -1)
	{
		model = glm::translate(model_loc, glm::vec3(lado * 1.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.45f, 5.0f, 5.0f));

		lightingShader.setMat4("model", model);
		lightingShader.setInt("material_diffuse", t_metal);
		lightingShader.setInt("material_specular", t_metal_brillo);
		glDrawArrays(GL_QUADS, 156, 20);    //Contorno
		glDrawArrays(GL_POLYGON, 176, 5);   //Costado Derecho
		glDrawArrays(GL_POLYGON, 181, 5);   //Costado Izquierdo
	}


	// ---------------- Tubos de Seguridad ----------------
	
	if (seguridad != 0)
	{
		lightingShader.setInt("material_diffuse", t_metal_amarillo);           //Texturas de los tubos
		lightingShader.setInt("material_specular", t_metal_amarillo_brillo);

		// --- Superior Derecho ---
		model = glm::translate(model_loc, glm::vec3(1.0f, 1.5f, -1.125f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.25f, 0.25f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);

		// --- Superior Derecho Inclinado ---
		model = glm::translate(model_loc, glm::vec3(0.82f, 1.290f, -0.375f));
		model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.23f, 0.75f, 0.23f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);

		// --- Frontal Derecho ---
		model = glm::translate(model_loc, glm::vec3(0.630f, 0.625f, -0.175f));
		model = glm::scale(model, glm::vec3(0.25f, 1.0f, 0.25f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);

		// --- Inferior Derecho Inclinado ---
		model = glm::translate(model_loc, glm::vec3(0.63f, -0.61f, -0.975f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.23f, 2.30f, 0.23f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);

		// --- Superior Izquierdo ---
		model = glm::translate(model_loc, glm::vec3(-1.0f, 1.5f, -1.125f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 1.25f, 0.25f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);

		// --- Superior Izquierdo Inclinado ---
		model = glm::translate(model_loc, glm::vec3(-0.82f, 1.290f, -0.375f));
		model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.23f, 0.75f, 0.23f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);

		// --- Frontal Izquierdo ---
		model = glm::translate(model_loc, glm::vec3(-0.630f, 0.625f, -0.175f));
		model = glm::scale(model, glm::vec3(0.25f, 1.0f, 0.25f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);

		// --- Inferior Izquierdo Inclinado ---
		model = glm::translate(model_loc, glm::vec3(-0.63f, -0.61f, -0.975f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.23f, 2.30f, 0.23f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_QUADS, 24, 48);
		glDrawArrays(GL_POLYGON, 72, 12);
		glDrawArrays(GL_POLYGON, 84, 12);
	}
}

void ruedaDeLaFortuna(glm::mat4 model_loc, Shader lightingShader, Shader lampShader)
{
	glm::mat4 model;  //Matriz para ir dibujando cada elemento

	//  ------------- Estructura Circular -------------
	
	Shader shader("shaders/shader_lamp.vs", "shaders/shader_lamp.fs");   //Shader vacío para elegir entre cuál utilizar

	// Modo de iluminación
	if (modoNoche)
	{
		lampShader.use();                               //Se activa este shader        
		lampShader.setVec3("color", 0.0f, 0.0f, 1.0f);  //Color de la estructura en modo noche  
		shader = lampShader;                            //Se selecciona este shader
	}
	else
	{
		lightingShader.use();                                      //Se activa este shader  
		lightingShader.setInt("material_diffuse", t_azul);         //Texturas de la estructura en modo día
		lightingShader.setInt("material_specular", t_blanco_brillo);
		shader = lightingShader;                                   //Se selecciona este shader
	}

	// --- Rueda Frontal ---
	
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));                     //Ubicación del anillo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                             //Escala del anillo
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	anillo(model, shader);                                                              //Se dibuja el anillo
	
	// --- Rueda Trasera ---

	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));                    //Ubicación del anillo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                             //Escala del anillo
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	anillo(model, shader);                                                              //Se dibuja el anillo


	//  ------------- Soportes de los Anillos -------------

	// Modo de iluminación
	if (modoNoche)
	{
		lampShader.use();                                //Se activa este shader  
		lampShader.setVec3("color", 1.0f, 1.0f, 1.0f);   //Color de la estructura en modo noche  
		shader = lampShader;                             //Se selecciona este shader
	}
	else
	{
		lightingShader.use();                                         //Se activa este shader  
		lightingShader.setInt("material_diffuse", t_blanco);          //Texturas de la estructura en modo día
		lightingShader.setInt("material_specular", t_blanco_brillo);
		shader = lightingShader;                                      //Se selecciona este shader
	}

	// --- Soportes Frontales ---
	// 1
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));
	
	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 2
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(22.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 3
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 4
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(67.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f)); 

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 5
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 6
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(112.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 7
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 8
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(157.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f)); 

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Soportes Traseros ---
	// 1
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 2
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(22.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 3
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 4
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(67.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 5
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 6
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(112.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 7
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));    //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 8
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(157.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));    //Animación de Giro
	model = glm::scale(model, glm::vec3(0.5f, 30.0f, 0.5f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);


	//  ------------- Eje de la Rueda -------------

	// Modo de iluminación
	if (modoNoche)
	{
		lampShader.use();                                //Se activa este shader  
		lampShader.setVec3("color", 0.0f, 0.0f, 1.0f);   //Color de la estructura en modo noche  
		shader = lampShader;                             //Se selecciona este shader
	}
	else
	{
		lightingShader.use();                                       //Se activa este shader  
		lightingShader.setInt("material_diffuse", t_azul);          //Texturas de la estructura en modo día
		lightingShader.setInt("material_specular", t_blanco_brillo);
		shader = lightingShader;                                      //Se selecciona este shader
	}

	// Tubo central
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -3.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 1.0f, .0f));  //Animación de Giro
	model = glm::scale(model, glm::vec3(1.25f, 10.0f, 1.25f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Anillo Frontal
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 1.0f, 0.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Anillo Trasero
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.0f, -7.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(giroRueda), glm::vec3(0.0f, 1.0f, 0.0f));   //Animación de Giro
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);


	//  ------------- Soportes de la Base -------------

	// Modo de iluminación
	if (modoNoche)
	{
		lampShader.use();                                //Se activa este shader  
		lampShader.setVec3("color", 0.0f, 0.0f, 1.0f);   //Color de la estructura en modo noche  
		shader = lampShader;                             //Se selecciona este shader
	}
	else
	{
		lightingShader.use();                                       //Se activa este shader  
		lightingShader.setInt("material_diffuse", t_azul);          //Texturas de la estructura en modo día
		lightingShader.setInt("material_specular", t_blanco_brillo);
		shader = lightingShader;                                      //Se selecciona este shader
	}

	// -- Soportes Frontales ---

	// Soporte Izquierdo
	model = glm::translate(model_loc, glm::vec3(-6.0f, -10.5f, 1.0f));
	model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 24.0f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Soporte Derecho
	model = glm::translate(model_loc, glm::vec3(6.0f, -10.5f, 1.0f));
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 24.0f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	//Unión Soporte-Piso Izquierdo
	model = glm::translate(model_loc, glm::vec3(-11.75f, -20.75f, 1.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 0.5f, 2.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	//Unión Soporte-Piso Derecho
	model = glm::translate(model_loc, glm::vec3(11.75f, -20.75f, 1.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 0.5f, 2.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// -- Soportes Traseros ---

	// Soporte Izquierdo
	model = glm::translate(model_loc, glm::vec3(-6.0f, -10.5f, -8.0f));
	model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 24.0f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Soporte Derecho
	model = glm::translate(model_loc, glm::vec3(6.0f, -10.5f, -8.0f));
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 24.0f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	//Unión Soporte-Piso Izquierdo
	model = glm::translate(model_loc, glm::vec3(-11.75f, -20.75f, -8.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 0.5f, 2.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	//Unión Soporte-Piso Derecho
	model = glm::translate(model_loc, glm::vec3(11.75f, -20.75f, -8.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 0.5f, 2.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);


	// ------------- Piso -------------

	lightingShader.use();

	model = glm::translate(model_loc, glm::vec3(0.0f, -21.5f, -3.5f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(40.0f, 1.0f, 16.0f));

	lightingShader.setMat4("model", model);
	lightingShader.setInt("material_diffuse", t_terracota);          //Textura del piso
	lightingShader.setInt("material_specular", t_terracota_brillo);
	glDrawArrays(GL_QUADS, 0, 24);


	//  ------------- Cúpulas-------------

	// 1
	lightingShader.setInt("material_diffuse", t_cafe);           //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::translate(model, glm::vec3(15.0f, 0.0f, -3.5f));                           //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));      //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                            //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                 //Escala de la cúpula
	cupula(model, lightingShader);                                                          //Se dibuja la cúpula

	// 2
	lightingShader.setInt("material_diffuse", t_morado);          //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));   //Animación de Giro
	model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -3.5f));                          //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));      //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                            //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                 //Escala de la cúpula
	cupula(model, lightingShader);                                                          //Se dibuja la cúpula

	// 3
	lightingShader.setInt("material_diffuse", t_azul_rey);       //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_azul_rey);

	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));  //Animación de Giro
	model = glm::translate(model, glm::vec3(0.0f, 15.0f, -3.5f));                          //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));     //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                           //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                //Escala de la cúpula
	cupula(model, lightingShader);                                                         //Se dibuja la cúpula

	// 4
	lightingShader.setInt("material_diffuse", t_rojo);          //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));  //Animación de Giro
	model = glm::translate(model, glm::vec3(0.0f, -15.0f, -3.5f));                         //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));     //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                           //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                //Escala de la cúpula
	cupula(model, lightingShader);                                                         //Se dibuja la cúpula

	// 5
	lightingShader.setInt("material_diffuse", t_rosa);          //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));  //Animación de Giro
	model = glm::translate(model, glm::vec3(10.675f, 10.675f, -3.5f));                     //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));     //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                           //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                //Escala de la cúpula
	cupula(model, lightingShader);                                                         //Se dibuja la cúpula

	// 6
	lightingShader.setInt("material_diffuse", t_naranja);         //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));  //Animación de Giro
	model = glm::translate(model, glm::vec3(-10.675f, 10.675f, -3.5f));                    //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));     //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                           //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                //Escala de la cúpula
	cupula(model, lightingShader);                                                         //Se dibuja la cúpula

	// 7
	lightingShader.setInt("material_diffuse", t_amarillo);        //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_blanco_brillo);
	 
	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));  //Animación de Giro
	model = glm::translate(model, glm::vec3(10.675f, -10.675f, -3.5f));                    //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));     //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                           //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                //Escala de la cúpula
	cupula(model, lightingShader);                                                         //Se dibuja la cúpula

	// 8
	lightingShader.setInt("material_diffuse", t_verde);          //Texturas de la cúpula
	lightingShader.setInt("material_specular", t_blanco_brillo);

	model = glm::rotate(model_loc, glm::radians(giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));  //Animación de Giro
	model = glm::translate(model, glm::vec3(-10.675f, -10.675f, -3.5f));                   //Ubicación de la cúpula
	model = glm::rotate(model, glm::radians(-giroRueda), glm::vec3(0.0f, 0.0f, 1.0f));     //Rotación inversa para preservar la verticalidad
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));                           //Offset hacia abajo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));                                //Escala de la cúpula
	cupula(model, lightingShader);                                                         //Se dibuja la cúpula
}

void cupula(glm::mat4 model_loc, Shader lightingShader)
{
	glm::mat4 model;  //Matriz para ir dibujando cada elemento


	// ------------- Techo -------------
	// Barrera
	model = glm::translate(model_loc, glm::vec3(0.0f, 2.5f, 0.0f));
	model = glm::scale(model, glm::vec3(6.75f, 0.3f, 6.75f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Adorno 1
	model = glm::translate(model_loc, glm::vec3(0.0f, 2.625f, 0.0f));
	model = glm::scale(model, glm::vec3(4.5f, 0.5f, 4.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Adorno 2
	model = glm::translate(model_loc, glm::vec3(0.0f, 2.75f, 0.0f));
	model = glm::scale(model, glm::vec3(2.5f, 0.5f, 2.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);
	

	// ------------- Contorno -------------
	// Barrera
	model = glm::translate(model_loc, glm::vec3(0.0f, -0.625f, 0.0f));              //Ubicación del anillo
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));   //El anillo va acostado
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 1.75f));                        //Escala del anillo
	anillo(model, lightingShader);                                                  //Se dibuja el anillo

	// Borde
	model = glm::translate(model_loc, glm::vec3(0.0f, 0.05f, 0.0f));                //Ubicación del anillo
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));   //El anillo va acostado
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.225f));                       //Escala del anillo
	anillo(model, lightingShader);                                                  //Se dibuja el anillo


	//  ------------- Sillas -------------
	// Izquierda
	model = glm::translate(model_loc, glm::vec3(0.0f, -0.25f, 0.55f));    //Ubicación de la silla sobre la cúpula
	model = glm::scale(model, glm::vec3(0.425f));                         //Escalado de la silla
	silla(model, 0, 0, lightingShader);                                   //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda

	// Derecha
	model = glm::translate(model_loc, glm::vec3(0.0f, -0.25f, -0.55f));   //Ubicación de la silla sobre la cúpula
	model = glm::scale(model, glm::vec3(0.425f));                         //Escalado de la silla
	silla(model, 0, 0, lightingShader);                                   //Se dibuja la silla. -1: Guarda Derecha  1: Guarda Izquierda


	//  ------------- Base -------------
	// Piso
	model = glm::translate(model_loc, glm::vec3(0.0f, -1.25f, 0.0f));
	model = glm::scale(model, glm::vec3(6.75f, 0.3f, 6.75f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Adorno
	model = glm::translate(model_loc, glm::vec3(0.0f, -1.5f, 0.0f));
	model = glm::scale(model, glm::vec3(4.5f, 0.5f, 4.5f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);


	//  ------------- Uniones del Techo con la Base -------------

	lightingShader.setInt("material_diffuse", t_metal);          //Textura de las uniones
	lightingShader.setInt("material_specular", t_metal_brillo);

	// --- Barras Verticales ---
	
	// Unión Frontal Izquierda
	model = glm::translate(model_loc, glm::vec3(-1.25f, 1.25f, 2.75f));
	model = glm::scale(model, glm::vec3(0.15f, 2.5f, 0.15f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Unión Frontal Derecha
	model = glm::translate(model_loc, glm::vec3(-1.25f, 1.25f, -2.75f));
	model = glm::scale(model, glm::vec3(0.15f, 2.5f, 0.15f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Unión Trasera Izquierda
	model = glm::translate(model_loc, glm::vec3(1.25f, 1.25f, 2.75f));
	model = glm::scale(model, glm::vec3(0.15f, 2.5f, 0.15f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Unión Trasera Derecha
	model = glm::translate(model_loc, glm::vec3(1.25f, 1.25f, -2.75f));
	model = glm::scale(model, glm::vec3(0.15f, 2.5f, 0.15f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// --- Remaches ---

	// Remache Frontal Izquierdo
	model = glm::translate(model_loc, glm::vec3(-1.25f, 2.5f, 2.75f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.25f, 0.15f, 0.25f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Remache Frontal Derecho
	model = glm::translate(model_loc, glm::vec3(-1.25f, 2.5f, -2.75f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.25f, 0.15f, 0.25f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Remache Trasero Izquierdo
	model = glm::translate(model_loc, glm::vec3(1.25f, 2.5f, 2.75f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.25f, 0.15f, 0.25f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// Remache Trasero Derecho
	model = glm::translate(model_loc, glm::vec3(1.25f, 2.5f, -2.75f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(70.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.25f, 0.15f, 0.25f));

	lightingShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);	
}

void anillo(glm::mat4 model_loc, Shader shader)
{
	glm::mat4 model;  //Matriz para ir dibujando cada elemento

	// ------------ Partes del Anillo ------------
	// 1
	model = glm::translate(model_loc, glm::vec3(0.0f, 15.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 2
	model = glm::translate(model_loc, glm::vec3(0.0f, -15.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 3
	model = glm::translate(model_loc, glm::vec3(15.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 4
	model = glm::translate(model_loc, glm::vec3(-15.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 5
	model = glm::translate(model_loc, glm::vec3(10.675f, 10.675f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 5.90f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 6
	model = glm::translate(model_loc, glm::vec3(-10.675f, -10.675f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 5.90f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 7
	model = glm::translate(model_loc, glm::vec3(10.675f, -10.675f, 0.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 5.90f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 8
	model = glm::translate(model_loc, glm::vec3(-10.675f, 10.675f, 0.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 5.90f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 9
	model = glm::translate(model_loc, glm::vec3(5.85f, 13.85f, 0.0f));
	model = glm::rotate(model, glm::radians(67.75f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 10
	model = glm::translate(model_loc, glm::vec3(-5.85f, -13.85f, 0.0f));
	model = glm::rotate(model, glm::radians(67.75f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 11
	model = glm::translate(model_loc, glm::vec3(5.85f, -13.85f, 0.0f));
	model = glm::rotate(model, glm::radians(-67.75f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 12
	model = glm::translate(model_loc, glm::vec3(-5.85f, 13.85f, 0.0f));
	model = glm::rotate(model, glm::radians(-67.75f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 13
	model = glm::translate(model_loc, glm::vec3(13.85f, 5.85f, 0.0f));
	model = glm::rotate(model, glm::radians(22.25f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 14
	model = glm::translate(model_loc, glm::vec3(-13.85f, -5.85f, 0.0f));
	model = glm::rotate(model, glm::radians(22.25f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 15
	model = glm::translate(model_loc, glm::vec3(13.85f, -5.85f, 0.0f));
	model = glm::rotate(model, glm::radians(-22.25f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);

	// 16
	model = glm::translate(model_loc, glm::vec3(-13.85f, 5.85f, 0.0f));
	model = glm::rotate(model, glm::radians(-22.25f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1.0f, 6.25f, 1.0f));

	shader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 24, 48);
	glDrawArrays(GL_POLYGON, 72, 12);
	glDrawArrays(GL_POLYGON, 84, 12);
}

