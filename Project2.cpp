#define _USE_MATH_DEFINES
#include <cmath>
#include <glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include "LoadShaders.h"
#include "OBJDrawable.h"
#include "linmath.h"
#include <map>
#include <vector>
#include <math.h>
using namespace std;
/*
 * Computer Graphics I -- Project 2 -- Base Code.
 * Name:  Collin Creps
*/

#define BUFFER_OFFSET(x)  ((const void*) (x))

GLuint programID;
/*
* Arrays to store the indices/names of the Vertex Array Objects and
* Buffers.  Rather than using the books enum approach I've just
* gone out and made a bunch of them and will use them as needed.
*
* This will change as we add things into our toolbox this term.
*/

GLuint vertexBuffers[10], arrayBuffers[10], elementBuffers[10];
/*
* Global variables
*   The location for the transformation and the current rotation
*   angle are set up as globals since multiple methods need to
*   access them.
*/
float rotationAngle;
int nbrTriangles[10];

//initialize our matrixes
mat4x4 rotationMatrix;
mat4x4 projectionMatrix;
mat4x4 viewingMatrix;
mat4x4 modelingMatrix;

map<string, GLuint> locationMap;

//holds the bridge and plane as drawable objects
vector<OBJDrawable*> items;
//holds the calculated lengths from the excel
vector<GLfloat> lengths = { 0, 0.946977433, 1.725831899, 2.340870953, 2.945881995, 3.620610457, 4.284400538, 4.879311715, 5.516923436, 6.335720106, 7.300905591, 8.247883024, 9.02673749,
	9.641776544, 10.24678759, 10.92151605, 11.58530613, 12.18021731, 12.81782903, 13.6366257, 14.60181118};
//hold the step length we are currently on and want to calculate x for
float targetValue = 0.0;
//constant which makes the product 30fps. targetValue is incremented by this
const float stepLength = 14.60181 / 300.0;
//current time which gets incremented by .001 at end of display loop (basically controls speed of animation)
float t = 0.0f;

// Prototypes
GLuint buildProgram(string vertexShaderName, string fragmentShaderName);
GLFWwindow* glfwStartUp(int& argCount, char* argValues[],
	string windowTitle = "Autobots Roll Out!", int width = 500, int height = 500);
void setAttributes(float lineWidth = 1.0, GLenum face = GL_FRONT_AND_BACK,
	GLenum fill = GL_FILL);
void buildObjects();
void getLocations();
void init(string vertexShader, string fragmentShader);

float* readOBJFile(string filename, int& nbrTriangles, float*& normalArray);
/*
 * Error callback routine for glfw -- uses cstdio
 */
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

/*
 * Routine to encapsulate some of the startup routines for GLFW.  It returns the window ID of the
 * single window that is created.
 */
GLFWwindow* glfwStartUp(int& argCount, char* argValues[], string title, int width, int height) {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);   // This is set to compliance for 4.1 -- if your system
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);   // supports 4.5 or 4.6 you may wish to modify it. 

	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSwapInterval(1);

	return window;
}


/*
 * Use the author's routines to build the program and return the program ID.
 */
GLuint buildProgram(string vertexShaderName, string fragmentShaderName) {

	/*
	*  Use the Books code to load in the shaders.
	*/
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, vertexShaderName.c_str() },
		{ GL_FRAGMENT_SHADER, fragmentShaderName.c_str() },
		{ GL_NONE, NULL }
	};
	GLuint program = LoadShaders(shaders);
	if (program == 0) {
		cerr << "GLSL Program didn't load.  Error \n" << endl
			<< "Vertex Shader = " << vertexShaderName << endl
			<< "Fragment Shader = " << fragmentShaderName << endl;
	}
	glUseProgram(program);
	return program;
}

/*
 * Set up the clear color, lineWidth, and the fill type for the display.
 */
void setAttributes(float lineWidth, GLenum face, GLenum fill) {
	/*
	* I'm using wide lines so that they are easier to see on the screen.
	* In addition, this version fills in the polygons rather than leaving it
	* as lines.
	*/
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(lineWidth);
	glPolygonMode(face, fill);
	glEnable(GL_DEPTH_TEST);

}

/*
 * read and/or build the objects to be displayed.  Also sets up attributes that are
 * vertex related.
 */
