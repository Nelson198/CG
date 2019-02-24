#include <iostream>
#include <fstream>

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

// FAZER AS DIVISIONS
// Function that prints a box's vertices to outFile
void printBox(int dimX, int dimY, int dimZ, int divisions) {
	// Bottom triangles
	printVertex(dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, -dimZ/2.0);

	printVertex(-dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, dimZ/2.0);

	// Lateral triangles
	printVertex(dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, -dimZ/2.0);

	printVertex(dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(dimX/2.0, dimY/2.0, -dimZ/2.0);

	printVertex(dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, -dimZ/2.0);

	printVertex(dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY/2.0, -dimZ/2.0);

	printVertex(-dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, dimZ/2.0);

	printVertex(-dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(-dimX/2.0, dimY/2.0, dimZ/2.0);

	printVertex(-dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, dimZ/2.0);

	printVertex(-dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, dimY/2.0, dimZ/2.0);

	// Top triangles
	printVertex(dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY/2.0, -dimZ/2.0);

	printVertex(-dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, dimY/2.0, dimZ/2.0);
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

int main(int argc, char **argv) {
	if (argc < 4) {
		std::cout << "Indique a forma geométrica que pretende gerar e o ficheiro de destino\n";
		exit(EXIT_FAILURE);
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

		int dimX = atoi(argv[2]), dimY = atoi(argv[3]), dimZ = atoi(argv[4]), divisions = 0;
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
