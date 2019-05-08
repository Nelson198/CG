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
#include <IL/il.h>

#include "tinyxml2.h"

// Structure that represents a 3D vertex
struct Vertex {
	float x, y, z;
};

// Structure that represents a color and all its components
struct Color {
	float diffR, diffG, diffB, diffA;
	float specR, specG, specB, specA;
	float ambiR, ambiG, ambiB, ambiA;
	float emisR, emisG, emisB, emisA;
};

// Structure that represents the coordinates of a texture
struct TexCoord {
	float s, t;
};

// Structure that represents a transformation and all its parameters
struct Transformation {
	char type;    // It can be: [TM] - Translate (Normal/Time), [AI] - Rotate (Angle/Time), S - Scale
	float param1; // These parameters depend on the type
	float param2;
	float param3;
	float param4;
	std::vector<Vertex> catmull;
};

// Structure that represents a model, i.e., all the information of a primite to be drawn
struct Model {
	int bufferIdx;
	int textureIdx;
	std::vector<Vertex> vertices;
	std::vector<Vertex> normals;
	std::vector<TexCoord> texCoords;
	Color color;
};

// Structure that represents a group composed of transformations, models and sub-groups
struct Group {
	std::vector<Transformation> trans;
	std::vector<Model> models;
	std::vector<Group> subGroups;
};

// Structure that represents a light to be displayed
struct Light {
	int idx;
	float x, y, z, w;
};

/// Global variables
// Group that contains the information of the entire scene
Group mainGroup;

// Variables related to camera and mouse movement
GLdouble dist = 50, beta = M_PI_4, alpha = M_PI_4, xd = 0, zd = 0;
int tracking = 0, startX, startY;

// Variables related to VBO's
GLuint *buffers;
int numModels = 0, currentModelIdx = 0;

// Vector that contains all the lights of the scene
std::vector<Light> sceneLights;
///

// Function that normalizes a 3D vector
void normalize(float* a) {
	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
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

// Function that returns the point in the catmull-rom curve given a time
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

// Function that returns the point in the catmull-rom curve given a time
Vertex getGlobalCatmullRomPoint(std::vector<Vertex> pontos, float elapsedTime, Vertex *deriv) {
    int POINT_COUNT = pontos.size();
    float t = elapsedTime * POINT_COUNT; // this is the real global t
    int index = floor(t);  // which segment
    t = t - index; // where within the segment

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
	// Apply all the transformations before drawing the models
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

	// Draw each model now that all transformations have been applied
	for (Model m : g.models) {
		if (m.textureIdx != -1)
			glBindTexture(GL_TEXTURE_2D, m.textureIdx);

		float diff[4] = {m.color.diffR, m.color.diffG, m.color.diffB, m.color.diffA};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
		float ambi[4] = {m.color.ambiR, m.color.ambiG, m.color.ambiB, m.color.ambiA};
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambi);
		float spec[4] = {m.color.specR, m.color.specG, m.color.specB, m.color.specA};
		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
		float emis[4] = {m.color.emisR, m.color.emisG, m.color.emisB, m.color.emisA};
		glMaterialfv(GL_FRONT, GL_EMISSION, emis);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[m.bufferIdx]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (void *) ((m.vertices.size())*3*sizeof(GLfloat)));
		if (m.textureIdx != -1) {
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, (void *) ((m.vertices.size() + m.normals.size())*3*sizeof(GLfloat)));
		}
		glDrawArrays(GL_TRIANGLES, 0, m.vertices.size());

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	// Revert the last rotation if there are two of them
	if (numRotations == 2)
		glRotatef(-timeRotationParams[0], timeRotationParams[1], timeRotationParams[2], timeRotationParams[3]);

	// Draw each of the sub-groups
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

	// Translate the world view to the desired location
	glTranslatef(xd, 0, zd);

	// Turn the lights on
	for (Light l : sceneLights) {
		float pos[4] = {l.x, l.y, l.z, l.w};
		glLightfv(GL_LIGHT0 + l.idx, GL_POSITION, pos);
	}

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
		case 's':
			xd -= deltaToMove;
			zd -= deltaToMove;
			break;

		// Move the vertices in the (-deltaToMove, 0, deltaToMove) direction
		case 'd':
			xd -= deltaToMove;
			zd += deltaToMove;
			break;

		// Move the vertices in the (deltaToMove, 0, deltaToMove) direction
		case 'w':
			xd += deltaToMove;
			zd += deltaToMove;
			break;

		// Move the vertices in the (deltaToMove, 0, -deltaToMove) direction
		case 'a':
			xd += deltaToMove;
			zd -= deltaToMove;
			break;

		// Move the camera closer to (0, 0, 0)
		case 'q':
			dist -= deltaToZoom;
			if (dist < 10)
				dist = 10;
			break;

		// Move the camera further from (0, 0, 0)
		case 'e':
			dist += deltaToZoom;
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
			if (beta > M_PI_2)
				beta = M_PI_2;
			break;

		// Move the camera down
		case GLUT_KEY_DOWN:
			beta -= deltaToMove;
			if (beta < -M_PI_2)
				beta = -M_PI_2;
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

