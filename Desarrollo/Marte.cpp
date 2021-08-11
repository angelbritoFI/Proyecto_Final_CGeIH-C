/*
* Materia: Laboratorio de Computación Gráfica e Interacción Humano-Computadora
* Semestre 2021-2
* Fecha de creación: 12/07/2021
* Descripción: Archivo principal de la ejecución del Proyecto Final
* Autor:
*	Brito Segura Angel
* Colaboradores (hasta versión 1):
	Hernández Torres Agustín de Jesús
	Huarte Nolasco Mario
*/
#define STB_IMAGE_IMPLEMENTATION //para cargar imagen

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

// Implementación de la librería de audio
#include <irrklang/irrKlang.h>

#pragma comment(lib, "irrKlang.lib") // Para poder ocupar el irrKlang.dll, se requiere enlazarlo con irrKlang.lib.

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//Piso
Texture plainTexture;
Texture pisoTexture;

//Personajes
//Wall-E
Model Avatar_M; 
Model BrazoD_M;
Model BrazoI_M;
Model PieD_M;
Model PieI_M;

Model Eva_M;
Model Cuca_M;
Model FinnJake_M;
Model ClonO_M;
Model Yoda_M;
Model Droide1_M;
Model Droide2_M;
Model R2D2_M;

Model Curiosity_M;
Model Perseverance_M;
Model Rocas_M;
Model TieDV_M;
Model TieB_M;
Model TieD_M;

//Stormtroppers
Model CuerpoC_M;
Model BrazoCI_M;
Model BrazoCD_M;
Model PiernaCI_M;
Model PiernaCD_M;

//Naves
Model InterceptorJedi_M;
Model SpeederBike_M;
Model Pod_M;

Model Mont_M;
Model Camino_M;
Model Basura1_M;
Model Basura2_M;
Model Basura3_M;

//Cielo
Model DeathStar_M;
Model Luna_M;
Model Sol_M;
Model Estrellas_M;

//skybox dia noche
Skybox skybox;
Skybox skyboxNight;

//materiales objeto interactúe con la luz
Material Material_brillante; 
Material Material_opaco;

// 3 tipos de luz
//luz direccional (sol, ilumina todo el escenario por igual, debe de existir)
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS]; //luz puntual (ilumna el camino en la noche)
SpotLight spotLights[MAX_SPOT_LIGHTS]; //solo una parte como cono

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//Variables auxiliares tiempo
float tiempo_offset = 0.05;

// Variables para Animación de Wall-E
float posXrobot = 0.0f, posYrobot = 0.0f, posZrobot = 0.0f, posXspeeder = 0.0f, posYspeeder = 0.0f, posZspeeder = 0.0f;

float rotaHeli = 0.0f, rota = 0.0f;
int adelanteX = 1, adelanteY = 0, adelanteZ = 1, arriba = 0, abajo = 0;

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

// Crear el piso
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

