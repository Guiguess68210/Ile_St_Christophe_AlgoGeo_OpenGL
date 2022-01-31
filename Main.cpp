// Inclure les librairies glad, GLM et GLFW3
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>
// Autres librairies et classes
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <random>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>
#include <linmath.h>
#include "enveloppe.h"
#include "point.h"
#include "carte.h"
#include <algorithm>
#include <stack>
#include <fstream>
#include <sstream>
#include <array>

static int tourner = 1;
static int deplacer = 1;
static float xx = 0.0f, anglex = 0.0f, angley = 0.0f, xold = 0.0f, yold = 0.0f;
static int presse = 0;

// Hauteur et largeur de la fenètre
const int WIDTH = 1600;
const int HEIGH = 1000;

// Paramètres d'intéraction des touches du clavier et de la souris
bool leftkeytable = false;
bool rightkeytable = false;
bool leftButtonMouse = false;

DemiCote* premierExterieur;

int refreshMills = 15;

vector<array<GLfloat, 6>> verticesTab;

static const struct
{
	float x, y, z;
	float r, g, b;
} vertices[] =
{
	 -0.6f, -0.4f, 0.0f, 1.f, 0.f, 0.f ,  //sommet 0
	  0.6f, -0.4f, 0.0f, 0.f, 1.f, 0.f ,  //sommet 1
	  0.f,  0.6f, 0.0f, 0.f, 0.f, 1.f ,   //sommet 2
	 0.f,  0.0f, 0.6f, 1.f, 0.f, 1.f      //sommet 3
};

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

/**
* Afficher un message d'erreur
*/
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

// Fonction définissant les dimensions de la fenêtre lorsque l'utilisateur ou l'OS la redimensionne
void framebuffer_size_callback(GLFWwindow* window, int width = WIDTH, int heigh = HEIGH)
{
	glViewport(0, 0, 2500, 2500);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		switch (key) {
		case  GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
		case  GLFW_KEY_L: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case  GLFW_KEY_F: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case  GLFW_KEY_P: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		case  GLFW_KEY_X: anglex += 0.1; break;
		case  GLFW_KEY_Y: angley += 0.1;
		}
}

/**
* Intéragir avec la position du curseur
*/
static void cursor_position_callback(GLFWwindow* window, double xpos, double  ypos)
{
	if (presse)
	{
		angley = -(xpos - xold) / 600.0;
		anglex = (ypos - yold) / 500.0;
	}
	else
	{
		xold = xpos;
		yold = ypos;
	}
}

/**
* Intéragir avec le click de la souris
*/
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		presse = 0;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		presse = 1;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		anglex = 0.0f;
		angley = 0.0f;
	}
}

/**
* Associer 2 demis côtés pour former un côté
*/
static void segment(DemiCote* demiCoteA, DemiCote* demiCoteB) {
	float demiCoteAx, demiCoteAy, demiCoteAz, demiCoteAr, demiCoteAg, demiCoteAb,
		demiCoteBx, demiCoteBy, demiCoteBz, demiCoteBr, demiCoteBg, demiCoteBb;

	demiCoteAx = (((float(demiCoteA->coordonnees().x()))) / 1000) - 1;
	demiCoteAy = (((float(demiCoteA->coordonnees().y()))) / 1000) - 1;
	demiCoteAz = (((float(demiCoteA->coordonnees().z()))) / 1000) - 1;
	if (demiCoteA->coordonnees().z() <= 0)
	{
		demiCoteAr = 0.0;
		demiCoteAg = 1.0;
		demiCoteAb = 1.0;
	}
	else if ((demiCoteA->coordonnees().z() < (30 / 3))) {
		demiCoteAr = 0.1;
		demiCoteAg = 1.0;
		demiCoteAb = 0.1;
	}
	else {
		demiCoteAr = 0.5;
		demiCoteAg = 0.35;
		demiCoteAb = 0.05;
	}

	demiCoteBx = (((float(demiCoteB->coordonnees().x()))) / 1000) - 1;
	demiCoteBy = (((float(demiCoteB->coordonnees().y()))) / 1000) - 1;
	demiCoteBz = (((float(demiCoteB->coordonnees().z()))) / 1000) - 1;
	if (demiCoteA->coordonnees().z() <= 0)
	{
		demiCoteBr = 0.0;
		demiCoteBg = 1.0;
		demiCoteBb = 1.0;
	}
	else if ((demiCoteB->coordonnees().z() < (30 / 3))) {
		demiCoteBr = 0.1;
		demiCoteBg = 1.0;
		demiCoteBb = 0.1;
	}
	else {
		demiCoteBr = 0.5;
		demiCoteBg = 0.35;
		demiCoteBb = 0.05;
	}

	Point newPointA = Point(demiCoteAx, demiCoteAy, demiCoteAz, demiCoteAr, demiCoteAg, demiCoteAb);
	Point newPointB = Point(demiCoteBx, demiCoteBy, demiCoteBz, demiCoteBr, demiCoteBg, demiCoteBb);

	verticesTab.push_back(newPointA.getVertice());
	verticesTab.push_back(newPointB.getVertice());
}

