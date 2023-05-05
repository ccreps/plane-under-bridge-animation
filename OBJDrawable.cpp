#include "OBJDrawable.h"

GLuint OBJDrawable::getBufferArrayName()
{
	return bufferArrayName;
}

GLuint OBJDrawable::getProgramID()
{
	return programID;
}

OBJDrawable::OBJDrawable(string filename, GLuint progID, string positionAttribute, string normalAttribute)
{
	float* vertices;
	float* normals;
	programID = progID;
	vertices = readOBJFile(filename, nbrOfTriangles, normals);
	glGenVertexArrays(1, &vertexArrayName);
	glBindVertexArray(vertexArrayName);

	// Alternately...
	// GLuint   vaoID;
	// glGenVertexArrays(1, &vaoID);
	// glBindVertexArray(vaoID);
	//

	glGenBuffers(1, &bufferArrayName);
	glBindBuffer(GL_ARRAY_BUFFER, bufferArrayName);
	glBufferData(GL_ARRAY_BUFFER,
		nbrOfTriangles * 3 * 4 * sizeof(GLfloat) + nbrOfTriangles * 3 * 3 * sizeof(GLfloat),
		NULL, GL_STATIC_DRAW);
	//                               offset in bytes   size in bytes     ptr to data    
	glBufferSubData(GL_ARRAY_BUFFER, 0, nbrOfTriangles * 3 * 4 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nbrOfTriangles * 3 * 4 * sizeof(GLfloat), nbrOfTriangles * 3 * 3 * sizeof(GLfloat), normals);
	/*
	 * Set up variables into the shader programs (Note:  We need the
	 * shaders loaded and built into a program before we do this)
	 */
	GLuint vPosition = glGetAttribLocation(programID, positionAttribute.c_str());
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(programID, normalAttribute.c_str());
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(nbrOfTriangles * 3 * 4 * sizeof(GLfloat)));
}

void OBJDrawable::display() {
	GLint savedProgramID, savedBufferArray, savedVertexArray;
	glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgramID);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &savedBufferArray);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &savedVertexArray);
	glUseProgram(programID);
	glBindVertexArray(vertexArrayName);
	glBindBuffer(GL_ARRAY_BUFFER, bufferArrayName);

	glDrawArrays(GL_TRIANGLES, 0, nbrOfTriangles * 3);
	glUseProgram(savedProgramID);
	glBindVertexArray(savedVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, savedBufferArray);
}

void OBJDrawable::dispose() {

}