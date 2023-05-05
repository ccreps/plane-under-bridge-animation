#version 330 core
//
// Project 2 Vertex shader.
// Passes vertex information through without changing it.
// Last Updated October 12, 2022
//
// Note -- the normalMatrix is now set up to be passed in as a
//         mat4.  This is being done to stay compatible with the
//         JOGL PMVMatrix implmentation.  This will mean the C++
//         normal code will no longer have to strip off the 4th
//         column and the 4th row. 
// 
uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;
uniform vec3 lightDirection;
uniform vec4 lightColor;
uniform vec4 objectColor;

in vec4 vPosition;
in vec3 vNormal;

out vec3 Normal;
out vec4 Position;

void main()
{
	Normal = mat3(normalMatrix) * vNormal;
	Position = viewingMatrix * modelingMatrix * vPosition;
        gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vPosition;
}
