#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

std::ofstream outFile;

// Function that prints a vertex's coordinates to outFile as "x y z\n"
void printVertex(float x, float y, float z) {
	outFile << x << " " << y << " " << z << "\n";
}

// Function that prints a plane's vertices to outFile
void printPlane(int dist) {
	printVertex(dist/2.0, 0, 0);
	printVertex(0, 0, -dist/2.0);
	printVertex(-dist/2.0, 0, 0);

	printVertex(-dist/2.0, 0, 0);
	printVertex(0, 0, dist/2.0);
	printVertex(dist/2.0, 0, 0);

	printVertex(dist/2.0, 0, 0);
	printVertex(0, 0, dist/2.0);
	printVertex(-dist/2.0, 0, 0);

	printVertex(-dist/2.0, 0, 0);
	printVertex(0, 0, -dist/2.0);
	printVertex(dist/2.0, 0, 0);
}

// Function that prints a box's vertices to outFile
void printBox(int dimX, int dimY, int dimZ, int divisions) {
	for (int i = 0; i < divisions; i++) {
		for (int j = 0; j < divisions; j++) {
			double xOffset, yOffset, zOffset, nextXOffset, nextYOffset, nextZOffset;;

			// Top triangles
			// (x, +y, z)
			xOffset = dimX/2.0 - i*dimX/(float)divisions, yOffset = dimY/2.0, zOffset = dimZ/2.0 - j*dimZ/(float)divisions;
			nextXOffset = xOffset - dimX/(float)divisions, nextZOffset = zOffset - dimZ/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printVertex(xOffset, yOffset, nextZOffset);
			printVertex(nextXOffset, yOffset, nextZOffset);

			printVertex(nextXOffset, yOffset, nextZOffset);
			printVertex(nextXOffset, yOffset, zOffset);
			printVertex(xOffset, yOffset, zOffset);

			// Lateral triangles
			// (+x, y, z)
			xOffset = dimX/2.0, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = dimZ/2.0 - j*dimZ/(float)divisions;
			nextYOffset = yOffset - dimY/(float)divisions, nextZOffset = zOffset - dimZ/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printVertex(xOffset, nextYOffset, zOffset);
			printVertex(xOffset, nextYOffset, nextZOffset);

			printVertex(xOffset, yOffset, zOffset);
			printVertex(xOffset, nextYOffset, nextZOffset);
			printVertex(xOffset, yOffset, nextZOffset);

			// (x, y, -z)
			xOffset = dimX/2.0 - j*dimX/(float)divisions, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = -dimZ/2.0;
			nextXOffset = xOffset - dimX/(float)divisions, nextYOffset = yOffset - dimY/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printVertex(xOffset, nextYOffset, zOffset);
			printVertex(nextXOffset, nextYOffset, zOffset);

			printVertex(xOffset, yOffset, zOffset);
			printVertex(nextXOffset, nextYOffset, zOffset);
			printVertex(nextXOffset, yOffset, zOffset);

			// (-x, y, z)
			xOffset = -dimX/2.0, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = -dimZ/2.0 + j*dimZ/(float)divisions;
			nextYOffset = yOffset - dimY/(float)divisions, nextZOffset = zOffset + dimZ/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printVertex(xOffset, nextYOffset, zOffset);
			printVertex(xOffset, nextYOffset, nextZOffset);

			printVertex(xOffset, yOffset, zOffset);
			printVertex(xOffset, nextYOffset, nextZOffset);
			printVertex(xOffset, yOffset, nextZOffset);

			// (x, y, +z)
			xOffset = -dimX/2.0 + j*dimX/(float)divisions, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = dimZ/2.0;
			nextXOffset = xOffset + dimX/(float)divisions, nextYOffset = yOffset - dimY/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printVertex(xOffset, nextYOffset, zOffset);
			printVertex(nextXOffset, nextYOffset, zOffset);

			printVertex(xOffset, yOffset, zOffset);
			printVertex(nextXOffset, nextYOffset, zOffset);
			printVertex(nextXOffset, yOffset, zOffset);

			// Bottom triangles
			// (x, -y, z)
			xOffset = dimX/2.0 - i*dimX/(float)divisions, yOffset = -dimY/2.0, zOffset = dimZ/2.0 - j*dimZ/(float)divisions;
			nextXOffset = xOffset - dimX/(float)divisions, nextZOffset = zOffset - dimZ/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printVertex(nextXOffset, yOffset, nextZOffset);
			printVertex(xOffset, yOffset, nextZOffset);

			printVertex(nextXOffset, yOffset, nextZOffset);
			printVertex(xOffset, yOffset, zOffset);
			printVertex(nextXOffset, yOffset, zOffset);
		}
	}
}