// Function that creates a texCoord object from its string representation "s t"
TexCoord extractTexCoord(std::string str) {
	std::string delimiter = " ";
	float s, t;
	int pos;
	std::string token;

	// Process the s coordinate
	pos = str.find(delimiter);
	token = str.substr(0, pos);
	s = atof(token.c_str());
	str.erase(0, pos + delimiter.length());

	// Process the t coordinate
	pos = str.find(delimiter);
	token = str.substr(0, pos);
	t = atof(token.c_str());

	return TexCoord{s,t};
}

// Function that adds the vertices, the normals and the textures to a model from a file
void addModelInfo(std::ifstream &vertices, Model *mdl) {
	char v[100];
	int lineType = 0;
	while (vertices.getline(v, 100)) {
		if (lineType == 0)
			mdl->vertices.push_back(extractVertice(v));
		else if (lineType == 1)
			mdl->normals.push_back(extractVertice(v));
		else {
			mdl->texCoords.push_back(extractTexCoord(v));
			lineType = -1;
		}
		lineType++;
	}
}

// Function that loads a texture given its path
int loadTexture(std::string s) {
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	unsigned int t;
	ilGenImages(1,&t);
	ilBindImage(t);
	ilLoadImage((ILstring)s.c_str());
	unsigned int tw = ilGetInteger(IL_IMAGE_WIDTH);
	unsigned int th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	unsigned int texID;
	glGenTextures(1,&texID);

	glBindTexture(GL_TEXTURE_2D,texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

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
				addModelInfo(myfile, &mdl);

				myfile.close();

				// Check if it has texture field
				const char *texture = model->Attribute("texture");
				if (texture) {
					mdl.textureIdx = loadTexture(texture);
				}

				// Extract the color of the model
				float diff[4] = {model->FloatAttribute("diffR"), model->FloatAttribute("diffG"), model->FloatAttribute("diffB"), 1};
				if (diff[0] == 0 && diff[1] == 0 && diff[2] == 0) {
					diff[0] = 1;
					diff[1] = 1;
					diff[2] = 1;
				}
				float ambi[4] = {model->FloatAttribute("ambiR"), model->FloatAttribute("ambiG"), model->FloatAttribute("ambiB"), 1};
				float spec[4] = {model->FloatAttribute("specR"), model->FloatAttribute("specG"), model->FloatAttribute("specB"), 1};
				if (spec[0] == 0 && spec[1] == 0 && spec[2] == 0) {
					spec[0] = 1;
					spec[1] = 1;
					spec[2] = 1;
				}
				float emis[4] = {model->FloatAttribute("emisR"), model->FloatAttribute("emisG"), model->FloatAttribute("emisB"), 1};
				Color c = { diff[0], diff[1], diff[2], 1,
							ambi[0], ambi[1], ambi[2], 1,
							spec[0], spec[1], spec[2], 1,
							emis[0], emis[1], emis[2], 1};
				mdl.color = c;

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

	// Check if any light source is defined
	tinyxml2::XMLElement *lights = scene->FirstChildElement("lights");
	if (lights) {
		glEnable(GL_LIGHTING);

		int lightIdx = 0;
		for (tinyxml2::XMLElement *light = lights->FirstChildElement("light"); light != nullptr; light = light->NextSiblingElement("light")) {
			glEnable(GL_LIGHT0 + lightIdx);

			GLfloat amb[4] = {0, 0, 0, 1};
			GLfloat diff[4] = {1, 1, 1, 1};
			GLfloat spec[4] = {1, 1, 1, 1};
			glLightfv(GL_LIGHT0 + lightIdx, GL_AMBIENT, amb);
			glLightfv(GL_LIGHT0 + lightIdx, GL_DIFFUSE, diff);
			glLightfv(GL_LIGHT0 + lightIdx, GL_SPECULAR, spec);

			Light lt;
			std::string type = light->Attribute("type");
			if (type == "POINT") {
				lt.x = light->FloatAttribute("posX");
				lt.y = light->FloatAttribute("posY");
				lt.z = light->FloatAttribute("posZ");
				lt.w = 1;
			} else if (type == "DIRECTIONAL") {
				lt.x = light->FloatAttribute("dirX");
				lt.y = light->FloatAttribute("dirY");
				lt.z = light->FloatAttribute("dirZ");
				lt.w = 0;
			} else {
				printf("Tipo de luz desconhecida: %s\n", type.c_str());
				exit(EXIT_FAILURE);
			}
			sceneLights.push_back(lt);
			lightIdx++;
		}
	}

	// Process the scene element and all its children
	mainGroup = processGroup(scene);
}

// Function that fills the VBO's buffers for a given group and all its sub-groups
void fillBuffers(Group g) {
	for (Model m : g.models) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[m.bufferIdx]);
		glBufferData(GL_ARRAY_BUFFER, 3 * m.vertices.size() * 3 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m.vertices.size() * 3 * sizeof(GLfloat), m.vertices.data());
		glBufferSubData(GL_ARRAY_BUFFER, m.vertices.size() * 3 * sizeof(GLfloat), m.normals.size() * 3 * sizeof(GLfloat), m.normals.data());
		if (m.textureIdx != -1)
			glBufferSubData(GL_ARRAY_BUFFER, (m.vertices.size() + m.normals.size()) * 3 * sizeof(GLfloat), m.texCoords.size() * 2 * sizeof(GLfloat), m.texCoords.data());
	}

	for (Group sg : g.subGroups)
		fillBuffers(sg);
}