/**
* Création du tableau de vertices pour tracer l'île
*/
void trace(Carte* carte)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < carte->nbDemiCote(); i++)
	{
		segment(carte->demiCote(i), carte->demiCote(i)->oppose());
	}

	//angleCube -= 0.30f;  // Swap the front and back frame buffers (double buffering)
}

/**
* Algorithme incrémental de la triangulation
*/
void triangulation(const vector<Point>& T, Carte& C)
{
	// Construction d'un premier triangle
	DemiCote* dc1;
	DemiCote* dc3;
	DemiCote* dc5;

	// Si point à gauche alors ABC
	if (T[2].aGauche(T[0], T[1]) < 0)
	{
		dc1 = C.ajouteCote(T[0], T[1]);
		dc3 = C.ajouteCote(T[2], dc1->oppose());
		dc5 = C.ajouteCote(dc3, dc1);
	}
	// Sinon ACB
	else {
		dc1 = C.ajouteCote(T[0], T[2]);
		dc3 = C.ajouteCote(T[1], dc1->oppose());
		dc5 = C.ajouteCote(dc3, dc1);
	}

	DemiCote* haut = dc5;
	DemiCote* bas = dc5->precedent();

	for (int i = 3; i < T.size(); i++)
	{
		int compteur = 0;
		// Parcours du sommet d'abscicce maximal sens trigonométrique
		while (T[i].aGauche(haut->coordonnees(), haut->oppose()->coordonnees()) > 0)
		{
			haut = haut->oppose()->suivant();
			compteur++;
		}
		haut = haut->precedent();
		// Parcours du sommet d'abscicce maximal sens horaire
		while (T[i].aGauche(bas->oppose()->coordonnees(), bas->coordonnees()) > 0 || compteur == 0)
		{
			if (T[i].aGauche(bas->oppose()->coordonnees(), bas->coordonnees()) > 0)
			{
				compteur++;
			}
			bas = bas->oppose()->precedent();
		}
		DemiCote* nouveau = C.ajouteCote(T[i], bas);
		DemiCote* nouveau2 = C.ajouteCote(nouveau, nouveau->oppose()->suivant()->oppose());
		for (int boucle = 1; boucle < compteur; boucle++)
		{
			nouveau2 = C.ajouteCote(nouveau, nouveau2->oppose()->suivant()->oppose());
		}

		bas = nouveau;
		haut = nouveau2;
	}
	premierExterieur = C.demiCote(C.nbDemiCote() - 1);
}

