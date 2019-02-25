#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#include "tinyxml2.h"

typedef std::tuple<float, float, float> vertex;

std::vector<vertex> allVertices;
GLdouble dist = 10, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;

// Function that creates a vertex object from its string representation "x y z"
vertex extractVertice(std::string s) {
	std::string delimiter = " ";
	float x, y, z;
	int pos;
	std::string token;

	// Process the x coordinate
	pos = s.find(delimiter);
	token = s.substr(0, pos);
	x = atof(token.c_str());
	s.erase(0, pos + delimiter.length());

	// Process the y coordinate
	pos = s.find(delimiter);
	token = s.substr(0, pos);
	y = atof(token.c_str());
	s.erase(0, pos + delimiter.length());

	// Process the z coordinate
	pos = s.find(delimiter);
	token = s.substr(0, pos);
	z = atof(token.c_str());

	return vertex(x,y,z);
}

// Function that processes and adds the vertices from a file to the allVertices vector
void addVertices(std::ifstream &vertices) {
    char v[100];
    while (vertices.getline(v, 100)) {
        allVertices.push_back(extractVertice(v));
    }
}

// Function that draws the loaded vertices to the screen
void drawVertices() {
	glBegin(GL_TRIANGLES);

	for(vertex v: allVertices) {
		// Give a different color to every vertex, so that a gradient effect is applied
		glColor3f(rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX);
		glVertex3f(std::get<0>(v), std::get<1>(v), std::get<2>(v));
	}

	glEnd();
}
void drawAxis() {
	glBegin(GL_LINES);
		// x
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-100.0, 0.0, 0.0);
		glVertex3f(100.0, 0.0, 0.0);
		// y
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, -100.0, 0.0);
		glVertex3f(0.0, 100.0, 0.0);
		// z
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, -100.0);
		glVertex3f(0.0, 0.0, 100.0);
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

	// Draw the axis
	drawAxis();

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
		// Move the vertices in the (-deltaToMove, 0, -deltaToMove) direction
		case 'w':
			xd -= deltaToMove;
			zd -= deltaToMove;
			break;

		// Move the vertices in the (-deltaToMove, 0, deltaToMove) direction
		case 'a':
			xd -= deltaToMove;
			zd += deltaToMove;
			break;

		// Move the vertices in the (deltaToMove, 0, deltaToMove) direction
		case 's':
			xd += deltaToMove;
			zd += deltaToMove;
			break;

		// Move the vertices in the (deltaToMove, 0, -deltaToMove) direction
		case 'd':
			xd += deltaToMove;
			zd -= deltaToMove;
			break;

		// Move the camera closer to (0, 0, 0)
		case 'q':
			dist += deltaToZoom;
			break;

		// Move the camera further from (0, 0, 0)
		case 'e':
			dist -= deltaToZoom;
			break;

		// Show triangles as filled shapes
		case 'f':
			glPolygonMode(GL_FRONT, GL_FILL);
			break;

		// Show triangles' edges 
		case 'l':
			glPolygonMode(GL_FRONT, GL_LINE);
			break;

		// Show triangles' vertices
		case 'p':
			glPolygonMode(GL_FRONT, GL_POINT);
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
		// Move the camera up
		case GLUT_KEY_UP:
			beta += deltaToMove;
			break;

		// Move the camera down
		case GLUT_KEY_DOWN:
			beta -= deltaToMove;
			break;

		// Move the camera to the left
		case GLUT_KEY_LEFT:
			alpha -= deltaToMove;
			break;

		// Move the camera to the right
		case GLUT_KEY_RIGHT:
			alpha += deltaToMove;
			break;

		default:
			return;
	}

	glutPostRedisplay();
}

// Function called when the window is resized
void resizeWindow(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// Compute the window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);

	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// Return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "Por favor, forneça um ficheiro XML\n";
		exit(EXIT_FAILURE);
	}

	// Load the XML file to memory
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError e = doc.LoadFile(argv[1]);
	if (e != tinyxml2::XML_SUCCESS) {
		std::cout << "Por favor, forneça um ficheiro XML válido\n";
		exit(EXIT_FAILURE);
	}

	// Save the structure associated to the attribute "scene"
	tinyxml2::XMLElement *scene = doc.FirstChildElement("scene");
	if (!scene) {
		std::cout << "Formato do ficheiro inválido\n";
		exit(EXIT_FAILURE);
	}

	// Go through all "model" structures
	for (tinyxml2::XMLElement *model = scene->FirstChildElement("model"); model != nullptr; model = model->NextSiblingElement()) {
		std::ifstream myfile;
		myfile.open(model->Attribute("file"));
		addVertices(myfile);
		myfile.close();
	}

	// Init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Fase1TP - Engine");

	// Register the required callback 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(resizeWindow);

	// Resgister callback for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	// Set OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