// Function that prints a sphere's vertices to outFile
void printSphere(int radius, int slices, int stacks) {
	// Angle of a single slice
	double sliceDelta = 2 * M_PI / slices;
	// Angle of a single stack
	double betaDelta = M_PI_2 / (stacks / 2);

	float r = radius;
	// Generate the lower and upper stacks at the same time
	for (int j = 0; j < ((float)stacks)/2; j++) {
		float nextR = radius*cos(betaDelta*(j+1));
		float height = radius*sin(betaDelta*(j));
		float nextHeight = radius*sin(betaDelta*(j+1));

		// The 0.5 adjustement in the for loop is to alternate between the triangles of
		// adjacent stacks, so that they align correctly with each other
		for (float i = 0.5*(j%2); i < slices + 0.5*(j%2); i++) {
			// Triangles with the tip pointing up of the upper dome
			printVertex(r*sin(i*sliceDelta), height, r*cos(i*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), height, r*cos((i + 1)*sliceDelta));
			printVertex(nextR*sin((i + 0.5)*sliceDelta), nextHeight, nextR*cos((i + 0.5)*sliceDelta));

			// Triangles with the tip pointing up of the lower dome
			printVertex(r*sin(i*sliceDelta), -height, r*cos(i*sliceDelta));
			printVertex(nextR*sin((i + 0.5)*sliceDelta), -nextHeight, nextR*cos((i + 0.5)*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), -height, r*cos((i + 1)*sliceDelta));


			// This 0.5 adjustement (and the one below) is to offset the triangles in the same
			// stack, so that they don't end up on top of each other
			i += 0.5;

			// Triangles with the tip pointing down of the upper dome
			printVertex(nextR*sin((i + 1)*sliceDelta), nextHeight, nextR*cos((i + 1)*sliceDelta));
			printVertex(nextR*sin(i*sliceDelta), nextHeight, nextR*cos(i*sliceDelta));
			printVertex(r*sin((i + 0.5)*sliceDelta), height, r*cos((i + 0.5)*sliceDelta));

			// Triangles with the tip pointing up of the lower dome
			printVertex(nextR*sin((i + 1)*sliceDelta), -nextHeight, nextR*cos((i + 1)*sliceDelta));
			printVertex(r*sin((i + 0.5)*sliceDelta), -height, r*cos((i + 0.5)*sliceDelta));
			printVertex(nextR*sin(i*sliceDelta), -nextHeight, nextR*cos(i*sliceDelta));

			i -= 0.5;
		}
		r = nextR;
	}
}

// Function that prints a cone's vertices to outFile
void printCone(int bottomRadius, int height, int slices, int stacks) {
	// Angle of a single slice
	double sliceDelta = 2 * M_PI / slices;

	float r = bottomRadius;
	// Draw the bottom circle
	for (float i = 0; i < slices; i++) {
		printVertex(0, 0, 0);
		printVertex(r*sin((i+1)*sliceDelta), 0, r*cos((i+1)*sliceDelta));
		printVertex(r*sin(i*sliceDelta), 0, r*cos(i*sliceDelta));
	}

	float rnext = r - (bottomRadius/((float)stacks));
	// Draw each stack, one at a time
	for(int j = 0; j < stacks; j++) {
		float h = j*(((float)height)/stacks);
		float hnext = (j+1)*(((float)height)/stacks);

		// The 0.5 adjustement in the for loop is to alternate between the triangles of
		// adjacent stacks, so that they align correctly with each other
		for (float i = 0.5*(j%2); i < slices + 0.5*(j%2); i++) {
			// Triangles with the tip pointing up
			printVertex(r*sin(i*sliceDelta), h, r*cos(i*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), h, r*cos((i + 1)*sliceDelta));
			printVertex(rnext*sin((i + 0.5)*sliceDelta), hnext, rnext*cos((i + 0.5)*sliceDelta));

			// This 0.5 adjustement (and the one below) is to offset the triangles in the same
			// stack, so that they don't end up on top of each other
			i += 0.5;

			// Triangles with the tip pointing down
			printVertex(rnext*sin((i + 1)*sliceDelta), hnext, rnext*cos((i + 1)*sliceDelta));
			printVertex(rnext*sin(i*sliceDelta), hnext, rnext*cos(i*sliceDelta));
			printVertex(r*sin((i + 0.5)*sliceDelta), h, r*cos((i + 0.5)*sliceDelta));
			i -= 0.5;
		}
		r = rnext;
		rnext -= (bottomRadius/((float)stacks));
	}
}

