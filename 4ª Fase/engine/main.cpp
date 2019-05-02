#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <IL/il.h>

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
	float diffR, diffG, diffB;
	float specR, specG, specB;
	float emisR, emisG, emisB;
	float ambiR, ambiG, ambiB;
};

struct Transformation {
	char type;    // It can be: [TM] - Translate (Normal/Time), [AI] - Rotate (Angle/Time), S - Scale
	float param1; // These parameters depend on the type
	float param2;
	float param3;
	float param4;
	std::vector<Vertex> catmull;
};

struct Model {
	int bufferIdx;
	int textureIdx;
	std::vector<Vertex> vertices;
	std::vector<Color> colors;
};

struct Group {
	std::vector<Transformation> trans;
	std::vector<Model> models;
	std::vector<Group> subGroups;
};

Group mainGroup;

GLdouble dist = 50, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;

// Vector to store vertex positions
GLuint *buffers;
int numModels = 0, currentModelIdx = 0;

// Function that normalizes a vector
void normalize(float* a) {
	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}

// Function that returns the point resulting from multiplying a 4D matrix by a point
Vertex multMatrixPoint(float mat[4][4], Vertex v) {
	float x = mat[0][0]*v.x + mat[0][1]*v.y + mat[0][2]*v.z + mat[0][3];
	float y = mat[1][0]*v.x + mat[1][1]*v.y + mat[1][2]*v.z + mat[1][3];
	float z = mat[2][0]*v.x + mat[2][1]*v.y + mat[2][2]*v.z + mat[2][3];

	return Vertex{x, y, z};
}

// Function that multiplies a matrix by a vector
void multMatrixVector(float *m, float *v, float *res) {
    for (int j = 0; j < 4; j++) {
        res[j] = 0;
        for (int k = 0; k < 4; k++) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }
}

// Function that returns the point in the curve given a time
Vertex getCatmullRomPoint(float t, Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex *deriv) {
    float T[4] = {powf(t,3), powf(t,2), t, 1};
    float T1[4] = {3*powf(t,2), 2*t, 1, 0};
	Vertex pos;

    // Catmull-Rom matrix
    float m[4][4] = {   {-0.5f,  1.5f, -1.5f,  0.5f},
                        { 1.0f, -2.5f,  2.0f, -0.5f},
                        {-0.5f,  0.0f,  0.5f,  0.0f},
                        { 0.0f,  1.0f,  0.0f,  0.0f}};
            
    float p1[4] = {v0.x, v1.x, v2.x, v3.x};
    float a1[4];
    multMatrixVector((float*)m, p1, a1);
    pos.x = T[0]*a1[0] + T[1]*a1[1] + T[2]*a1[2] + T[3]*a1[3];
	deriv->x = T1[0]*a1[0] + T1[1]*a1[1] + T1[2]*a1[2] + T1[3]*a1[3];

    float p2[4] = {v0.y, v1.y, v2.y, v3.y};
    float a2[4];
    multMatrixVector((float*)m, p2, a2);
    pos.y = T[0]*a2[0] + T[1]*a2[1] + T[2]*a2[2] + T[3]*a2[3];
	deriv->y = T1[0]*a2[0] + T1[1]*a2[1] + T1[2]*a2[2] + T1[3]*a2[3];
    
    float p3[4] = {v0.z, v1.z, v2.z, v3.z};
    float a3[4];
    multMatrixVector((float*)m, p3, a3);
    pos.z = T[0]*a3[0] + T[1]*a3[1] + T[2]*a3[2] + T[3]*a3[3];
	deriv->z = T1[0]*a3[0] + T1[1]*a3[1] + T1[2]*a3[2] + T1[3]*a3[3];

	return pos;
}