//Cubos de basura
void CrearCubo()
{
	unsigned int cubo_indices[] = {
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


	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CreateShaders() {
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//----------------------------INICIO KEYFRAMES----------------------------------------

//------------------Variables------------------------

//_------------------------generales---------------------------------------------------
#define MAX_FRAMES 20		//maximo de frames
int i_max_steps = 300;		//tiempo de la animacion en pasos


//Definiendo una estructura FRAME con movimiento en X y Y
typedef struct _frame
{
	// Variables para guardar los Key Frames
	float X;		// Variable para PosicionX
	float Y;		// Variable para PosicionY
	float XPlus;	// Variable para IncrementoX
	float YPlus;	// Variable para IncrementoY
	float Z;		// Variable para PosicionZ
	float GX;		// Variable para Giro
	float GY;		// Variable para Giro
	float GZ;		// Variable para Giro
	float ZPlus;	// Variable para IncrementoZ
	float GXPlus;	// Variable para IncrementoG
	float GYPlus;	// Variable para IncrementoG
	float GZPlus;	// Variable para IncrementoG
}FRAME;

//---------------------------SOL-----------------------------------------------------
int i_curr_steps = 0;		//ira avanzando en la animacion
float SolX = 0, SolY = 0; //Variables auxiliares para mover el sol
FRAME KeyFrameSol[MAX_FRAMES]; //KEYFRAME DEL sol
int FrameIndexSol = 4;			//Maximo KeyFrame sol
bool playSol = true;			//Variable para detener o iniciar animacion de sol
int playIndexSol = 0;			//indice para ir entre frames

//_---------------------------luna---------------------------------------------------
float LunaX = 0, LunaY = 0; //Variables auxiliares para mover la luna
FRAME KeyFrameLuna[MAX_FRAMES]; //KEYFRAME DE LA LUNA
int FrameIndexLuna = 4;			//Maximo KeyFrame LuNA
bool playLuna = false;			//Variable para detener o iniciar animacion de Luna		
int playIndexLuna = 0;				//indice para ir entre frames

//-------------------------FUNCIONES KEYFRAMES  LUNA   -------------------------------------//

//Funcion para regresar a la Luna a su coordenada inicial
void resetElementsLuna(void){
	LunaX = 0;
	LunaY = 0;
}

//Funcion de Interpolacion general para Luna
void interpolationLUNA(int playIndex) {
	KeyFrameLuna[playIndex].XPlus = (KeyFrameLuna[playIndex + 1].X - KeyFrameLuna[playIndex].X) / i_max_steps;
	KeyFrameLuna[playIndex].YPlus = (KeyFrameLuna[playIndex + 1].Y - KeyFrameLuna[playIndex].Y) / i_max_steps;
}

//FUNCION QUE DEFINE LA ANIMACION DE LA LUNA 
void animaLuna(void)
{
	//Movimiento del objeto
	if (playLuna)
	{
		//primer interpolacion
		if (playIndexLuna == 0 and i_curr_steps == 0)
			interpolationLUNA(playIndexLuna);

		if (i_curr_steps >= i_max_steps) //fin de un frame
		{
			//le sumo uno al indice
			playIndexLuna++;
			if (playIndexLuna > FrameIndexLuna-1)//fin de la animacion 
			{
				playLuna = false;
				playSol = true;
				playIndexLuna = 0;
				resetElementsLuna();
				i_curr_steps = 0;
			}
			else //Siguiente frame
			{
				i_curr_steps = 0; //Reset 
				//Interpolation
				interpolationLUNA(playIndexLuna);
				
			}
		}
		else
		{
			//Animacion
			LunaX += KeyFrameLuna[playIndexLuna].XPlus;
			LunaY += KeyFrameLuna[playIndexLuna].YPlus;
			i_curr_steps++;
		}

	}
}

//-------------------------FUNCIONES KEYFRAMES SOL	 -------------------------------------//

//Funcion para regresar al sol a su coordenada inicial
void resetElementsSol(void) {
	SolX = 0;
	SolY = 0;
}

//Funcion de Interpolacion general para Sol
void interpolationSOL(int playIndex) {
	KeyFrameSol[playIndex].XPlus = (KeyFrameSol[playIndex + 1].X - KeyFrameSol[playIndex].X) / i_max_steps;
	KeyFrameSol[playIndex].YPlus = (KeyFrameSol[playIndex + 1].Y - KeyFrameSol[playIndex].Y) / i_max_steps;
}


//FUNCION QUE DEFINE LA ANiMACION DEl sol
void animaSol(void)
{
	//Movimiento del objeto
	if (playSol)
	{
		//primer interpolacion
		if(playIndexSol==0 and i_curr_steps==0)
			interpolationSOL(playIndexSol);

		if (i_curr_steps >= i_max_steps) //fin de un frame
		{
			//le sumo uno al indice
			playIndexSol++;
			if (playIndexSol > FrameIndexSol-1)//fin de la animacion 
			{
				playSol = false;
				playLuna = true;
				playIndexSol = 0;
				resetElementsSol();
				i_curr_steps = 0;
			}
			else //Siguiente frame
			{
				i_curr_steps = 0; //Reset 
				//Interpolation
				interpolationSOL(playIndexSol);

			}
		}
		else
		{
			//Animacion
			SolX += KeyFrameSol[playIndexSol].XPlus;
			SolY += KeyFrameSol[playIndexSol].YPlus;
			i_curr_steps++;
		}

	}
}




//------------------------INICIA EL MAIN-----------------------------------
int main() {
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CreateShaders();
	
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 1.0f, 0.5f);

	//Piso
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso_marte.jpg");
	pisoTexture.LoadTextureA();

	//Wall-E
	Avatar_M = Model();
	Avatar_M.LoadModel("Models/Wall-E.obj");
	BrazoI_M = Model();
	BrazoI_M.LoadModel("Models/Brazo-Derecho-Frente.obj");
	BrazoD_M = Model();
	BrazoD_M.LoadModel("Models/Brazo-Izquierdo-Frente.obj");
	PieD_M = Model();
	PieD_M.LoadModel("Models/Pie-Derecho.fbx");
	PieI_M = Model();
	PieI_M.LoadModel("Models/Pie-Izquierdo.fbx");

	Eva_M = Model();
	Eva_M.LoadModel("Models/EVA.obj");

	Cuca_M = Model();
	Cuca_M.LoadModel("Models/cucaracha.obj");

	ClonO_M = Model();
	ClonO_M.LoadModel("Models/Stormtrooper.obj");

	CuerpoC_M = Model();
	CuerpoC_M.LoadModel("Models/Stormtrooper-Cuerpo.fbx");

	BrazoCI_M = Model();
	BrazoCI_M.LoadModel("Models/Stormtrooper-Brazo-Izquierdo.fbx");

	BrazoCD_M = Model();
	BrazoCD_M.LoadModel("Models/Stormtrooper-Brazo-Derecho.fbx");

	PiernaCI_M = Model();
	PiernaCI_M.LoadModel("Models/Stormtrooper-Pierna-Izquierda.fbx");

	PiernaCD_M = Model();
	PiernaCD_M.LoadModel("Models/Stormtrooper-Pierna-Derecha.fbx");

	InterceptorJedi_M = Model();
	InterceptorJedi_M.LoadModel("Models/InterceptorJedi.fbx");

	SpeederBike_M = Model();
	SpeederBike_M.LoadModel("Models/SpeederBike.obj");

	Yoda_M = Model();
	Yoda_M.LoadModel("Models/yoda.obj");

	Droide1_M = Model();
	Droide1_M.LoadModel("Models/droide-batalla.fbx");

	Droide2_M = Model();
	Droide2_M.LoadModel("Models/droideSW.fbx");

	Pod_M = Model();
	Pod_M.LoadModel("Models/capsula-escape.fbx");

	R2D2_M = Model();
	R2D2_M.LoadModel("Models/r2d2.obj");

	FinnJake_M = Model();
	FinnJake_M.LoadModel("Models/Finn-Jake.obj");

	Mont_M = Model();
	Mont_M.LoadModel("Models/montania.fbx");

	Camino_M = Model();
	Camino_M.LoadModel("Models/camino.fbx");

	Basura1_M = Model();
	Basura1_M.LoadModel("Models/CuboBasura1.obj");

	Basura2_M = Model();
	Basura2_M.LoadModel("Models/CuboBasura2.obj");

	Basura3_M = Model();
	Basura3_M.LoadModel("Models/CuboBasura3.obj");

	DeathStar_M = Model();
	DeathStar_M.LoadModel("Models/DeathStar.obj");	

	Luna_M = Model();
	Luna_M.LoadModel("Models/Luna.obj");

	Sol_M = Model();
	Sol_M.LoadModel("Models/Sol.obj");

	Estrellas_M = Model();
	Estrellas_M.LoadModel("Models/Estrellas.obj");

	Curiosity_M = Model();
	//Curiosity_M.LoadModel("Models/curiosity.obj");

	Perseverance_M = Model();
	Perseverance_M.LoadModel("Models/perseverance.fbx");

	Rocas_M = Model();
	Rocas_M.LoadModel("Models/rocas.fbx");

	TieDV_M = Model();
	TieDV_M.LoadModel("Models/DarthVader-Tie.fbx");

	TieB_M = Model();
	TieB_M.LoadModel("Models/bombardero-tie.fbx");

	TieD_M = Model();
	TieD_M.LoadModel("Models/defensor-tie.obj");

	//Skybox dia
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/marte-dia_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/marte-dia_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/marte-dia_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/marte-dia_up.tga");
	skyboxFaces.push_back("Textures/Skybox/marte-dia_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/marte-dia_ft.tga");
	skybox = Skybox(skyboxFaces);

	//skybox noche
	std::vector<std::string> skyboxFacesNight;
	skyboxFacesNight.push_back("Textures/Skybox/marte-noche_lf.tga");
	skyboxFacesNight.push_back("Textures/Skybox/marte-noche_rt.tga");
	skyboxFacesNight.push_back("Textures/Skybox/marte-noche_dn.tga");
	skyboxFacesNight.push_back("Textures/Skybox/marte-noche_up.tga");
	skyboxFacesNight.push_back("Textures/Skybox/marte-noche_bk.tga");
	skyboxFacesNight.push_back("Textures/Skybox/marte-noche_ft.tga");
	skyboxNight = Skybox(skyboxFacesNight);

	// Se declaran los materiales de iluminación
	Material_brillante = Material(4.0f, 256); //Componente espectacular, reflejo (luz azul)
	Material_opaco = Material(0.3f, 4); //Si es muy pequeño no se refleja nada

	// Se declaran las luces
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, //de que color va ser la luz, color adicional que se le agregarán a los objetos
		0.3f, 0.3f, //coeficiente ambiental que tan intenso es la luz y coeficiente difuso que tan claros son los objetos
		0.0f, 0.0f, -1.0f); //vector de posisción en Z negativo para ver todo el rango

	//contador de luces puntuales (no se requiere si solo se tiene una sola luz)
	unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
	pointLights[0] = PointLight(0.0f, 0.0f, 0.0f, //sin color
		1.0f, 1.0f, //coeficientes
		2.0f, 1.5f, 1.5f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-100.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[2] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-100.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[3] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-80.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[4] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-80.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[5] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-60.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[6] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-60.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[7] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-40.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[8] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-40.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[9] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-20.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[10] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-20.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[11] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		0.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[12] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		0.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[13] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		20.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[14] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		20.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[15] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		40.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[16] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		40.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[17] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		60.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[18] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		60.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[19] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		80.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[20] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		80.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[21] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		100.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[22] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		100.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[23] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		120.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[24] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		120.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[25] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		140.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[26] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		140.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[27] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		160.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[28] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		160.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[29] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		180.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[30] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		180.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[31] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		200.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[32] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		200.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[33] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		220.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[34] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		220.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[35] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		240.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[36] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		240.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[37] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		260.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[38] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		260.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[39] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		280.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[40] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		280.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[41] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-120.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[42] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-120.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[43] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-140.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[44] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-140.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[45] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-160.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[46] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-160.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[47] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-180.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[48] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-180.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[49] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-200.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[50] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-200.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[51] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-220.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[52] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-220.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[53] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-240.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[54] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-240.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[55] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-260.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[56] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-260.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[57] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-280.0f, 1.5f, 25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	pointLights[58] = PointLight(1.0f, 1.0f, 1.0f, //color blanco
		1.0f, 1.0f, //coeficientes
		-280.0f, 1.5f, -25.0f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	unsigned int spotLightCount = 0;
	
	//linterna (luz ligada a la cámara)
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, //alcance
		5.0f); //apertura (radio) del cono entre más grande será más grande la circunferencia
	spotLightCount++;
	
	glm::vec3 colorLuzW;

	//Luz de Wall-E (apagada)
	spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f, //Sin mostrar
		1.0f, 1.0f,
		-1.0f, 8.5f, 0.1f,
		-0.05f, 0.0f, 0.0f, //ecuación de segundo grado
		0.5f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	//Variables auxiliares para el show de luces
	float luzFX1 = -60.0f;
	float luzFZ1 = 40.0f;
	float luzFX3 = -70.0f;
	float luzFZ3 = 50.0f;
	float luzFX2 = -50.0f;
	float luzFZ2 = 50.0f;
	glm::vec3 posicionLuzF1;
	glm::vec3 colorLuzF1;
	glm::vec3 posicionLuzF2;
	glm::vec3 colorLuzF2;
	glm::vec3 posicionLuzF3;
	glm::vec3 colorLuzF3;

	//Luces de Finn y Jake (apagadas)
	spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f, 
		0.0f, 2.0f, //coeficientes
		0.0f, 0.0f, 0.0f, //posición dentro del escenario
		0.0f, -1.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f, //alcance
		100.0f); //radio del cono
	spotLightCount++;

	spotLights[3] = SpotLight(0.0f, 0.0f, 0.0f, 
		0.0f, 2.0f, //coeficientes
		0.0f, 0.0f, 0.0f, //posición dentro del escenario
		0.0f, -1.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f, //alcance
		100.0f); //radio del cono
	spotLightCount++;

	spotLights[4] = SpotLight(0.0f, 0.0f, 0.0f, 
		0.0f, 2.0f, //coeficientes
		0.0f, 0.0f, 0.0f, //posición dentro del escenario
		0.0f, -1.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f, //alcance
		100.0f); //radio del cono
	spotLightCount++;

	//Variables auxiliares para luces de Yoda
	float luzYX2 = 50.0f;
	float luzYZ2 = -50.0f;
	float luzYX3 = 50.0f;
	float luzYZ3 = -50.0f;
	glm::vec3 colorLuzY1;
	glm::vec3 posicionLuzY2;
	glm::vec3 colorLuzY2;
	glm::vec3 posicionLuzY3;
	glm::vec3 colorLuzY3;

	//Luces de Yoda
	spotLights[5] = SpotLight(0.0f, 0.0f, 0.0f, //apagada
		0.0f, 2.0f, //coeficientes
		50.0f, 0.0f, -50.0f, //posición dentro del escenario
		0.0f, -1.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f, //alcance
		100.0f); //radio del cono
	spotLightCount++;

	spotLights[6] = SpotLight(0.0f, 0.0f, 0.0f, //apagada
		0.0f, 2.0f, //coeficientes
		0.0f, 0.0f, 0.0f, //posición dentro del escenario
		0.0f, -1.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f, //alcance
		100.0f); // radio del cono
	spotLightCount++;

	spotLights[7] = SpotLight(0.0f, 0.0f, 0.0f, //apagada
		0.0f, 2.0f, //coeficientes
		0.0f, 0.0f, 0.0f, //posición dentro del escenario
		0.0f, -1.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f, //alcance
		100.0f); // radio del cono
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	//Inicializar el audio
	irrklang::ISoundEngine* sonido = irrklang::createIrrKlangDevice();

	if (!sonido) {
		printf("Error al iniciar el audio\n");
		return -1;
	}
	
	sonido->play2D("media/air.mp3", true); //Reproducir sonido de fondo -Soundtrack-

	//Banderas para efectos de sonido
	bool reproduceW = true, reproduceS = true;

	float offsetHeli = 0.03, offsetPos = 0.01f, giroHelice = 0.0f;

	// KEY FRAMES LUNA	
	KeyFrameLuna[0].X = 0.0f;
	KeyFrameLuna[0].Y = 0.0f;

	KeyFrameLuna[1].X = 150.0f;
	KeyFrameLuna[1].Y = 200.0f;

	KeyFrameLuna[2].X = 300.0f;
	KeyFrameLuna[2].Y = 400.0f;

	KeyFrameLuna[3].X = 450.0f;
	KeyFrameLuna[3].Y = 200.0f;

	KeyFrameLuna[4].X = 600.0f;
	KeyFrameLuna[4].Y = 5.0f;
	GLfloat rotaL = 0.0f;

	// KEY FRAMES SOL	
	KeyFrameSol[0].X = 0.0f;
	KeyFrameSol[0].Y = 0.0f;

	KeyFrameSol[1].X = 150.0f;
	KeyFrameSol[1].Y = 200.0f;

	KeyFrameSol[2].X = 300.0f;
	KeyFrameSol[2].Y = 400.0f;

	KeyFrameSol[3].X = 450.0f;
	KeyFrameSol[3].Y = 200.0f;

	KeyFrameSol[4].X = 600.0f;
	KeyFrameSol[4].Y = 5.0f;

	GLfloat rotaSol = 0.0f;

	// Interceptor Jedi
	GLfloat offset = 0.0f;
	GLfloat angulo = 0.0f;
	float posXnave = -150.0f;
	float posYnave = 180.0f;
	GLfloat rotaDS = 0.0f;
	GLfloat rotacionN1 = 0.0f;
	GLfloat rotacionN2 = 0.0f;
	GLfloat posZnave, desaparece;
	bool baja = true;
	bool giraI = false;
	bool avanza = false;
	bool vuelta = false;
	bool irse = false;
	bool reproduceE = true;
	bool reproduceSW = true;

	//vistas
	bool flag = true;
	int giro=0;

	// Stormtrooper
	GLfloat rotacionDerecha = 0.0f;
	GLfloat rotacionIzquierda = 0.0f;
	GLfloat rotacionBrazoD = 0.0f;
	GLfloat rotacionBrazoI = 0.0f;
	GLfloat rotacionS1 = 0.0f;
	GLfloat rotacionS2 = 0.0f;
	float posXclon = -100.0f;
	float posZclon = -80.0f;
	float posYclon = 12.0;
	bool iniciaR = true;
	bool regresaR = false;
	bool avanzaS = true;
	bool vueltaS = false;
	bool regresa = false;
	bool vuelveOrigen = false;
	bool terminaS = false;
	bool subeBrazos = false;
	bool reproduceDV = true;

	//Eva
	GLfloat anguloE = 0.0f;
	float posYeva = 0.0f;
	float posXeva = 0.0f;
	float posZeva = 30.0f;
	GLfloat rotaEX = 0.0f;
	GLfloat rotaEY = -90.0f;
	GLfloat rotaEZ = 0.0f;
	bool iniciaE = true;
	bool giraE = false;
	bool enfrenteW = false;
	bool vueltaEW = false;
	bool regresaE = false;
	bool reproduceD = true;
	bool reproduceWE = true;

	//Yoda
	GLfloat rotaY = 0.0f;
	float posYdroide = 0.0f;
	GLfloat rotaD = 0.0f;
	bool reproduceY = true;

	//R2-D2
	GLfloat rotaRY = 90.0f;
	GLfloat rotaRZ = 0.0f;
	float posZr = 40.0f;
	float posYr = -1.0f;
	bool reproduceR = true;

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose()) {

		//Variables para manejar el tiempo entre computadoras
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario en camara uno 
		glfwPollEvents();

		if (mainWindow.getCamara() == 1) {

			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			camera.setCameraDirection(camera.getCameraDirection());
			camera.setCameraPosition(camera.getCameraPosition());
		}
		else if (mainWindow.getCamara() == 2) {
			camera.setGiro(0,0);
			switch (giro)
			{
			case 1:
				camera.setCameraPosition(glm::vec3(0.0f + posXrobot, 10.0f, 20.0f + posZrobot));
				camera.setCameraDirection(glm::vec3(0.0f, 0.0f, -1.0f));
				break;
				
			case 2:
				camera.setCameraPosition(glm::vec3(0.0f + posXrobot, 10.0f, -20.0f + posZrobot));
				camera.setCameraDirection(glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			default:
				camera.setCameraPosition(glm::vec3(25.0f + posXrobot, 10.0f, 0.0f + posZrobot));
				camera.setCameraDirection(glm::vec3(-1.0f, 0.0f, 0.0f));
			}
		}
		else if (mainWindow.getCamara() == 3) {
			camera.setCameraPosition(glm::vec3(0.0+camera.getCameraPosition().x, 100.0f, 0.0f+camera.getCameraPosition().z));
			camera.setCameraDirection(glm::vec3(-1.0f, 0.0f, 0.0f));
			camera.setGiro(0, -90);
			camera.keyControlXZ(mainWindow.getsKeys(), deltaTime);
		}


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------------------------------------------ CAMBIO DE SKYBOX ------------------------------------------
		// skybox de día
		if (playSol)
		{
			skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
			shaderList[0].UseShader();
			shaderList[0].SetPointLights(pointLights, 0);
		}

		// Proyección de skybox de noche
		if (playLuna)
		{
			skyboxNight.DrawSkybox(camera.calculateViewMatrix(), projection);
			shaderList[0].UseShader();
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}


		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la cámara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition(); //Se toma la posición de la cámara
		lowerLight.y -= 0.3f; //se baja para que no apunten al mismo lado
		
		//función programada con posición y dirección de esos valores
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		
		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelauxL(1.0);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
				
		//Show de luces mediante teclado (Tecla C enciende y apaga)
		if (mainWindow.getPrendeLuz()) {
			colorLuzW = glm::vec3(1.0f, 1.0f, 0.0f); //Encender luz

			colorLuzF1 = glm::vec3(1.0f, 0.0f, 1.0f); //color morado
			colorLuzF2 = glm::vec3(0.0f, 1.0f, 1.0f); //color azul verdoso
			colorLuzF3 = glm::vec3(1.0f, 0.5f, 1.0f); //color lila

			if (luzFZ1 < 80.0f && luzFX1 == -60.0f) {
				luzFZ1 += 0.1 * deltaTime;				
				if (luzFZ2 > 35.0f) {
					luzFZ2 -= 0.1 * deltaTime;
				}
				else {
					luzFX2 += 0.1 * deltaTime;
				}
				luzFX3 -= 0.1 * deltaTime;
				luzFZ3 += 0.1 * deltaTime;
			}
			else if (luzFX2 > -60.0f) {
				luzFZ1 -= 0.11 * deltaTime;
				if (luzFX1 > -70.0f) {
					luzFX1 -= 0.1 * deltaTime;
				}
				luzFX3 += 0.18 * deltaTime;
				luzFZ3 -= 0.15 * deltaTime;
				luzFX2 -= 0.1 * deltaTime;
			}
			else { //Valores iniciales
				luzFX1 = -60.0f;
				luzFZ1 = 40.0f;
				luzFX3 = -70.0f;
				luzFZ3 = 50.0f;
				luzFX2 = -50.0f;
				luzFZ2 = 50.0f;
			}			
		}
		else { //Apagar luces
			colorLuzW = glm::vec3(0.0f, 0.0f, 0.0f);
			colorLuzF1 = glm::vec3(0.0f, 0.0f, 0.0f);
			colorLuzF2 = glm::vec3(0.0f, 0.0f, 0.0f);
			colorLuzF3 = glm::vec3(0.0f, 0.0f, 0.0f);
			//Valores iniciales
			luzFX1 = -60.0f;
			luzFZ1 = 40.0f;
			luzFX3 = -70.0f;
			luzFZ3 = 50.0f;
			luzFX2 = -50.0f;
			luzFZ2 = 50.0f;
		}
		spotLights[1].SetColor(colorLuzW);

		posicionLuzF1 = glm::vec3(luzFX1, 4.0f, luzFZ1); //Luz 1
		posicionLuzF2 = glm::vec3(luzFX2, 4.0f, luzFZ2); //Luz 2
		posicionLuzF3 = glm::vec3(luzFX3, 4.0f, luzFZ3); //Luz 3
		
		spotLights[2].SetPos(posicionLuzF1); //Posicionar Luz
		spotLights[2].SetColor(colorLuzF1); //Indicar color		
		spotLights[3].SetPos(posicionLuzF3);
		spotLights[3].SetColor(colorLuzF3);
		spotLights[4].SetPos(posicionLuzF2);
		spotLights[4].SetColor(colorLuzF2);
		
		// ------------------------------------------ CARGA DE MODELOS ------------------------------------------
		//------------------SOL--------------
		//_------------------ANIMACION KEYFRAMES-------------		
		animaSol();
		rotaSol += 0.01 * deltaTime; //Rotación Sol
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f + SolX, -30.0f + SolY, 20.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		model = glm::rotate(model, 20 * rotaSol * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sol_M.RenderModel();

		//--------------------------LUNAS-------------------------------
		//_------------------ANIMACION KEYFRAMES-------------
		animaLuna();
		rotaL += 0.01 * deltaTime; //Rotación Lunas
		//Fobos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f + LunaX, -30.0f + LunaY, 20.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		model = glm::rotate(model, 20 * rotaL * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelauxL = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luna_M.RenderModel();

		//Deimos
		model = modelauxL;
		model = glm::translate(model, glm::vec3(-1.0f, -2.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luna_M.RenderModel();

		//Camino
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.2f, 1.0f, 3.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Camino_M.RenderModel();

		//Wall-E
		model = glm::mat4(1.0);
		if (posXrobot <= 100.0f && posXrobot > -65.0f) {
			modelaux = model = glm::translate(model, glm::vec3(0.0f + posXrobot, 0.0f, 0.0f + posZrobot));
		}
		else if (posXrobot <= -65.0f && posXrobot > -66.0f) {
			if (posZrobot > -20.0f && adelanteZ == 1) {
				modelaux = model = glm::translate(model, glm::vec3(0.0f + posXrobot, 0.0f, 0.0f + posZrobot));
				model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (posZrobot < 20.0f && adelanteZ == 0) {
				modelaux = model = glm::translate(model, glm::vec3(0.0f + posXrobot, 0.0f, 0.0f + posZrobot));
				model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			}
		}
		
		modelaux = model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avatar_M.RenderModel(); //Cuerpo de Wall-E		

		if (mainWindow.activaAnimacionWallE) {
			if (reproduceW) {
				sonido->play2D("media/wall-e.mp3", false); //Efecto de sonido
			}
			reproduceW = false;
			if (posXrobot <= 100.0f && posXrobot > -65.0f && adelanteX == 1) {
				posXrobot -= 0.01*deltaTime;
				spotLights[1].SetPos(glm::vec3(-1.0 + posXrobot, 8.5f, 0.1 + posZrobot));
			}
			else if (posXrobot <= -65.0f && posXrobot > -66.0f && adelanteX == 1) {
				if (posZrobot > -20.0f && adelanteZ == 1) {
					giro = 1;
					posZrobot -= 0.01*deltaTime;
					spotLights[1].SetPos(glm::vec3(-1.0 + posXrobot, 8.5f, 0.1 + posZrobot));
					spotLights[1].SetFlash(glm::vec3(-1.0 + posXrobot, 8.5f, 0.1 + posZrobot), glm::vec3(0.0f, 0.0f, -1.0f));
				}
				else {
					adelanteZ = 0;
					arriba = 1;
				}

				if (posZrobot < 20.0f && adelanteZ == 0) {
					giro = 2;
					posZrobot += 0.01*deltaTime;
					spotLights[1].SetPos(glm::vec3(-1.0 + posXrobot, 8.5f, 0.1 + posZrobot));
					spotLights[1].SetFlash(glm::vec3(-1.0 + posXrobot, 8.5f, 0.1 + posZrobot), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				else
				{
					adelanteZ = 1;
					abajo = 1;
				}

				if (!adelanteZ && rotaHeli < 90) {
					posZrobot += offsetPos * deltaTime;
					rotaHeli += offsetHeli * deltaTime;
				}
				if (adelanteZ && rotaHeli > 90 && rotaHeli < 180) {
					posZrobot -= offsetPos * deltaTime;
					rotaHeli += offsetHeli * deltaTime;
				}

				if (rotaHeli > 180) {
					rotaHeli = 0;
				}
			}
		}
		else if (mainWindow.reseteaAnimacionWallE) {
			reproduceW = true;
			posXrobot = 0.0f;
			posZrobot = 0.0f;
			spotLights[1].SetPos(glm::vec3(-1.0 + posXrobot, 8.5f, 0.1 + posZrobot));
			spotLights[1].SetFlash(glm::vec3(-1.0 + posXrobot, 8.5f, 0.1 + posZrobot), glm::vec3(-1.0f, 0.0f, 0.0f));
		}

		//Brazo derecho Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getMovAvatarX(), 0.0f + mainWindow.getMovAvatarZ(), 4.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoD_M.RenderModel();

		//Brazo izquierdo Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getMovAvatarX(), 0.0f + mainWindow.getMovAvatarZ(), -12.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoI_M.RenderModel();

		//Pie derecho Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.6f, 2.4f, -6.5f));
		model = glm::rotate(model, -10 * (posXrobot + posZrobot) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotación de sus engranes
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PieD_M.RenderModel();

		//Pie izquierdo Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.6f, 2.4f, 6.5f));
		model = glm::rotate(model, -10 * (posXrobot + posZrobot) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotación de sus engranes
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PieI_M.RenderModel();

		anguloE += 0.01f * deltaTime; //ángulo de inclinación
		posYeva = 10.0f + sin(30 * anguloE * toRadians); //arriba y abajo

		// Animación Eva
		if (iniciaE) {
			if (rotaEX < 90.0f) {
				rotaEX += 0.2f*deltaTime;
			}
			else {
				giraE = true;
			}
		}

		if (giraE) {
			iniciaE = false;
			if (rotaEZ < 90.0f) {
				rotaEZ += 0.2f*deltaTime;
				posXeva -= 0.05f*deltaTime;
			}
			else {
				enfrenteW = true;
			}
		}

		if (enfrenteW) {
			giraE = false;
			posZeva -= 0.05f*deltaTime;
			if (rotaEX < 180.0f) {
				rotaEX += 0.2f*deltaTime;
			}
			if (posZeva < 11.0f) {
				if (reproduceWE) {
					sonido->play2D("media/wall-e-eve.mp3", false); //Efecto de sonido
				}
				reproduceWE = false;				
				if (posZeva < -14.0f) {
					if (reproduceD) {  //Efectos de sonido
						sonido->play2D("media/directive.mp3", false); //Efecto de sonido
					}
					reproduceD = false;
					vueltaEW = true;
				}
			}					
		}

		if (vueltaEW) {
			enfrenteW = false;			
			if (rotaEX > 90.0f) {
				rotaEX -= 0.2f*deltaTime;
				posZeva -= 0.01f*deltaTime;
			}
			if (rotaEY > -180.0f) {
				rotaEY -= 0.2f*deltaTime;	
				posZeva -= 0.01f*deltaTime;
			}
			if (posXeva < 10.0f) {
				posXeva += 0.05f*deltaTime;
			}
			else {
				regresaE = true;
			}
		}

		if (regresaE) {
			vueltaEW = false;
			if (rotaEY > -270.0f) {
				rotaEY -= 0.2f*deltaTime;				
			}
			if (posZeva < 30.0f) {
				posZeva += 0.05f*deltaTime;
				posXeva += 0.01f*deltaTime;
			} else if (rotaEY > -360.0f) {
				rotaEY -= 0.2f*deltaTime;
				posXeva -= 0.05f*deltaTime;
			}
			else { //Regresar a valores iniciales
				regresaE = false;
				posXeva = 0.0f;
				posZeva = 30.0f;
				rotaEX = 0.0f;
				rotaEY = -90.0f;
				rotaEZ = 0.0f;
				iniciaE = true;
				reproduceD = true;
				reproduceWE = true;
			}
		}
		
		//Eva
		model = modelaux;
		model = glm::translate(model, glm::vec3(posXeva, posYeva, posZeva));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotaEY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotaEX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotaEZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Eva_M.RenderModel();

		//Cucaracha
		model = modelaux;
		model = glm::translate(model, glm::vec3(5.0f, -1.0f, 15.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuca_M.RenderModel();

		if (mainWindow.getMueveYoda()) {
			if (reproduceY) {
				sonido->play2D("media/yoda-effects.mp3", false); //Efecto de sonido
			}
			reproduceY = false;
			colorLuzY1 = glm::vec3(0.0f, 0.0f, 0.0f); //apagada			
			if (rotaY < 90.0f) {
				rotaY += 0.2f*deltaTime;
				luzYX2 -= 0.1*deltaTime;
				luzYX3 += 0.1*deltaTime;
				posYdroide += 0.2*deltaTime;
				colorLuzY2 = glm::vec3(0.5f, 1.0f, 1.0f); //cyan
				colorLuzY3 = glm::vec3(0.5f, 1.0f, 0.5f); //amarillo claro
			}
			else if (rotaY < 180.0f) {
				rotaY += 0.2f*deltaTime;				
				luzYZ2 += 0.04*deltaTime;
				luzYZ3 -= 0.04*deltaTime;
				rotaD -= 0.35f*deltaTime;
				colorLuzY2 = glm::vec3(0.33f, 0.67f, 1.0f); //cyan claro
				colorLuzY3 = glm::vec3(0.33f, 1.0f, 0.33f); //verde acuoso
			}
			else if (rotaY < 270.0f) {
				rotaY += 0.2f*deltaTime;
				luzYX2 += 0.1*deltaTime;
				luzYX3 -= 0.1*deltaTime;
				posYdroide -= 0.2*deltaTime;
				colorLuzY2 = glm::vec3(0.16f, 0.34f, 1.0f); //cyan fuerte
				colorLuzY3 = glm::vec3(0.16f, 1.0f, 0.16f); //verde claro
			}
			else if (rotaY < 360.0f) {
				rotaY += 0.2f*deltaTime;
				luzYZ2 -= 0.04*deltaTime;
				luzYZ3 += 0.04*deltaTime;
				colorLuzY2 = glm::vec3(0.0f, 0.0f, 1.0f); //azul
				colorLuzY3 = glm::vec3(0.0f, 1.0f, 0.0f); //verde
			}
			else {
				mainWindow.setMueveYoda(false); //Termina animación
			}
		}
		else {
			reproduceY = true;
			colorLuzY1 = glm::vec3(1.0f, 0.27f, 0.0f); //color naranja rojizo
			colorLuzY2 = glm::vec3(0.0f, 0.0f, 0.0f); //apagada
			colorLuzY3 = glm::vec3(0.0f, 0.0f, 0.0f); //apagada
			rotaY = 0.0f;
			posYdroide = 0.0f;
			rotaD = 0.0f;
		}		

		if (!mainWindow.getVerShow()) { //Apagadas
			colorLuzY1 = glm::vec3(0.0f, 0.0f, 0.0f);
			colorLuzY2 = glm::vec3(0.0f, 0.0f, 0.0f);
			colorLuzY3 = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		posicionLuzY2 = glm::vec3(luzYX2, 2.0f, luzYZ2); //Luz 2
		posicionLuzY3 = glm::vec3(luzYX3, 2.0f, luzYZ3); //Luz 3

		spotLights[5].SetColor(colorLuzY1); //Indicar color

		spotLights[6].SetPos(posicionLuzY2);
		spotLights[6].SetColor(colorLuzY2);

		spotLights[7].SetPos(posicionLuzY3);
		spotLights[7].SetColor(colorLuzY3);

		//Yoda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 5.0f, -50.0f));
		model = glm::scale(model, glm::vec3(400.0f, 400.0f, 400.0f));
		model = glm::rotate(model, rotaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Yoda_M.RenderModel();

		//Droide de batalla (atrás)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, posYdroide, -70.0f));
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));		
		model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, rotaD * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Droide1_M.RenderModel();

		//Droide Eyegree (atrás)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, posYdroide, -70.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotaD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Droide2_M.RenderModel();

		//Droide de batalla (lado derecho a Yoda)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, posYdroide, -50.0f));
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, rotaD * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Droide1_M.RenderModel();

		//Droide Eyegree (lado izquierdo a Yoda)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, posYdroide, -50.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotaD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Droide2_M.RenderModel();

		//Droide de batalla (enfrente)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, posYdroide, -25.0f));
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 150 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, rotaD * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Droide1_M.RenderModel();

		//Droide Eyegree (enfrente)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(70.0f, posYdroide, -25.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, 210 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotaD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Droide2_M.RenderModel();

		//Cápsula de escape
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, 10.0f, 120.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		model = glm::rotate(model, 135 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pod_M.RenderModel();

		if (mainWindow.getMueveR2D2()) {
			if (reproduceR) {
				sonido->play2D("media/r2d2_sms.mp3", false); //Efecto de sonido
			}
			reproduceR = false;

			if (posYr < 10.0f && rotaRY == 90.0f) {
				posYr += 0.1f*deltaTime;
			}
			else if (rotaRY > -90.0f && posZr == 40.0f) {
				rotaRY -= 0.3f*deltaTime;
			}
			else if (posZr < 80.0f) {
				posZr += 0.1f*deltaTime;
			}
			else if (posYr > -1.0f) {
				posYr -= 0.1f*deltaTime;
			}
			else if (rotaRZ < 5.0f) {
				rotaRZ += 0.2f*deltaTime;
			}
			else if (posZr < 120.0f) {
				posZr += 0.1f*deltaTime;
			}
			else if (rotaRY < 0.0f) {
				rotaRY += 0.2f*deltaTime;
			}
			else {
				mainWindow.setMueveR2D2(false); //Termina animación
			}
		}
		else {
			reproduceR = true;			
			rotaRY = 90.0f;
			rotaRZ = 0.0f;
			posZr = 40.0f;
			posYr = -1.0f;
		}

		//R2-D2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(70.0f, posYr, posZr));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, rotaRY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotaRZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		R2D2_M.RenderModel();		

		//Animación Interceptor Jedi
		offset += 0.1f * deltaTime; //ángulo de inclinación
		posZnave = cos(40 * offset * toRadians); //arriba y abajo
				
		if (baja) {
			desaparece = 1.0f; //Aparece estrella de la muerte
			if (posYnave > 110.0f) {
				posYnave -= 0.02f * deltaTime;
			}
			else {
				giraI = true;
			}
		}

		if (giraI) {
			desaparece = 1.0f; //Aparece estrella de la muerte
			baja = false;
			if (rotacionN1 < 90.0f) {
				rotacionN1 += 0.2f *deltaTime;
				posXnave += 0.01f *deltaTime;
			}
			else if (rotacionN2 > -90.0f) {
				rotacionN2 -= 0.2f *deltaTime;
				posXnave += 0.01f *deltaTime;
			}
			else
			{
				avanza = true;
			}
		}

		if (avanza) {
			desaparece = 1.0f; //Aparece estrella de la muerte
			giraI = false;
			if (posXnave < 75.0f) {
				posXnave += 0.1f *deltaTime;
			} else if (posXnave < 80.0f) {
				if (reproduceE) {
					sonido->play2D("media/explosion.wav", false); //Efecto de sonido
				}
				reproduceE = false;
				desaparece = 0.0f;
				posXnave += 0.05f *deltaTime;
			} else {
				vuelta = true;
			}
		}

		if (vuelta) {
			desaparece = 0.0f;
			avanza = false;
			if (reproduceSW) {
				sonido->play2D("media/interceptor-jedi.mp3", false); //Efecto de sonido
			}
			reproduceSW = false;
			if (rotacionN1 < 180.0f) {
				rotacionN1 += 0.2f *deltaTime;
				posXnave += 0.1f *deltaTime;
			}
			else {
				irse = true;
			}
		}

		if (irse) {
			desaparece = 0.0f;
			vuelta = false;
			if (angulo < 100.0f) {
				angulo += 1.0f * deltaTime; //ángulo de inclinación
				posZnave = tan(angulo * toRadians); //arriba y abajo
			}
			else { //Regresar a valores iniciales
				irse = false;
				reproduceE = true;
				reproduceSW = true;
				desaparece = 1.0f;
				baja = true;
				posXnave = -150.0f;
				posYnave = 180.0f;
				rotacionN1 = 0.0f;
				rotacionN2 = 0.0f;
			}
		}

		//Nave Interceptor Jedi
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXnave, posYnave, posZnave));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotacionN1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionN2 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		InterceptorJedi_M.RenderModel();
				
		rotaDS += 0.01 * deltaTime; //Rotación Estrella de la muerte

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 100.0f, 20.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, 5 * rotaDS * toRadians, glm::vec3(0.0f, desaparece, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DeathStar_M.RenderModel();
		
		//Speeder Bike
		model = glm::mat4(1.0);
		if (posYspeeder > 0.0f && adelanteY == 0) {
			model = glm::translate(model, glm::vec3(-40.0f, -5.0f + posYspeeder, -60.0f + posZspeeder));
			model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (posYspeeder < 20.0f && adelanteY == 1 || posYspeeder == 0.0f) {
			model = glm::translate(model, glm::vec3(-40.0f, -5.0f + posYspeeder, -60.0f + posZspeeder));
		}
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SpeederBike_M.RenderModel();

		if (mainWindow.activaAnimacionSpeeder) {
			if (reproduceS) {
				sonido->play2D("media/star-wars-dogfight.mp3", false); //Efecto de sonido
			}
			reproduceS = false;
			if (posYspeeder > 0.0f && adelanteY == 0) {
				posYspeeder -= 0.5 * sin(0.05);
				posZspeeder -= 0.5 * cos(0.05);
			}
			else {
				adelanteY = 1;
			}

			if (posYspeeder < 20.0f && adelanteY == 1) {
				posYspeeder += 0.5 * sin(0.05);
				posZspeeder += 0.5 * cos(0.05);
			}
			else
			{
				adelanteY = 0;
			}
		}
		else if (mainWindow.reseteaAnimacionSpeeder) {
			reproduceS = true;
			posZspeeder = 0.0f;
			posYspeeder = 0.0f;
		}

		// Stormtrooper
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, -1.0f, -40.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ClonO_M.RenderModel();

		// Animación Stormtroopers
		if (avanzaS) {
			if (iniciaR) {
				if (rotacionDerecha > -30.0f) {
					rotacionDerecha -= 0.02f * deltaTime;
					rotacionIzquierda += 0.02f * deltaTime;
				}
				else {
					regresaR = true;
					iniciaR = false;
				}
			}
			if (regresaR) {
				if (rotacionDerecha < 30.0f) {
					rotacionDerecha += 0.02f * deltaTime;
					rotacionIzquierda -= 0.02f * deltaTime;
				}
				else {
					iniciaR = true;
					regresaR = false;
				}
			}
			if (posZclon < -25.0f) {
				posZclon += 0.01f * deltaTime;
			}
			else {
				vueltaS = true;
			}
		}

		if (vueltaS) {
			avanzaS = false;
			if (iniciaR) {
				if (rotacionDerecha > -30.0f) {
					rotacionDerecha -= 0.02f * deltaTime;
					rotacionIzquierda += 0.02f * deltaTime;
				}
				else {
					regresaR = true;
					iniciaR = false;
				}
			}
			if (regresaR) {
				if (rotacionDerecha < 30.0f) {
					rotacionDerecha += 0.02f * deltaTime;
					rotacionIzquierda -= 0.02f * deltaTime;
				}
				else {
					iniciaR = true;
					regresaR = false;
				}
			}
			if (rotacionS1 > -180.0f) {
				rotacionS1 -= 0.2f *deltaTime;
			}
			else {
				regresa = true;
			}
		}

		if (regresa) {
			vueltaS = false;
			if (iniciaR) {
				if (rotacionDerecha > -30.0f) {
					rotacionDerecha -= 0.02f * deltaTime;
					rotacionIzquierda += 0.02f * deltaTime;
				}
				else {
					regresaR = true;
					iniciaR = false;
				}
			}
			if (regresaR) {
				if (rotacionDerecha < 30.0f) {
					rotacionDerecha += 0.02f * deltaTime;
					rotacionIzquierda -= 0.02f * deltaTime;
				}
				else {
					iniciaR = true;
					regresaR = false;
				}
			}
			if (posZclon > -80.0f) {
				posZclon -= 0.01f * deltaTime;
			}
			else {
				vuelveOrigen = true;
			}
		}

		if (vuelveOrigen) {
			regresa = false;
			if (reproduceDV) {
				sonido->play2D("media/darth_vader.mp3", false); //Efecto de sonido
			}
			reproduceDV = false;
			if (iniciaR) {
				if (rotacionDerecha > -30.0f) {
					rotacionDerecha -= 0.02f * deltaTime;
					rotacionIzquierda += 0.02f * deltaTime;
				}
				else {
					regresaR = true;
					iniciaR = false;
				}
			}
			if (regresaR) {
				if (rotacionDerecha < 30.0f) {
					rotacionDerecha += 0.02f * deltaTime;
					rotacionIzquierda -= 0.02f * deltaTime;
				}
				else {
					iniciaR = true;
					regresaR = false;
				}
			}
			if (rotacionS1 < 0.0f) {
				rotacionS1 += 0.2f *deltaTime;
				rotacionS2 -= 0.4f * deltaTime;
			}
			else {
				subeBrazos = true;
			}
		}

		if (subeBrazos) {
			vuelveOrigen = false;
			if (rotacionBrazoD > -180.0f) {
				rotacionBrazoD -= 0.55f * deltaTime;
				rotacionBrazoI -= 0.5f * deltaTime;
			}
			else {
				terminaS = true;
			}
		}

		if (terminaS) {
			subeBrazos = false;
			if (rotacionBrazoD < 0.0f) {
				rotacionBrazoD += 0.55f * deltaTime;
				rotacionBrazoI += 0.5f * deltaTime;
			}
			else { //Regresar a valores iniciales
				terminaS = false;
				rotacionBrazoD = 0.0f;
				rotacionBrazoI = 0.0f;
				avanzaS = true;
				reproduceDV = true;
			}
		}

		// Stormtrooper 1 (cuerpo)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXclon, posYclon, posZclon));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotacionS1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionS2 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoC_M.RenderModel();

		// Stormtrooper (brazo izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionIzquierda + rotacionBrazoI) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCI_M.RenderModel();

		// Stormtrooper (brazo derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionDerecha + rotacionBrazoD) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCD_M.RenderModel();

		// Stormtrooper (pierna izquierda)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionDerecha  * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCI_M.RenderModel();

		// Stormtrooper (pierna derecha)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionIzquierda * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCD_M.RenderModel();

		// Stormtrooper 2 (cuerpo)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXclon - 15.0f, posYclon, posZclon));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotacionS1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionS2 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoC_M.RenderModel();

		// Stormtrooper (brazo izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionIzquierda + rotacionBrazoI) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCI_M.RenderModel();

		// Stormtrooper (brazo derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionDerecha + rotacionBrazoD) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCD_M.RenderModel();

		// Stormtrooper (pierna izquierda)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionDerecha  * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCI_M.RenderModel();

		// Stormtrooper (pierna derecha)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionIzquierda * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCD_M.RenderModel();

		// Stormtrooper 3 (cuerpo)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXclon - 30.0f, posYclon, posZclon));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotacionS1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionS2 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoC_M.RenderModel();

		// Stormtrooper (brazo izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionIzquierda + rotacionBrazoI) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCI_M.RenderModel();

		// Stormtrooper (brazo derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionDerecha + rotacionBrazoD) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCD_M.RenderModel();

		// Stormtrooper (pierna izquierda)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionDerecha  * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCI_M.RenderModel();

		// Stormtrooper (pierna derecha)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionIzquierda * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCD_M.RenderModel();

		// Stormtrooper 4 atrás (cuerpo)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXclon, posYclon, posZclon - 20.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotacionS1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionS2 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoC_M.RenderModel();

		// Stormtrooper (brazo izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionIzquierda + rotacionBrazoI) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCI_M.RenderModel();

		// Stormtrooper (brazo derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionDerecha + rotacionBrazoD) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCD_M.RenderModel();

		// Stormtrooper (pierna izquierda)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionDerecha  * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCI_M.RenderModel();

		// Stormtrooper (pierna derecha)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionIzquierda * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCD_M.RenderModel();

		// Stormtrooper 5 (cuerpo)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXclon - 15.0f, posYclon, posZclon - 20.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotacionS1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionS2 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoC_M.RenderModel();

		// Stormtrooper (brazo izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionIzquierda + rotacionBrazoI) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCI_M.RenderModel();

		// Stormtrooper (brazo derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionDerecha + rotacionBrazoD) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCD_M.RenderModel();

		// Stormtrooper (pierna izquierda)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionDerecha  * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCI_M.RenderModel();

		// Stormtrooper (pierna derecha)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionIzquierda * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCD_M.RenderModel();

		// Stormtrooper 6 (cuerpo)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXclon - 30.0f, posYclon, posZclon - 20.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, rotacionS1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionS2 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoC_M.RenderModel();

		// Stormtrooper (brazo izquierdo)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionIzquierda + rotacionBrazoI) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCI_M.RenderModel();

		// Stormtrooper (brazo derecho)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, 0.21f, -0.03f));
		model = glm::rotate(model, (rotacionDerecha + rotacionBrazoD) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoCD_M.RenderModel();

		// Stormtrooper (pierna izquierda)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionDerecha  * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCI_M.RenderModel();

		// Stormtrooper (pierna derecha)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::rotate(model, rotacionIzquierda * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaCD_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-70.0f, -1.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FinnJake_M.RenderModel();

		// Montaña
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-253.0f, 27.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 350.0f, 250.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Mont_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(250.0f, -1.0f, -70.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 200 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Perseverance_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(214.0f, 0.0f, -80.0f));
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Rocas_M.RenderModel();

		// Nave de Darth Vader
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-180.0f, 10.0f, -110.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		model = glm::rotate(model, -127 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TieDV_M.RenderModel();

		// Naves TIE
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 180.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -115 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TieB_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-70.0f, 0.0f, 250.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TieD_M.RenderModel();

		// CUBOS DE BASURA
		//Lado derecho visto de frente de Wall-E
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(275.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(285.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(265.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(215.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(225.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(235.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(185.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(195.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(205.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(155.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(175.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(125.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(135.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(145.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(95.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(115.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(65.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-85.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-65.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-115.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-95.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-145.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-135.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-125.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-165.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-155.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(245.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(255.0f, 0.0f, 75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-185.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		//Lado Izquierdo visto de frente de Wall-E				
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(125.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();
				
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(145.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(95.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(115.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(65.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-85.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-65.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-115.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-95.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-145.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-135.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-125.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-165.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-155.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-205.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-195.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-185.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-225.0f, 0.0f, -75.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-215.0f, 0.0f, -40.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		// Estrellas		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 200.0f, 100.0f));		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Estrellas_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 200.0f, 100.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Estrellas_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 200.0f, 100.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Estrellas_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 200.0f, 100.0f));
		model = glm::rotate(model, -270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Estrellas_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	sonido->drop(); // borrar sonido
	return 0;
}
