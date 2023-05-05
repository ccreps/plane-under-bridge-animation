#ifndef __OBJDRAWABLE_H__
#define __OBJDRAWABLE_H__
#include <string>
#include <glad.h>
#include <GLFW/glfw3.h>
using namespace std;

float* readOBJFile(string filename, int& nbrTriangles, float*& normalArray);
#define BUFFER_OFFSET(x)  ((const void*) (x))

class OBJDrawable
{
private:
protected:
	GLuint vertexArrayName, bufferArrayName, programID;
	string positionAttributeName, normalAttributeName;
	int  nbrOfTriangles;

public:
	OBJDrawable();
	OBJDrawable(string filename, GLuint progID, string positionAttribute="vPosition", string normalAttribute = "vNormal");
	void display();
	void dispose();
	GLuint getVertexArrayName();
	GLuint getBufferArrayName();
	GLuint getProgramID();
};


#endif

