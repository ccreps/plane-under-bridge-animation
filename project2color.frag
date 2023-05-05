#version 330 core
// project 2 Fragment shader -- with only objectColor used. 
// Last update October 12, 2022
//

uniform vec4  objectColor;
uniform vec3  lightDirection;
uniform vec4  lightColor;
uniform float shininess;
uniform vec3  halfVector;
uniform vec4  ambientLight;

in vec3 Normal;
in vec4 Position;

out vec4 FragColor;

void main()
{
	FragColor = vec4(objectColor);
}