// Function that handles mouse clicks
void processMouseButtons(int button, int state, int xx, int yy) {
	if (state == GLUT_DOWN)  {
		// Mouse button pressed
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	} else if (state == GLUT_UP) {
		// Mouse button released
		double deltaX = (xx - startX)*M_PI/180;
		double deltaY = (yy - startY)*M_PI/180;
	
		if (tracking == 1) {
			alpha += deltaX;
			beta += deltaY;

			if (beta > M_PI_2)
				beta = M_PI_2;
			else if (beta < -M_PI_2)
				beta = -M_PI_2;
		} else if (tracking == 2) {
			dist -= (yy - startY);
			if (dist < 10)
				dist = 10;
		}
		tracking = 0;
	}
}

// Function that handles mouse movement
void processMouseMotion(int xx, int yy) {
	if (!tracking)
		return;

	double deltaX = (xx - startX)*M_PI/180;
	double deltaY = (yy - startY)*M_PI/180;

	if (tracking == 1) {
		// Mouse dragged while left button pressed (pan)
		alpha += deltaX;
		beta += deltaY;

		if (beta > M_PI_2)
			beta = M_PI_2;
		else if (beta < -M_PI_2)
			beta = -M_PI_2;
	} else if (tracking == 2) {
		// Mouse dragged while right button pressed (zoom)
		dist -= (yy - startY);
		if (dist < 10)
			dist = 10;
	}

	startX = xx;
	startY = yy;

	glutPostRedisplay();
}


int main(int argc, char **argv) {
	if (argc != 2) {
		std::cout << "Por favor, forneça um ficheiro XML\n";
		exit(EXIT_FAILURE);
	}

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

	// Resgister callback for keyboard and mouse processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

#ifndef __APPLE__
	// Init Glew
	glewInit();
#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Set OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Textures
	glEnable(GL_TEXTURE_2D);

	// Change the size of the points
	glPointSize(3);

	// Processing XML file
	processXML(argv);

	// Generate the VBO buffers
	buffers = (GLuint *) malloc(numModels*sizeof(GLuint));
	glGenBuffers(numModels, buffers);

	// Put the data in the buffers
	fillBuffers(mainGroup);

	// Enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