void buildObjects() {

	OBJDrawable* bridgeObject = new OBJDrawable("GoldenGateTriangulatedRotated.obj", programID);
	OBJDrawable* planeObject = new OBJDrawable("triangulatedAirplane.obj", programID);
	items.push_back(bridgeObject);
	items.push_back(planeObject);
}

void setupLightingUniforms() {

	//
	GLfloat lightDirection[] = { 0.0f, 0.7071f, 0.7071f };
	GLfloat lightColor[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat shininess = 50.0f;
	GLfloat halfVector[] = { 0.0f, 0.3827f, 0.9239f };

	
	//pass down the arrays provided
	GLuint lightDirectionLocation = glGetUniformLocation(programID, "lightDirection");
	glUniform3f(lightDirectionLocation, lightDirection[0], lightDirection[1], lightDirection[2]);
	GLuint lightColorLocation = glGetUniformLocation(programID, "lightColor");
	glUniform4f(lightColorLocation, lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
	GLuint ambientLightLocation = glGetUniformLocation(programID, "ambientLight");
	glUniform4f(ambientLightLocation, ambientLight[0], ambientLight[1], ambientLight[2], ambientLight[3]);
	GLuint halfVectorLocation = glGetUniformLocation(programID, "halfVector");
	glUniform3f(halfVectorLocation, halfVector[0], halfVector[1], halfVector[2]);
	GLuint shininessLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(shininessLocation, shininess);
	
}


void getLocations() {
	/*
	 * Find out how many uniforms there are and go out there and get them from the
	 * shader program.  The locations for each uniform are stored in a global -- locationMap --
	 * for later retrieval.
	 */
	GLint numberBlocks;
	char uniformName[1024];
	int nameLength;
	GLint size;
	GLenum type;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numberBlocks);
	for (int blockIndex = 0; blockIndex < numberBlocks; blockIndex++) {
		glGetActiveUniform(programID, blockIndex, 1024, &nameLength, &size, &type, uniformName);
		cout << uniformName << endl;
		locationMap[string(uniformName)] = blockIndex;
	}
}

void init(string vertexShader, string fragmentShader) {

	setAttributes(1.0f, GL_FRONT_AND_BACK, GL_FILL);
	programID = buildProgram(vertexShader, fragmentShader);
	mat4x4_identity(rotationMatrix);
	mat4x4_identity(projectionMatrix);
	mat4x4_identity(viewingMatrix);
	buildObjects();
	getLocations();
}

void setupObjectColor(float red, float green, float blue, float alpha) {
	GLuint objectColorLocation = glGetUniformLocation(programID, "objectColor");
	glUniform4f(objectColorLocation, red, green, blue, alpha);
}

float xPosition(float t) {
	return (float)cos(t * 2.0 * M_PI);
}
float yPosition(float t) {
	return (float)(2.5 * cos(t * 2 * M_PI + (M_PI / 2)) + 2.7);
}
float zPosition(float t) {
	return (float)sin(2 * t * 2.0 * M_PI);
}

float lerp() {
	//goal here is to take a value fed from incrementing counter with lengthSteps and find where in the length array that lies.
	//Then we use the t values and and provided equation to figure out x.

	//setup vars for the equation
	float lowerYBound = 0.0;
	float upperYBound = 0.0;
	float lowerXBound = 0.0;
	float upperXBound = 0.0;

	//loop through lengths array to find the two index that are surrounding our target interpolation value
	for (int i = 0; i < lengths.size(); i++) {
		for (int j = 1; j < lengths.size(); j++) {
			if (lengths.at(i) <= targetValue && lengths.at(j) >= targetValue) {
				lowerXBound = t;
				upperXBound = t + 0.001;
				lowerYBound = lengths.at(i);
				upperYBound = lengths.at(j);
			}
		}
	}
	//return the value passed to parametric equations
	return (((targetValue - lowerYBound) * (upperXBound - lowerXBound)) / (upperYBound - lowerYBound)) + lowerXBound;
}


void updateNormalMatrix(mat4x4 viewingMatrix, mat4x4 modelingMatrix, const GLchar* normalMatrixUniformName) {
	
	//Update the normal matrix allows for lighting to be displayed properly.
	
	mat4x4 vpMatrix;
	mat4x4_identity(vpMatrix);
	mat4x4_mul(vpMatrix, vpMatrix, viewingMatrix);
	mat4x4_mul(vpMatrix, vpMatrix, modelingMatrix);
	int normalMatrixLocation = glGetUniformLocation(programID, normalMatrixUniformName);

	for (int i = 0; i < 3; i++) {
		vpMatrix[i][3] = 0;
		vpMatrix[3][i] = 0;
	}

	vpMatrix[3][3] = 1;
	mat4x4 inverted;
	mat4x4_invert(inverted, vpMatrix);
	mat4x4 normalMatrix;
	mat4x4_transpose(normalMatrix, inverted);
	glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, (const GLfloat*)normalMatrix);
}