/**
* Triangulation de Delaunay
*/
void delaunay(Carte& C)
{
	int compteur = 0;
	std::stack<DemiCote*> pile;
	DemiCote* parcour = premierExterieur;
	while (parcour != premierExterieur || compteur == 0)
	{
		parcour->changeMarque(2);
		parcour->oppose()->changeMarque(2);
		parcour = parcour->suivant()->oppose();
		compteur++;
	}

	for (int i = 0; i < C.nbDemiCote(); i++)
	{
		if (C.demiCote(i)->marque() == 0)
		{
			pile.push(C.demiCote(i));
		}
		i++;
	}
	std::vector<DemiCote*> pileSecondaire;
	while (pile.size() != 0)
	{
		DemiCote* dc1 = pile.top();
		DemiCote* dc1Oppose = dc1->oppose();
		Point a_1 = dc1->coordonnees();
		Point b_2 = dc1Oppose->coordonnees();
		Point c_3 = dc1->precedent()->oppose()->coordonnees();
		Point d_4 = dc1->suivant()->oppose()->coordonnees();

		std::vector<Point> pointsATrie = { a_1, b_2, c_3 };
		sort(pointsATrie, pointsATrie.size());

		int det = d_4.dansCercle(a_1, b_2, c_3);

		if (det <= 0)
		{
			pile.top()->changeMarque(1);
			pile.top()->oppose()->changeMarque(1);
			pile.pop();
		}
		else if (det > 0)
		{
			pile.pop();
			C.flip(dc1);

			DemiCote* dcQuad1 = dc1->suivant();
			DemiCote* dcQuad2 = dc1->precedent();
			DemiCote* dcQuad3 = dc1Oppose->suivant();
			DemiCote* dcQuad4 = dc1Oppose->precedent();

			if (dcQuad1->marque() == 1)
			{
				dcQuad1->changeMarque(0);
				dcQuad1->oppose()->changeMarque(0);
				pile.push(dcQuad1);
			}
			if (dcQuad2->marque() == 1)
			{
				dcQuad2->changeMarque(0);
				dcQuad2->oppose()->changeMarque(0);
				pile.push(dcQuad2);
			}
			if (dcQuad3->marque() == 1)
			{
				dcQuad3->changeMarque(0);
				dcQuad3->oppose()->changeMarque(0);
				pile.push(dcQuad3);
			}
			if (dcQuad4->marque() == 1)
			{
				dcQuad4->changeMarque(0);
				dcQuad4->oppose()->changeMarque(0);
				pile.push(dcQuad4);
			}
		}
	}
}

/**
* Géneration des points de la carte à partir du fichier points.csv
*/
static void mapGeneration() {
	string fname = "points.csv";
	std::vector<Point> listeDePoints;
	listeDePoints.clear();
	Carte* carte = new Carte();
	vector<string> row;
	string line, word;

	fstream file(fname, ios::in);
	if (file.is_open())
	{
		while (getline(file, line))
		{
			row.clear();

			stringstream str(line);
			int compteur = 1;
			int x;
			int y;
			int z;
			while (getline(str, word, ';'))
			{
				if (compteur == 1)
				{
					x = std::stoi(word);
				}
				else if (compteur == 2)
				{
					y = std::stoi(word);
				}
				else {
					z = std::stoi(word);
				}
				compteur++;
			}
			listeDePoints.push_back(Point(x / 30, y / 30, z / 30));
		}
	}
	sort(listeDePoints, listeDePoints.size());
	triangulation(listeDePoints, *carte);
	delaunay(*carte);
	trace(carte);
}

int main(void)
{
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;
	float angle = 0.0f;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(WIDTH, HEIGH, "Ile St Christophe", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	// Redimensionnement de la fenêtre
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	mapGeneration();

	// NOTE: OpenGL error checks have been omitted for brevity

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, verticesTab.size() * sizeof(float) * 6, verticesTab.data(), GL_STATIC_DRAW);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 6, (void*)(sizeof(float) * 3));

	int success;
	char infoLog[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s \n", infoLog);
	}

	// check for shader compile errors
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s \n", infoLog);
	}

	// check for linking errors
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s \n", infoLog);
	}

	// glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width = WIDTH, heigh = HEIGH;
		mat4x4 m, p, mvp, t;

		glfwGetFramebufferSize(window, &width, &heigh);
		ratio = WIDTH / (float)HEIGH;

		// Couleur d'arrière plan
		glViewport(0, 0, WIDTH, HEIGH);
		glClear(GL_COLOR_BUFFER_BIT);
		glLineWidth(2);

		mat4x4_identity(m);
		mat4x4_rotate(m, m, 0.0f, 1.f, 0.0f, (float)glfwGetTime());
		mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 2.f, -2.f);
		mat4x4_mul(mvp, p, m);

		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
		glDrawArrays(GL_LINES, 0, 1000000);
		//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}