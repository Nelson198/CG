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

// Function that prints a normal's coordinates to outFile as "x y z\n"
void printNormal(float x, float y, float z) {
	outFile << x << " " << y << " " << z << "\n";
}

// Function that prints a texture's coordinates to outFile as "x y\n"
void printTexCoord(float x, float y) {
	outFile << x << " " << y << "\n";
}

// Function that prints a plane's vertices to outFile
void printPlane(int dist) {
	// Top
	printVertex(dist/2.0, 0, 0);
	printNormal(0, 1, 0);
	printVertex(0, 0, -dist/2.0);
	printNormal(0, 1, 0);
	printVertex(-dist/2.0, 0, 0);
	printNormal(0, 1, 0);

	printVertex(-dist/2.0, 0, 0);
	printNormal(0, 1, 0);
	printVertex(0, 0, dist/2.0);
	printNormal(0, 1, 0);
	printVertex(dist/2.0, 0, 0);
	printNormal(0, 1, 0);

	// Bottom
	printVertex(dist/2.0, 0, 0);
	printNormal(0, -1, 0);
	printVertex(0, 0, dist/2.0);
	printNormal(0, -1, 0);
	printVertex(-dist/2.0, 0, 0);
	printNormal(0, -1, 0);

	printVertex(-dist/2.0, 0, 0);
	printNormal(0, -1, 0);
	printVertex(0, 0, -dist/2.0);
	printNormal(0, -1, 0);
	printVertex(dist/2.0, 0, 0);
	printNormal(0, -1, 0);
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
			printNormal(0, 1, 0);
			printVertex(xOffset, yOffset, nextZOffset);
			printNormal(0, 1, 0);
			printVertex(nextXOffset, yOffset, nextZOffset);
			printNormal(0, 1, 0);

			printVertex(nextXOffset, yOffset, nextZOffset);
			printNormal(0, 1, 0);
			printVertex(nextXOffset, yOffset, zOffset);
			printNormal(0, 1, 0);
			printVertex(xOffset, yOffset, zOffset);
			printNormal(0, 1, 0);

			// Lateral triangles
			// (+x, y, z)
			xOffset = dimX/2.0, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = dimZ/2.0 - j*dimZ/(float)divisions;
			nextYOffset = yOffset - dimY/(float)divisions, nextZOffset = zOffset - dimZ/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printNormal(1, 0, 0);
			printVertex(xOffset, nextYOffset, zOffset);
			printNormal(1, 0, 0);
			printVertex(xOffset, nextYOffset, nextZOffset);
			printNormal(1, 0, 0);

			printVertex(xOffset, yOffset, zOffset);
			printNormal(1, 0, 0);
			printVertex(xOffset, nextYOffset, nextZOffset);
			printNormal(1, 0, 0);
			printVertex(xOffset, yOffset, nextZOffset);
			printNormal(1, 0, 0);

			// (x, y, -z)
			xOffset = dimX/2.0 - j*dimX/(float)divisions, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = -dimZ/2.0;
			nextXOffset = xOffset - dimX/(float)divisions, nextYOffset = yOffset - dimY/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printNormal(0, 0, -1);
			printVertex(xOffset, nextYOffset, zOffset);
			printNormal(0, 0, -1);
			printVertex(nextXOffset, nextYOffset, zOffset);
			printNormal(0, 0, -1);

			printVertex(xOffset, yOffset, zOffset);
			printNormal(0, 0, -1);
			printVertex(nextXOffset, nextYOffset, zOffset);
			printNormal(0, 0, -1);
			printVertex(nextXOffset, yOffset, zOffset);
			printNormal(0, 0, -1);

			// (-x, y, z)
			xOffset = -dimX/2.0, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = -dimZ/2.0 + j*dimZ/(float)divisions;
			nextYOffset = yOffset - dimY/(float)divisions, nextZOffset = zOffset + dimZ/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printNormal(-1, 0, 0);
			printVertex(xOffset, nextYOffset, zOffset);
			printNormal(-1, 0, 0);
			printVertex(xOffset, nextYOffset, nextZOffset);
			printNormal(-1, 0, 0);

			printVertex(xOffset, yOffset, zOffset);
			printNormal(-1, 0, 0);
			printVertex(xOffset, nextYOffset, nextZOffset);
			printNormal(-1, 0, 0);
			printVertex(xOffset, yOffset, nextZOffset);
			printNormal(-1, 0, 0);

			// (x, y, +z)
			xOffset = -dimX/2.0 + j*dimX/(float)divisions, yOffset = dimY/2.0 - i*dimY/(float)divisions, zOffset = dimZ/2.0;
			nextXOffset = xOffset + dimX/(float)divisions, nextYOffset = yOffset - dimY/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printNormal(0, 0, 1);
			printVertex(xOffset, nextYOffset, zOffset);
			printNormal(0, 0, 1);
			printVertex(nextXOffset, nextYOffset, zOffset);
			printNormal(0, 0, 1);

			printVertex(xOffset, yOffset, zOffset);
			printNormal(0, 0, 1);
			printVertex(nextXOffset, nextYOffset, zOffset);
			printNormal(0, 0, 1);
			printVertex(nextXOffset, yOffset, zOffset);
			printNormal(0, 0, 1);

			// Bottom triangles
			// (x, -y, z)
			xOffset = dimX/2.0 - i*dimX/(float)divisions, yOffset = -dimY/2.0, zOffset = dimZ/2.0 - j*dimZ/(float)divisions;
			nextXOffset = xOffset - dimX/(float)divisions, nextZOffset = zOffset - dimZ/(float)divisions;
			printVertex(xOffset, yOffset, zOffset);
			printNormal(0, -1, 0);
			printVertex(nextXOffset, yOffset, nextZOffset);
			printNormal(0, -1, 0);
			printVertex(xOffset, yOffset, nextZOffset);
			printNormal(0, -1, 0);

			printVertex(nextXOffset, yOffset, nextZOffset);
			printNormal(0, -1, 0);
			printVertex(xOffset, yOffset, zOffset);
			printNormal(0, -1, 0);
			printVertex(nextXOffset, yOffset, zOffset);
			printNormal(0, -1, 0);
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
		for (float i = 0; i < slices; i++) {
			// Triangles with the tip pointing up of the upper dome
			printVertex(r*sin(i*sliceDelta), height, r*cos(i*sliceDelta));
			printNormal(r*sin(i*sliceDelta), height, r*cos(i*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), height, r*cos((i + 1)*sliceDelta));
			printNormal(r*sin((i + 1)*sliceDelta), height, r*cos((i + 1)*sliceDelta));
			printVertex(nextR*sin((i + 1)*sliceDelta), nextHeight, nextR*cos((i + 1)*sliceDelta));
			printNormal(nextR*sin((i + 1)*sliceDelta), nextHeight, nextR*cos((i + 1)*sliceDelta));

			// Triangles with the tip pointing up of the lower dome
			printVertex(r*sin(i*sliceDelta), -height, r*cos(i*sliceDelta));
			printNormal(r*sin(i*sliceDelta), -height, r*cos(i*sliceDelta));
			printVertex(nextR*sin((i + 1)*sliceDelta), -nextHeight, nextR*cos((i + 1)*sliceDelta));
			printNormal(nextR*sin((i + 1)*sliceDelta), -nextHeight, nextR*cos((i + 1)*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), -height, r*cos((i + 1)*sliceDelta));
			printNormal(r*sin((i + 1)*sliceDelta), -height, r*cos((i + 1)*sliceDelta));

			// Triangles with the tip pointing down of the upper dome
			printVertex(nextR*sin((i + 1)*sliceDelta), nextHeight, nextR*cos((i + 1)*sliceDelta));
			printNormal(nextR*sin((i + 1)*sliceDelta), nextHeight, nextR*cos((i + 1)*sliceDelta));
			printVertex(nextR*sin(i*sliceDelta), nextHeight, nextR*cos(i*sliceDelta));
			printNormal(nextR*sin(i*sliceDelta), nextHeight, nextR*cos(i*sliceDelta));
			printVertex(r*sin(i*sliceDelta), height, r*cos(i*sliceDelta));
			printNormal(r*sin(i*sliceDelta), height, r*cos(i*sliceDelta));

			// Triangles with the tip pointing up of the lower dome
			printVertex(nextR*sin((i + 1)*sliceDelta), -nextHeight, nextR*cos((i + 1)*sliceDelta));
			printNormal(nextR*sin((i + 1)*sliceDelta), -nextHeight, nextR*cos((i + 1)*sliceDelta));
			printVertex(r*sin(i*sliceDelta), -height, r*cos(i*sliceDelta));
			printNormal(r*sin(i*sliceDelta), -height, r*cos(i*sliceDelta));
			printVertex(nextR*sin(i*sliceDelta), -nextHeight, nextR*cos(i*sliceDelta));
			printNormal(nextR*sin(i*sliceDelta), -nextHeight, nextR*cos(i*sliceDelta));
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
		printNormal(0, -1, 0);
		printVertex(r*sin((i+1)*sliceDelta), 0, r*cos((i+1)*sliceDelta));
		printNormal(0, -1, 0);
		printVertex(r*sin(i*sliceDelta), 0, r*cos(i*sliceDelta));
		printNormal(0, -1, 0);
	}

	float rnext = r - (bottomRadius/((float)stacks));
	// Draw each stack, one at a time
	for(int j = 0; j < stacks; j++) {
		float h = j*(((float)height)/stacks);
		float hnext = (j+1)*(((float)height)/stacks);

		// COMPLETAR NORMAIS ! COMPLETAR NORMAIS ! COMPLETAR NORMAIS ! COMPLETAR NORMAIS ! COMPLETAR NORMAIS !

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

// Function that multiplies a matrix by a vector and puts the result in res
void multMatrixVector(float* m, float* v, float* res)
{
	for (int j = 0; j < 4; j++) {
		res[j] = 0;

		for (int k = 0; k < 4; k++) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}
}

// Bezier's matrix
float bezierMatrix[4][4] = { { -1.0f, 3.0f, -3.0f, 1.0f },
                             { 3.0f, -6.0f, 3.0f, 0.0f },
                             { -3.0f, 3.0f, 0.0f, 0.0f },
							 { 1.0f, 0.0f, 0.0f, 0.0f } };

// Funct5ion that returns a given Bezier's point
void getBezierPoint(float u, float v, float** pMatrixX, float** pMatrixY, float** pMatrixZ, float* pos) {
	float uVec[4] = { u * u * u, u * u, u, 1 };
	float vVec[4] = { v * v * v, v * v, v, 1 };

	float mvVec[4];
	float px[4];
	float py[4];
	float pz[4];

	float mx[4];
	float my[4];
	float mz[4];

	// Calcula M (transposta) * V
	multMatrixVector((float*)bezierMatrix, vVec, mvVec);

	// Calcula P * resultado anterior
	multMatrixVector((float*)pMatrixX, mvVec, px);
	multMatrixVector((float*)pMatrixY, mvVec, py);
	multMatrixVector((float*)pMatrixZ, mvVec, pz);

	// Calcula M * resultado anterior
	multMatrixVector((float*)bezierMatrix, px, mx);
	multMatrixVector((float*)bezierMatrix, py, my);
	multMatrixVector((float*)bezierMatrix, pz, mz);

	// Calcula U * resultado anterior
	pos[0] = (uVec[0] * mx[0]) + (uVec[1] * mx[1]) + (uVec[2] * mx[2]) + (uVec[3] * mx[3]);
	pos[1] = (uVec[0] * my[0]) + (uVec[1] * my[1]) + (uVec[2] * my[2]) + (uVec[3] * my[3]);
	pos[2] = (uVec[0] * mz[0]) + (uVec[1] * mz[1]) + (uVec[2] * mz[2]) + (uVec[3] * mz[3]);
}

// Function that generates the points for a Bezier's patch
void generateBezierPatches(std::vector<CP> pVertices, std::vector<int> pIndexes, int t, std::vector<CP> *vertices, std::vector<int> *indexes) {
	float pMatrixX[4][4];
	float pMatrixY[4][4];
	float pMatrixZ[4][4];

	for (int start = 0; start < pIndexes.size(); start += 16) {
		for (int i = 0; i <= t; i++) {
			float u = ((float)i) / ((float)t);

			for (int j = 0; j <= t; j++) {
				float v = ((float)j) / ((float)t);

				for (int w = 0; w < 4; w++) {
					for (int z = 0; z < 4; z++) {
						pMatrixX[w][z] = pVertices.at(pIndexes.at(start + w * 4 + z)).x;
						pMatrixY[w][z] = pVertices.at(pIndexes.at(start + w * 4 + z)).y;
						pMatrixZ[w][z] = pVertices.at(pIndexes.at(start + w * 4 + z)).z;
					}
				}

				float pos[3];
				getBezierPoint(u, v, (float**)pMatrixX, (float**)pMatrixY, (float**)pMatrixZ, pos);

				vertices->push_back(CP{pos[0], pos[1], pos[2]});
			}
		}
	}

	for (int start = 0; start < pIndexes.size() / 16; start++) {
		int patch = (t + 1) * (t + 1) * start;

		for (int i = 0; i < t; i++) {
			for (int j = 0; j < t; j++) {
				indexes->push_back(patch + ((t + 1) * i) + j);
				indexes->push_back(patch + (t + 1) * (i + 1) + j + 1);
				indexes->push_back(patch + ((t + 1) * i) + j + 1);

				indexes->push_back(patch + ((t + 1) * i) + j);
				indexes->push_back(patch + (t + 1) * (i + 1) + j);
				indexes->push_back(patch + (t + 1) * (i + 1) + j + 1);
			}
		}
	}
}

// Function that prints a Bezier patches' vertices to outFile
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
	std::vector<int> patches;
	patches.reserve(numPatches*16);
	for (int i = 0; i < numPatches; i++) {
		patchFile.getline(line, 1000);

		std::string commaSepPatches(line);
		std::string delimiter = ", ";
		int pos = 0;
		std::string token;
		while ((pos = commaSepPatches.find(delimiter)) != std::string::npos) {
			token = commaSepPatches.substr(0, pos);
			patches.push_back(std::stoi(token));
			commaSepPatches.erase(0, pos + delimiter.length());
		}
		patches.push_back(std::stoi(commaSepPatches));
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

	std::vector<CP> vertices;
	std::vector<int> indexes;
	generateBezierPatches(controlPoints, patches, tesselation, &vertices, &indexes);

	for (int idx : indexes) {
		CP c = vertices.at(idx);
		printVertex(c.x, c.y, c.z);
		// COMPLETAR NORMAL ! COMPLETAR NORMAL ! COMPLETAR NORMAL ! COMPLETAR NORMAL ! COMPLETAR NORMAL !
		// printNormal();
	}

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