/*
	Sets up all the matricies to properly form the scene. Also calls above functions to manipulate and allow for the lighting, animation, etc. to show up properly.
*/
void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// needed -- clears screen before drawing.
	glUseProgram(programID);
	
	setupLightingUniforms();

	//setup the perspective projection matrix
	GLuint projectionMatrixLocation = glGetUniformLocation(programID, "projectionMatrix");
	mat4x4_perspective(projectionMatrix, 1.05f, 1.0f, 0.01f, 100.0f);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, (const GLfloat*)projectionMatrix);

	//setup the viewing matrix
	vec3 eye = { 0.0f, 0.0f, 10.0f };
	vec3 center = { 0.0f, 0.0f, 0.0f };
	vec3 up = { 0.0f, 1.0f, 0.0f };
	GLuint viewingMatrixLocation = glGetUniformLocation(programID, "viewingMatrix");
	mat4x4_look_at(viewingMatrix, eye, center, up);
	glUniformMatrix4fv(viewingMatrixLocation, 1, GL_FALSE, (const GLfloat*)viewingMatrix);

	//set up modeling matrix
	GLuint modelingMatrixLocation = glGetUniformLocation(programID, "modelingMatrix");
	mat4x4_identity(modelingMatrix);
	mat4x4_identity(rotationMatrix);
	mat4x4_mul(modelingMatrix, modelingMatrix, rotationMatrix);
	glUniformMatrix4fv(modelingMatrixLocation, 1, GL_FALSE, (const GLfloat*)modelingMatrix);

	//setup to draw bridge
	setupObjectColor(0.5f, 0.5f, 0.5f, 1.0f);
	items[0]->display();

	glUniformMatrix4fv(modelingMatrixLocation, 1, GL_FALSE, (const GLfloat*)modelingMatrix);

	//translate the plane according to x,y,z as a function of current time
	mat4x4 translationMatrix;
	mat4x4_identity(translationMatrix);
	mat4x4_translate(translationMatrix, xPosition(lerp()), yPosition(lerp()), zPosition(lerp()));
	mat4x4_mul(modelingMatrix, modelingMatrix, translationMatrix);
	glUniformMatrix4fv(modelingMatrixLocation, 1, GL_FALSE, (const GLfloat*)modelingMatrix);
	updateNormalMatrix(viewingMatrix, modelingMatrix, "normalMatrix");

	//setup to draw plane
	setupObjectColor(0.5f, 0.5f, 0.7f, 1.0f);
	items[1]->display();

	glUniformMatrix4fv(modelingMatrixLocation, 1, GL_FALSE, (const GLfloat*)modelingMatrix);

	//reset the timer or increment based on where we are in looping
	t += 0.001f;
	if (t >= 1.0f) {
		t = 0.0;
	}
	targetValue += stepLength;
	if (targetValue >= 14.60181) {
		targetValue = 0;
	}

	//The time step is what index is in slides
	//We want to take the length / 300 to divide the path into equal parts
	//use equation given to solve for x and pass that to the equaitons 

}

/*
* Handle window resizes -- adjust size of the viewport -- more on this later
*/

void reshapeWindow(GLFWwindow* window, int width, int height)
{
	float ratio;
	ratio = width / (float)height;

	glViewport(0, 0, width, height);

}
/*
* Main program with calls for many of the helper routines.
*/
int main(int argCount, char* argValues[]) {
	GLFWwindow* window = nullptr;
	window = glfwStartUp(argCount, argValues, "Project 1 Collin Creps - Interpolation");
	init("project2.vert", "project2.frag");
	glfwSetWindowSizeCallback(window, reshapeWindow);

	while (!glfwWindowShouldClose(window))
	{
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	};

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
