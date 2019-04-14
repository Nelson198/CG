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
#include <unordered_map>

#include "tinyxml2.h"

struct Vertex {
	float x;
	float y;
	float z;

	bool operator==(const Vertex &other) const {
		return (x == other.x && y == other.y && z == other.z);
  }
};

struct VertexHasher {
	std::size_t operator()(const Vertex& k) const {
		using std::size_t;
		using std::hash;
		using std::string;

		return ((hash<float>()(k.x) ^ (hash<float>()(k.y) << 1)) >> 1) ^ (hash<float>()(k.z) << 1);
	}
};

struct Color {
	float R;
	float G;
	float B;
};

struct Transformation {
	char type;    // It can be: [TM] - Translate (Normal/Time), [AI] - Rotate (Angle/Time), S - Scale
	float param1; // These parameters depend on the type
	float param2;
	float param3;
	float param4;
};

struct Group {
	std::vector<Transformation> trans;
	std::vector<Vertex> vert;
	std::unordered_map<Vertex,Color,VertexHasher> vertColors;
	std::vector<Group> subGroups;
};

Group mainGroup;

GLdouble dist = 50, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;

// Function that handles the drawing of a given group
void drawGroup(Group g) {
	glPushMatrix();

	bool timeRotated = false, timeTranslated = false;
	float timeRotationParams[4];
	for (Transformation t: g.trans) {
		switch (t.type) {
			case 'T': // Translation
				glTranslatef(t.param1, t.param2, t.param3);
				break;

			case 'M': // Translation Time
				timeTranslated = true;
				/* ACABAR */
				break;

			case 'A': // Rotate Angle
				glRotatef(t.param1, t.param2, t.param3, t.param4);
				break;

			case 'I': // Rotate Time
				timeRotated = true;
				timeRotationParams[0] = fmod(float(glutGet(GLUT_ELAPSED_TIME))/1000, t.param1)*360/t.param1;
				timeRotationParams[1] = t.param2;
				timeRotationParams[2] = t.param3;
				timeRotationParams[3] = t.param4;
				glRotatef(timeRotationParams[0], timeRotationParams[1], timeRotationParams[2], timeRotationParams[3]);
				break;

			case 'S':
				glScalef(t.param1, t.param2, t.param3);
				break;

			default:
				break;
		}
	}

	glBegin(GL_TRIANGLES);
	for (Vertex v: g.vert) {
		Color c = g.vertColors.at(v);
		glColor3f(c.R, c.G, c.B);
		glVertex3f(v.x, v.y, v.z);
	}
	glEnd();

	if (timeRotated)
		glRotatef(-timeRotationParams[0], timeRotationParams[1], timeRotationParams[2], timeRotationParams[3]);

	for (Group g: g.subGroups)
		drawGroup(g);

	glPopMatrix();
}

// Function that draws the axis
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
	// drawAxis();

	// Draw the scene that was loaded from the XML file
	drawGroup(mainGroup);

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

// Function that creates a vertex object from its string representation "x y z"
Vertex extractVertice(std::string s) {
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

	return Vertex{x,y,z};
}

// Function that processes and adds the vertices from a file to the allVertices vector
void addVertices(std::ifstream &vertices, Group *group) {
    char v[100];
    while (vertices.getline(v, 100))
        group->vert.push_back(extractVertice(v));
}

// Function that processes a group's information and returns an object with that info
Group processGroup(tinyxml2::XMLElement *group) {
	Group currentG;
	// Add all the attributes inside the current group to its "group" object
	for (tinyxml2::XMLElement *elem = group->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {
		std::string elemName = elem->Value();
		if (elemName == "translate") {
			if (elem->FloatAttribute("time") >= 0) {
				Transformation t = {type: 'M', param1: elem->FloatAttribute("time")};
				currentG.trans.push_back(t);
			} else {
				Transformation t = {type: 'T', param1: elem->FloatAttribute("X"), param2: elem->FloatAttribute("Y"), param3: elem->FloatAttribute("Z")};
				currentG.trans.push_back(t);
			}
		} else if (elemName == "rotate") {
			if (elem->FloatAttribute("angle") != 0) {
				Transformation t = {type: 'A', param1: elem->FloatAttribute("angle"), param2: elem->FloatAttribute("axisX"), param3: elem->FloatAttribute("axisY"), param4: elem->FloatAttribute("axisZ")};
				currentG.trans.push_back(t);			
			} else {
				Transformation t = {type: 'I', param1: elem->FloatAttribute("time"), param2: elem->FloatAttribute("axisX"), param3: elem->FloatAttribute("axisY"), param4: elem->FloatAttribute("axisZ")};
				currentG.trans.push_back(t);
			}
		} else if (elemName == "scale") {
			Transformation t = {type: 'S', param1: elem->FloatAttribute("X"), param2: elem->FloatAttribute("Y"), param3: elem->FloatAttribute("Z")};
			currentG.trans.push_back(t);
		} else if (elemName == "models") {
			// Iterate over all the models and load their vertices (including their base color)
			for (tinyxml2::XMLElement *model = elem->FirstChildElement("model"); model != nullptr; model = model->NextSiblingElement("model")) {
				// Extract the vertices
				std::ifstream myfile;
				myfile.open(model->Attribute("file"));
				addVertices(myfile, &currentG);
				myfile.close();

				// Extract the color of the vertices
				Color c = {model->FloatAttribute("R"), model->FloatAttribute("G"), model->FloatAttribute("B")};

				// Attribute a color to each of the vertices
				if (c.R != 0 || c.G != 0 || c.B != 0) {
					for (auto vertex : currentG.vert) {
						float variation = (rand() / (float) RAND_MAX) / 5;
						Color toAdd = {c.R + variation, c.G + variation, c.B + variation};
						currentG.vertColors.insert(std::pair<Vertex,Color>(vertex, toAdd));
					}
				} else {
					// Give a different color to every vertex, so that a gradient effect is applied
					for (auto vertex : currentG.vert) {
						Color toAdd = {rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX};
						currentG.vertColors.insert(std::pair<Vertex,Color>(vertex, toAdd));
					}
				}
			}
		} else if (elemName == "group") {
			Group child = processGroup(elem);
			currentG.subGroups.push_back(child);
		}
	}

	return currentG;
}

// Function that processes the XML file received as an argument
void processXML(char **argv) {
	// Load the XML file to memory
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError e = doc.LoadFile(argv[1]);
	if (e != tinyxml2::XML_SUCCESS) {
		std::cout << "Por favor, forneça um ficheiro XML válido\n";
		exit(EXIT_FAILURE);
	}

	// Get the structure associated to the attribute "scene"
	tinyxml2::XMLElement *scene = doc.FirstChildElement("scene");
	if (!scene) {
		std::cout << "Formato do ficheiro inválido\n";
		exit(EXIT_FAILURE);
	}

	// Process the scene element and all its children
	mainGroup = processGroup(scene);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "Por favor, forneça um ficheiro XML\n";
		exit(EXIT_FAILURE);
	}

	// Processing XML file
	processXML(argv);

	// Init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Fase3TP - Engine");

	// Register the required callback 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(resizeWindow);

	// Resgister callback for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	// Set OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glPointSize(3);

	// Enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