/* <------------------------------------------------ Bézier patches ------------------------------------------------> */

/* Struct of control points */
struct CP {
	float x;
	float y;
	float z;
};

/* Compute the position of a point along a Bezier curve at t [0:1] */
float bezierCurve(float P[3], float t) 
{ 
    float b0 = (1 - t) * (1 - t) * (1 - t); 
    float b1 = 3 * t * (1 - t) * (1 - t); 
    float b2 = 3 * t * t * (1 - t); 
    float b3 = t * t * t;

    return P[0] * b0 + P[1] * b1 + P[2] * b2 + P[3] * b3; 
}

float bezierDerivative(float P[3], float t) 
{ 
    return -3 * (1 - t) * (1 - t) * P[0] + 
           (3 * (1 - t) * (1 - t) - 6 * t * (1 - t)) * P[1] + 
           (6 * t * (1 - t) - 3 * t * t) * P[2] + 
            3 * t * t * P[3]; 
}
 
float bezierPatch(float *controlPoints, float u, float v) 
{ 
    float uCurve[4]; 
    for (int i = 0; i < 4; i++) {
		uCurve[i] = bezierCurve(controlPoints + 4 * i, u); 
	}
	return bezierCurve(uCurve, v); 
}

/* Compute the derivative of a point on Bezier patch along the u parametric direction */
float bezierDU(float *controlPoints, float u, float v) 
{ 
    float P[4]; 
    float vCurve[4]; 
    for (int i = 0; i < 4; ++i) { 
        P[0] = controlPoints[i]; 
        P[1] = controlPoints[4 + i]; 
        P[2] = controlPoints[8 + i]; 
        P[3] = controlPoints[12 + i]; 
        vCurve[i] = bezierCurve(P, v); 
    } 
    return bezierDerivative(vCurve, u); 
}

/* Compute the derivative of a point on Bezier patch along the v parametric direction */
float bezierDV(float *controlPoints, float u, float v) 
{ 
    float uCurve[4];
    for (int i = 0; i < 4; ++i) { 
        uCurve[i] = bezierCurve(controlPoints + 4 * i, u); 
    } 
    return bezierDerivative(uCurve, v); 
}

/* Generate a list of triangles to draw the Bézier surface */
void bezierGenerate(std::vector<std::vector<int>> patches, std::vector<CP> controlPoints) {

}

