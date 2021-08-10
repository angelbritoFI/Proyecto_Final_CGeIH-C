/*
* Materia: Laboratorio de Computaci�n Gr�fica e Interacci�n Humano-Computadora
* Semestre 2021-2
* Fecha de creaci�n: 12/07/2021
* Descripci�n: M�todos del objeto Window
* Autores:
*	Brito Segura Angel
* Colaboradores (hasta versi�n 1):
	Hern�ndez Torres Agust�n de Jes�s
	Huarte Nolasco Mario
*/
#include "Window.h"

Window::Window() {
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight) {
	width = windowWidth;
	height = windowHeight;
	prendeLuz = false; //Sin Luz
	verShow = true; //Con luz
	mueveYoda = false; //Inicia sin movimiento 
	mueveR2D2 = false; //Inicia sin movimiento
	activaAnimacionWallE = false; //Activar animaci�n Wall-E
	reseteaAnimacionWallE = false; //Resetear animaci�n Wall-E
	activaAnimacionSpeeder = false; //Activar animaci�n Speeder Bike
	reseteaAnimacionSpeeder = false; //Resetear animaci�n Speeder Bike
	movimientoAvatar[0] = 0.0f; // X
	movimientoAvatar[1] = 0.0f; // Z
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

int Window::Initialise() {
	//Inicializaci�n de GLFW
	if (!glfwInit()) {
		printf("Fall� inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Proyecto Final", NULL, NULL);

	if (!mainWindow) {
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}

	//Obtener tama�o de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("Fall� inicializaci�n de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	//Callback para detectar que se est� usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks() {
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange() {
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange() {
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

int contadorColor, contadorShow, contadorAnimacionWallE, contadorResetWallE, contadorAnimacionSpeeder, contadorResetSpeeder;

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_1) {
		theWindow->camara = 1;
	}
	if (key == GLFW_KEY_2) {
		theWindow->camara = 2;
	}
	if (key == GLFW_KEY_3) {
		theWindow->camara = 3;
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS && contadorColor % 2 == 0) {
		theWindow->prendeLuz = true;
		contadorColor = contadorColor+1;
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS && contadorColor % 2 != 0) {
		theWindow->prendeLuz = false;
		contadorColor = contadorColor+1;
	}

	if (key == GLFW_KEY_H && action == GLFW_PRESS && contadorShow % 2 == 0) {
		theWindow->verShow = false;
		contadorShow++;
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS && contadorShow % 2 != 0) {
		theWindow->verShow = true;
		contadorShow++;
	}

	if (key == GLFW_KEY_Y) {
		theWindow->mueveYoda = true;
	}

	if (key == GLFW_KEY_N) {
		theWindow->mueveR2D2 = true;
	}

	//Mover brazos del avatar con las flechas
	if (key == GLFW_KEY_LEFT && theWindow->movimientoAvatar[0] >= -5.0) {
		theWindow->movimientoAvatar[0] -= 1.0; // X--
	}
	else if (key == GLFW_KEY_LEFT && theWindow->movimientoAvatar[0] < -5.0) {
		theWindow->movimientoAvatar[0] -= 0.0; // X--
	}

	if (key == GLFW_KEY_RIGHT && theWindow->movimientoAvatar[0] <= -1.0) {
		theWindow->movimientoAvatar[0] += 1.0; // X++
	}
	else if (key == GLFW_KEY_RIGHT && theWindow->movimientoAvatar[0] > -1.0) {
		theWindow->movimientoAvatar[0] += 0.0; // X++
	}

	if (key == GLFW_KEY_DOWN && theWindow->movimientoAvatar[0] < -5.0 && theWindow->movimientoAvatar[1] >= -3.0) {
		theWindow->movimientoAvatar[1] -= 1.0; // Y--
	}
	else if (key == GLFW_KEY_DOWN && theWindow->movimientoAvatar[0] < -5.0 && theWindow->movimientoAvatar[1] < -3.0) {
		theWindow->movimientoAvatar[1] -= 0.0; // Y--
	}

	if (key == GLFW_KEY_UP && theWindow->movimientoAvatar[0] < -5.0 && theWindow->movimientoAvatar[1] <= -1.0) {
		theWindow->movimientoAvatar[1] += 1.0; // Y++
	}
	else if (key == GLFW_KEY_UP && theWindow->movimientoAvatar[0] < -5.0 && theWindow->movimientoAvatar[1] > -1.0) {
		theWindow->movimientoAvatar[1] += 0.0; // Y++
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS && contadorAnimacionWallE % 2 == 0) {
		theWindow->activaAnimacionWallE = true;
		contadorAnimacionWallE = contadorAnimacionWallE + 1;
	}
	else if (key == GLFW_KEY_I && action == GLFW_PRESS && contadorAnimacionWallE % 2 != 0) {
		theWindow->activaAnimacionWallE = false;
		contadorAnimacionWallE = contadorAnimacionWallE + 1;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS && contadorAnimacionSpeeder % 2 == 0) {
		theWindow->activaAnimacionSpeeder = true;
		contadorAnimacionSpeeder = contadorAnimacionSpeeder + 1;
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS && contadorAnimacionSpeeder % 2 != 0) {
		theWindow->activaAnimacionSpeeder = false;
		contadorAnimacionSpeeder = contadorAnimacionSpeeder + 1;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS && contadorResetWallE % 2 == 0) {
		theWindow->reseteaAnimacionWallE = true;
		theWindow->reseteaAnimacionSpeeder = true;
		contadorResetWallE = contadorResetWallE + 1;
		contadorResetSpeeder = contadorResetSpeeder + 1;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS && contadorResetWallE % 2 != 0) {
		theWindow->reseteaAnimacionWallE = false;
		theWindow->reseteaAnimacionSpeeder = false;
		contadorResetWallE = contadorResetWallE + 1;
		contadorResetSpeeder = contadorResetSpeeder + 1;
	}

	if (key >= 0 && key < 1024)	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			printf("se presiono la tecla %d'\n", key);			
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
