#include <iostream>
#include <fstream>

using namespace std;

ofstream myfile;

void printVertex(float x, float y, float z) {
	myfile << x << " " << y << " " << z << "\n";
}

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

void printBox(int dimX, int dimY, int dimZ, int divisions) {
	/* Triângulos da base */
	printVertex(dimX/2.0, 0, dimZ/2.0);
	printVertex(-dimX/2.0, 0, -dimZ/2.0);
	printVertex(dimX/2.0, 0, -dimZ/2.0);

	printVertex(-dimX/2.0, 0, -dimZ/2.0);
	printVertex(dimX/2.0, 0, dimZ/2.0);
	printVertex(-dimX/2.0, 0, dimZ/2.0);

	/* Triângulos laterais */
	printVertex(dimX/2.0, dimY, dimZ/2.0);
	printVertex(dimX/2.0, 0, dimZ/2.0);
	printVertex(dimX/2.0, 0, -dimZ/2.0);

	printVertex(dimX/2.0, dimY, dimZ/2.0);
	printVertex(dimX/2.0, 0, -dimZ/2.0);
	printVertex(dimX/2.0, dimY, -dimZ/2.0);

	printVertex(dimX/2.0, dimY, -dimZ/2.0);
	printVertex(dimX/2.0, 0, -dimZ/2.0);
	printVertex(-dimX/2.0, 0, -dimZ/2.0);

	printVertex(dimX/2.0, dimY, -dimZ/2.0);
	printVertex(-dimX/2.0, 0, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY, -dimZ/2.0);

	printVertex(-dimX/2.0, dimY, -dimZ/2.0);
	printVertex(-dimX/2.0, 0, -dimZ/2.0);
	printVertex(-dimX/2.0, 0, dimZ/2.0);

	printVertex(-dimX/2.0, dimY, -dimZ/2.0);
	printVertex(-dimX/2.0, 0, dimZ/2.0);
	printVertex(-dimX/2.0, dimY, dimZ/2.0);

	printVertex(-dimX/2.0, dimY, dimZ/2.0);
	printVertex(-dimX/2.0, 0, dimZ/2.0);
	printVertex(dimX/2.0, 0, dimZ/2.0);

	printVertex(-dimX/2.0, dimY, dimZ/2.0);
	printVertex(dimX/2.0, 0, dimZ/2.0);
	printVertex(dimX/2.0, dimY, dimZ/2.0);

	/* Triângulos do topo */
	printVertex(dimX/2.0, dimY, dimZ/2.0);
	printVertex(dimX/2.0, dimY, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY, -dimZ/2.0);

	printVertex(-dimX/2.0, dimY, -dimZ/2.0);
	printVertex(-dimX/2.0, dimY, dimZ/2.0);
	printVertex(dimX/2.0, dimY, dimZ/2.0);
}

void printSphere(int radius, int slices, int stacks) {

}

void printCone(int bottomRadius, int height, int slices, int stacks) {

}

/*
void drawCylinder(float radius, float height, int slices) {
	// put code to draw cylinder in here
	GLdouble alphaDelta = 2 * M_PI / slices;

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < slices; i++) {
		
		// Circunfer�ncia de cima
		glColor3f(1, 0.5, 0);
		glVertex3f(0, height, 0);
		glVertex3f(radius*sin(i*alphaDelta), height, radius*cos(i*alphaDelta));
		glVertex3f(radius*sin((i + 1)*alphaDelta), height, radius*cos((i + 1)*alphaDelta));

		// Festa dos tri�ngulos com os bicos para baixo
		glColor3f(0.4, 0.4, 0.4);
		glVertex3f(radius*sin((i + 1)*alphaDelta), height, radius*cos((i + 1)*alphaDelta));
		glVertex3f(radius*sin(i*alphaDelta), height, radius*cos(i*alphaDelta));
		glVertex3f(radius*sin((i + 0.5)*alphaDelta), 0, radius*cos((i + 0.5)*alphaDelta));
	}

	for (GLdouble i = 0.5; i < slices + 0.5; i++) {
		
		// Festa dos tri�ngulos com os bicos para cima
		glColor3f(0.6, 0.6, 0.6);
		glVertex3f(radius*sin(i*alphaDelta), 0, radius*cos(i*alphaDelta));
		glVertex3f(radius*sin((i + 1)*alphaDelta), 0, radius*cos((i + 1)*alphaDelta));
		glVertex3f(radius*sin((i + 0.5)*alphaDelta), height, radius*cos((i + 0.5)*alphaDelta));
		
		// Circunfer�ncia de baixo
		glColor3f(0, 0.5, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(radius*sin(i*alphaDelta), 0, radius*cos(i*alphaDelta));
		glVertex3f(radius*sin((i - 1)*alphaDelta), 0, radius*cos((i - 1)*alphaDelta));
	}

	glEnd();
} */

int main(int argc, char **argv) {
	if (argc < 5) {
		cout << "Indique a forma geométrica que pretende gerar e o ficheiro de destino\n";
		exit(EXIT_FAILURE);
	}

  	myfile.open(argv[argc-1]);
	if (string(argv[1]) == "plane") {
		if (argc != 5) {
			cout << "Número de argumentos para o plano incorreto\n";
			exit(EXIT_FAILURE);
		}

		int distx = atoi(argv[2]), distz = atoi(argv[3]);
		printPlane(distx, distz);
	} else if (string(argv[1]) == "box") {
		if (argc != 7) {
			cout << "Número de argumentos para a caixa incorreto\n";
			exit(EXIT_FAILURE);
		}

		int dimX = atoi(argv[2]), dimY = atoi(argv[3]), dimZ = atoi(argv[4]), divisions = atoi(argv[5]);
		printBox(dimX, dimY, dimZ, divisions);
	} else if (string(argv[1]) == "sphere") {
		if (argc != 6) {
			cout << "Número de argumentos para a esfera incorreto\n";
			exit(EXIT_FAILURE);
		}

		int radius = atoi(argv[2]), slices = atoi(argv[3]), stacks = atoi(argv[4]);
		printSphere(radius, slices, stacks);
	} else if (string(argv[1]) == "cone") {
		if (argc != 5) {
			cout << "Número de argumentos para o cone incorreto\n";
			exit(EXIT_FAILURE);
		}

		int bottomRadius = atoi(argv[2]), heightradius = atoi(argv[3]), slices = atoi(argv[4]), stacks = atoi(argv[5]);
		printCone(bottomRadius, heightradius, slices, stacks);
	} else {
		cout << "Figura geométrica não suportada\n";
		exit(EXIT_FAILURE);
	}

  	myfile.close();

	return 1;
}