void printBezier(char *filePatch, int tesselation) {
	// Open a file with the same name (but ending in ".3d") to output the triangles to
	std::ifstream patchFile;
	patchFile.open(filePatch);
	char *fileOut = (char *) malloc(strlen(filePatch)-2);
	strncpy(fileOut, filePatch, strlen(filePatch)-5);
	strcat(fileOut, "3d");
	outFile.open(fileOut);

	char *line = (char *) malloc(1000);

	// Get the patches
	patchFile.getline(line, 1000);
	int numPatches = atoi(line);
	std::vector<std::vector<int>> patches;
	patches.reserve(numPatches);
	for (int i = 0; i < numPatches; i++) {
		patchFile.getline(line, 1000);

		std::vector<int> toAdd = std::vector<int>();
		std::string commaSepPatches(line);
		std::string delimiter = ", ";
		int pos = 0;
		std::string token;
		while ((pos = commaSepPatches.find(delimiter)) != std::string::npos) {
			token = commaSepPatches.substr(0, pos);
			toAdd.push_back(std::stoi(token));
			commaSepPatches.erase(0, pos + delimiter.length());
		}
		toAdd.push_back(std::stoi(commaSepPatches));

		patches.push_back(toAdd);
	}

	// Get the control points
	patchFile.getline(line, 1000);
	int numControlPoints = atoi(line);
	std::vector<CP> controlPoints;
	controlPoints.reserve(numControlPoints);

	for (int i = 0; i < numControlPoints; i++) {
		patchFile.getline(line, 1000);

		std::string delimiter = ", ";
		float x, y, z;
		int pos;
		std::string commaSepCPs(line);
		std::string token;

		// Process the x coordinate
		pos = commaSepCPs.find(delimiter);
		token = commaSepCPs.substr(0, pos);
		x = atof(token.c_str());
		commaSepCPs.erase(0, pos + delimiter.length());

		// Process the y coordinate
		pos = commaSepCPs.find(delimiter);
		token = commaSepCPs.substr(0, pos);
		y = atof(token.c_str());
		commaSepCPs.erase(0, pos + delimiter.length());

		// Process the z coordinate
		pos = commaSepCPs.find(delimiter);
		token = commaSepCPs.substr(0, pos);
		z = atof(token.c_str());

		controlPoints.push_back(CP{x, y, z});
	}

	bezierGenerate(patches, controlPoints);

	///// FALTA USAR A TESSELATION PARA A IMAGEM FICAR MELHOR /////
	/*for (auto patch : patches) {
		for (int i = 0; i < patch.size()-2; i++) {
			CP aux = controlPoints.at(patch.at(i));
			printVertex(aux.x, aux.y, aux.z);

			aux = controlPoints.at(patch.at(i+1));
			printVertex(aux.x, aux.y, aux.z);

			aux = controlPoints.at(patch.at(i+2));
			printVertex(aux.x, aux.y, aux.z);
		}
	}*/

	patchFile.close();
	outFile.close();
}

int main(int argc, char **argv) {
	if (argc < 4) {
		std::cout << "Indique a forma geométrica que pretende gerar e o ficheiro de destino\n";
		exit(EXIT_FAILURE);
	}

	if (std::string(argv[1]) == "bezier") {
		if (argc != 4) {
			std::cout << "Número de argumentos para a patch de Bezier incorreto\n";
			exit(EXIT_FAILURE);
		}

		char *filePatch = (char*) malloc(strlen(argv[2])+1);
		strcpy(filePatch, argv[2]);
		int tesselation = atoi(argv[3]);
		printBezier(filePatch, tesselation);
		return 0;
	}

  	outFile.open(argv[argc-1]);
	if (std::string(argv[1]) == "plane") {
		if (argc != 4) {
			std::cout << "Número de argumentos para o plano incorreto\n";
			exit(EXIT_FAILURE);
		}

		int dist = atoi(argv[2]);
		printPlane(dist);
	} else if (std::string(argv[1]) == "box") {
		if (argc != 7 && argc != 6) {
			std::cout << "Número de argumentos para a caixa incorreto\n";
			exit(EXIT_FAILURE);
		}

		int dimX = atoi(argv[2]), dimY = atoi(argv[3]), dimZ = atoi(argv[4]), divisions = 1;
		if (argc == 7)
			divisions = atoi(argv[5]);
		printBox(dimX, dimY, dimZ, divisions);
	} else if (std::string(argv[1]) == "sphere") {
		if (argc != 6) {
			std::cout << "Número de argumentos para a esfera incorreto\n";
			exit(EXIT_FAILURE);
		}

		int radius = atoi(argv[2]), slices = atoi(argv[3]), stacks = atoi(argv[4]);
		printSphere(radius, slices, stacks);
	} else if (std::string(argv[1]) == "cone") {
		if (argc != 7) {
			std::cout << "Número de argumentos para o cone incorreto\n";
			exit(EXIT_FAILURE);
		}

		int bottomRadius = atoi(argv[2]), height = atoi(argv[3]), slices = atoi(argv[4]), stacks = atoi(argv[5]);
		printCone(bottomRadius, height, slices, stacks);
	} else {
		std::cout << "Figura geométrica não suportada\n";
		exit(EXIT_FAILURE);
	}
  	outFile.close();

	return 1;
}