// Function that returns the point in the curve given a time
Vertex getGlobalCatmullRomPoint(std::vector<Vertex> pontos, float elapsedTime, Vertex *deriv) {
    int POINT_COUNT = pontos.size();
    float t = elapsedTime * POINT_COUNT; // this is the real global t
    int index = floor(t);  // which segment
    t = t - index; // where within  the segment

    // indices store the points
    int indices[4]; 
    indices[0] = (index + POINT_COUNT-1)%POINT_COUNT;   
    indices[1] = (indices[0]+1)%POINT_COUNT;
    indices[2] = (indices[1]+1)%POINT_COUNT; 
    indices[3] = (indices[2]+1)%POINT_COUNT;

    return getCatmullRomPoint(t, pontos[indices[0]], pontos[indices[1]], pontos[indices[2]], pontos[indices[3]], deriv);
}

// Function that builds the rotation matrix from a Catmull-Rom's matrix
void buildRotMatrix(float *x, float *y, float *z, float *m) {
	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

// Function that calculates the cross product betqween two vectors
void cross(float *a, float *b, float *res) {
	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}

// Function that draws the data of a given group
void drawGroup(Group g) {
	glPushMatrix();

	int numRotations = 0;
	float timeRotationParams[4];
	for (Transformation t: g.trans) {
		switch (t.type) {
			case 'T': // Translation
				glTranslatef(t.param1, t.param2, t.param3);
				break;

			case 'M': // Translation Time
			{
				Vertex deriv;
				Vertex p = getGlobalCatmullRomPoint(t.catmull, float(float(glutGet(GLUT_ELAPSED_TIME))/1000/t.param1), &deriv);
				glTranslatef(p.x, p.y, p.z);

				float z[3];
				float y[3] = {0,0,1};
				float d[3] = {deriv.x, deriv.y, deriv.z};
				float m[4][4];
				cross(d,y,z);
				normalize(z);
				cross(z, d, y);
				normalize(y);
				normalize(d);
				buildRotMatrix(d,y,z,(float *)m);
				glMultMatrixf((float *)m);

				break;
			}

			case 'A': // Rotate Angle
				glRotatef(t.param1, t.param2, t.param3, t.param4);
				break;

			case 'I': // Rotate Time
				timeRotationParams[0] = fmod(float(glutGet(GLUT_ELAPSED_TIME))/1000, t.param1)*360/t.param1;
				timeRotationParams[1] = t.param2;
				timeRotationParams[2] = t.param3;
				timeRotationParams[3] = t.param4;
				numRotations++;
				glRotatef(timeRotationParams[0], timeRotationParams[1], timeRotationParams[2], timeRotationParams[3]);
				break;

			case 'S':
				glScalef(t.param1, t.param2, t.param3);
				break;

			default:
				break;
		}
	}

	for (Model m : g.models) {
		if (m.textureIdx != -1)
			glBindTexture(GL_TEXTURE_2D, m.textureIdx);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[m.bufferIdx]);
		glVertexPointer(3, GL_FLOAT, 0, nullptr);
		glColorPointer(3, GL_FLOAT, 0, (void *) (m.vertices.size()*3*sizeof(GLfloat)));
		glDrawArrays(GL_TRIANGLES, 0, m.vertices.size());

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (numRotations == 2)
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

	// Draw the groups generated from the XML file
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

// Function that processes and returns a vector of vertices from a file
std::vector<Vertex> getVerticesVector(std::ifstream &vertices) {
	char v[100];
	std::vector<Vertex> toAdd;
	while (vertices.getline(v, 100))
		toAdd.push_back(extractVertice(v));

	return toAdd;
}

int loadTexture(std::string s) {
	// setup - done once
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	
	// for each image
	unsigned int t;
	ilGenImages(1,&t);
	ilBindImage(t);
	ilLoadImage((ILstring)s.c_str());
	unsigned int tw = ilGetInteger(IL_IMAGE_WIDTH);
	unsigned int th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	unsigned char *texData = ilGetData();

	// create a texture slot
	unsigned int texID;
	glGenTextures(1, &texID);
	
	// bind the slot
	glBindTexture(GL_TEXTURE_2D, texID);

	// define texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	// send texture data to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

// Function that processes a group's information and returns an object with that info
Group processGroup(tinyxml2::XMLElement *group) {
	Group currentG;
	// Add all the attributes inside the current group to its "group" object
	for (tinyxml2::XMLElement *elem = group->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {
		std::string elemName = elem->Value();
		if (elemName == "translate") {
			if (elem->FloatAttribute("time") != 0) {
				Transformation t = {type: 'M', param1: elem->FloatAttribute("time")};
				for (tinyxml2::XMLElement *point = elem->FirstChildElement("point"); point != nullptr; point = point->NextSiblingElement("point")) {
					t.catmull.push_back(Vertex{point->FloatAttribute("X"), point->FloatAttribute("Y"), point->FloatAttribute("Z")});
				}
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
				Model mdl;
				mdl.textureIdx = -1;
				mdl.bufferIdx = currentModelIdx++;

				// Extract the vertices
				std::ifstream myfile;
				myfile.open(model->Attribute("file"));
				mdl.vertices = getVerticesVector(myfile);
				myfile.close();

				// Check if it has texture field
				const char *texture = model->Attribute("texture");
				if (texture) {
					mdl.textureIdx = loadTexture(texture);
				} else {
					// Extract the color of the vertices
					Color c = {model->FloatAttribute("diffR"), model->FloatAttribute("diffG"), model->FloatAttribute("diffB"),
							   model->FloatAttribute("specR"), model->FloatAttribute("specG"), model->FloatAttribute("specB"),
							   model->FloatAttribute("emisR"), model->FloatAttribute("emisG"), model->FloatAttribute("emisB"),
							   model->FloatAttribute("ambiR"), model->FloatAttribute("ambiG"), model->FloatAttribute("ambiB")};

					// Attribute a color to each of the vertices
					std::unordered_map<Vertex, Color, VertexHasher> vertColors;
					for (Vertex vertex : mdl.vertices) {
						Color clr;
						try {
							clr = vertColors.at(vertex);
						} catch(const std::exception& e) {
							if (c.diffR != 0 || c.diffG != 0 || c.diffB != 0) {
								float variation = (rand() / (float) RAND_MAX) / 5;
								clr = {c.diffR + variation, c.diffG + variation, c.diffB + variation};
							} else {
								clr = {rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX};
							}
							vertColors.insert(std::pair<Vertex,Color>(vertex, clr));
						}
						mdl.colors.push_back(clr);
					}
				}

				currentG.models.push_back(mdl);
				numModels++;
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

void fillBuffers(Group g) {
	for (Model m : g.models) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[m.bufferIdx]);
		glBufferData(GL_ARRAY_BUFFER, 2 * m.vertices.size() * 3 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m.vertices.size() * 3 * sizeof(GLfloat), m.vertices.data());
		glBufferSubData(GL_ARRAY_BUFFER, m.vertices.size() * 3 * sizeof(GLfloat), m.colors.size() * 3 * sizeof(GLfloat), m.colors.data());
	}

	for (Group sg : g.subGroups)
		fillBuffers(sg);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "Por favor, forneça um ficheiro XML\n";
		exit(EXIT_FAILURE);
	}

	// Textures
	glEnable(GL_TEXTURE_2D);

	// Processing XML file
	processXML(argv);

	// Init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Fase4TP - Engine");

	// Register the required callback 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(resizeWindow);

	// Resgister callback for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

#ifndef __APPLE__
	// Init Glew
	glewInit();
#endif

	// Init VBO's
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Generate the VBO buffers
	buffers = (GLuint *) malloc(numModels*sizeof(GLuint));
	glGenBuffers(numModels, buffers);

	// Put the data in the buffers
	fillBuffers(mainGroup);

	// Light
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	// Set OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glPointSize(3);

	// Enter GLUT's main cycle
	glutMainLoop();

	return 1;
}