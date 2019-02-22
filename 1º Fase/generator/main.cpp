#include <iostream>
#include <fstream>

#include <math.h>

std::ofstream outFile;

// Function that prints a vertex's info to outFile as "x y z\n"
void printVertex(float x, float y, float z) {
	outFile << x << " " << y << " " << z << "\n";
}

// Function that prints a plane's info to outFile
void printPlane(int distX, int distZ) {
	printVertex(0, 0, -distZ / 2.0);
	printVertex(-distX / 2.0, 0, 0);
	printVertex(0, 0, distZ/2.0);

	printVertex(0, 0, distZ / 2.0);
	printVertex(-distX / 2.0, 0, 0);
	printVertex(0, 0, -distZ / 2.0);

	printVertex(0, 0, distZ/2.0);
	printVertex(distX/2.0, 0, 0);
	printVertex(0, 0, -distZ / 2.0);

	printVertex(0, 0, -distZ / 2.0);
	printVertex(distX / 2.0, 0, 0);
	printVertex(0, 0, distZ / 2.0);
}

// FAZER AS DIVISIONS
// Function that prints a box's info to outFile
void printBox(int dimX, int dimY, int dimZ, int divisions) {
	/* Triângulos da base */
	printVertex(dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, -dimZ/2.0);

	printVertex(-dimX/2.0, -dimY/2.0, -dimZ/2.0);
	printVertex(dimX/2.0, -dimY/2.0, dimZ/2.0);
	printVertex(-dimX/2.0, -dimY/2.0, dimZ/2.0);

	/* Triângulos laterais */
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

	/* Triângulos do topo */
	printVertex(dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY/2.0, -dimZ/2.0);

	printVertex(-dimX/2.0, dimY/2.0, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY/2.0, dimZ/2.0);
	printVertex(dimX/2.0, dimY/2.0, dimZ/2.0);
}

// Function that prints a sphere's info to outFile
void printSphere(int radius, int slices, int stacks) {
	double sliceDelta = 2 * M_PI / slices;
	double betaDelta = M_PI_2 / (stacks / 2);

	float r = radius;
	for(int j = 0; j < ((float)stacks)/2; j++) {
		float rnext = radius*cos(betaDelta*(j+1));
		float h = radius*sin(betaDelta*(j));
		float hnext = radius*sin(betaDelta*(j+1));
		for (float i = 0.5*(j%2); i < slices + 0.5*(j%2); i++) {
			// Festa dos tri�ngulos com os bicos para cima cupula superior
			printVertex(r*sin(i*sliceDelta), h, r*cos(i*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), h, r*cos((i + 1)*sliceDelta));
			printVertex(rnext*sin((i + 0.5)*sliceDelta), hnext, rnext*cos((i + 0.5)*sliceDelta));
		
			// Festa dos tri�ngulos com os bicos para cima cupula inferior
			printVertex(r*sin(i*sliceDelta), -h, r*cos(i*sliceDelta));
			printVertex(rnext*sin((i + 0.5)*sliceDelta), -hnext, rnext*cos((i + 0.5)*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), -h, r*cos((i + 1)*sliceDelta));

			i += 0.5;
			// Festa dos tri�ngulos com os bicos para baixo cupula superior
			printVertex(rnext*sin((i + 1)*sliceDelta), hnext, rnext*cos((i + 1)*sliceDelta));
			printVertex(rnext*sin(i*sliceDelta), hnext, rnext*cos(i*sliceDelta));
			printVertex(r*sin((i + 0.5)*sliceDelta), h, r*cos((i + 0.5)*sliceDelta));

			// Festa dos tri�ngulos com os bicos para baixo cupula inferior
			printVertex(rnext*sin((i + 1)*sliceDelta), -hnext, rnext*cos((i + 1)*sliceDelta));
			printVertex(r*sin((i + 0.5)*sliceDelta), -h, r*cos((i + 0.5)*sliceDelta));
			printVertex(rnext*sin(i*sliceDelta), -hnext, rnext*cos(i*sliceDelta));
			i -= 0.5;
		}
		r = rnext;
	}
}

// Function that prints a cone's info to outFile
void printCone(int bottomRadius, int height, int slices, int stacks) {
	double sliceDelta = 2 * M_PI / slices;

	float r = bottomRadius;
	for (float i = 0; i < slices; i++) {
		printVertex(0, 0, 0);
		printVertex(r*sin((i+1)*sliceDelta), 0, r*cos((i+1)*sliceDelta));
		printVertex(r*sin(i*sliceDelta), 0, r*cos(i*sliceDelta));
	}

	float rnext = r - (bottomRadius/((float)stacks));
	for(int j = 0; j < stacks; j++) {
		float h = j*(((float)height)/stacks);
		float hnext = (j+1)*(((float)height)/stacks);
		for (float i = 0.5*(j%2); i < slices + 0.5*(j%2); i++) {
			// Festa dos tri�ngulos com os bicos para cima
			printVertex(r*sin(i*sliceDelta), h, r*cos(i*sliceDelta));
			printVertex(r*sin((i + 1)*sliceDelta), h, r*cos((i + 1)*sliceDelta));
			printVertex(rnext*sin((i + 0.5)*sliceDelta), hnext, rnext*cos((i + 0.5)*sliceDelta));

			i += 0.5;
			// Festa dos tri�ngulos com os bicos para baixo
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
	if (argc < 5) {
		std::cout << "Indique a forma geométrica que pretende gerar e o ficheiro de destino\n";
		exit(EXIT_FAILURE);
	}

  	outFile.open(argv[argc-1]);
	if (std::string(argv[1]) == "plane") {
		if (argc != 5) {
			std::cout << "Número de argumentos para o plano incorreto\n";
			exit(EXIT_FAILURE);
		}

		int distx = atoi(argv[2]), distz = atoi(argv[3]);
		printPlane(distx, distz);
	} else if (std::string(argv[1]) == "box") {
		if (argc != 7 || argc != 6) {
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
