#include <GL/glut.h>

#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#include "tinyxml2.h"

typedef std::tuple<float, float, float> vertice;

std::vector<vertice> allVertices;
GLdouble dist = 10, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;

// Function called when the window is resized
void resizeWindow(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);

	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

vertice extractVertice(std::string s) {
	std::string delimiter = " ";
	float x, y, z;
	int pos;
	std::string token;

	// x
	pos = s.find(delimiter);
	token = s.substr(0, pos);
	x = atof(token.c_str());
	s.erase(0, pos + delimiter.length());
	// y
	pos = s.find(delimiter);
	token = s.substr(0, pos);
	y = atof(token.c_str());
	s.erase(0, pos + delimiter.length());
	// z
	pos = s.find(delimiter);
	token = s.substr(0, pos);
	z = atof(token.c_str());

	return vertice(x,y,z);
}

void addVertices(std::ifstream &vertices) {
    char v[100];
    while (vertices.getline(v, 100)) {
        allVertices.push_back(extractVertice(v));
    }
}

// Function that draws the vertices to the screen
void drawVertices() {
	glBegin(GL_TRIANGLES);

	for(vertice v: allVertices) {
		// Dar uma cor aleatória a cada triângulo (AMAZING!!!)
		glColor3f(rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX);
		glVertex3f(std::get<0>(v), std::get<1>(v), std::get<2>(v));
	}

	glEnd();
}

// Function called when it is necessary to render the scene
void renderScene() {
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera
	glLoadIdentity();
	gluLookAt(dist*cos(beta)*sin(alpha), dist*sin(beta), dist*cos(beta)*cos(alpha),
		      0.0, 0.0, 0.0,
		      0.0f, 1.0f, 0.0f);

	// Translate the vertices to the desired location
	glTranslatef(xd, 0, zd);

	// Draw the vertices that were loaded from the ".3d" files
	drawVertices();

	// End of frame
	glutSwapBuffers();
}

// Function called when an ascii key is pressed
void processKeys(unsigned char c, int xx, int yy) {
	float deltaToZoom = 0.3;
	float deltaToMove = 0.1;
	switch (c) {
		case 'w':
			xd -= deltaToMove;
			zd -= deltaToMove;
			break;

		case 'a':
			xd -= deltaToMove;
			zd += deltaToMove;
			break;

		case 's':
			xd += deltaToMove;
			zd += deltaToMove;
			break;

		case 'd':
			xd += deltaToMove;
			zd -= deltaToMove;
			break;

		case 'q':
			dist += deltaToZoom;
			break;

		case 'e':
			dist -= deltaToZoom;
			break;

		default:
			return;
	}

	glutPostRedisplay();
}

// Function called when a non-ascii key is pressed
void processSpecialKeys(int key, int xx, int yy) {
	float deltaToMove = 0.1;
	switch (key) {
		case GLUT_KEY_UP:
			beta += deltaToMove;
			break;

		case GLUT_KEY_DOWN:
			beta -= deltaToMove;
			break;

		case GLUT_KEY_LEFT:
			alpha -= deltaToMove;
			break;

		case GLUT_KEY_RIGHT:
			alpha += deltaToMove;
			break;

		default:
			return;
	}

	glutPostRedisplay();
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "Por favor, forneça um ficheiro XML\n";
		exit(EXIT_FAILURE);
	}

	// Carregar o ficheiro XML para a memória
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError e = doc.LoadFile(argv[1]);
	if (e != tinyxml2::XML_SUCCESS) {
		std::cout << e << "Por favor, forneça um ficheiro XML válido\n";
		exit(EXIT_FAILURE);
	}

	// Guardar o atributo "scene"
	tinyxml2::XMLElement *scene = doc.FirstChildElement("scene");
	if (!scene) {
		std::cout << "Formato do ficheiro inválido\n";
		exit(EXIT_FAILURE);
	}

	// Percorrer os atributos "model"
	tinyxml2::XMLElement *model = scene->FirstChildElement("model");
	for (; model != nullptr; model = model->NextSiblingElement()) {
		std::ifstream myfile;
		myfile.open(model->Attribute("file"));
		addVertices(myfile);
		myfile.close();
	}

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Fase1TP - Engine");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(resizeWindow);

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
