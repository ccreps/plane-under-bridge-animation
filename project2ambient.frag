#version 330 core
// project 2 Fragment shader -- with only ambient lighting. 
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

        vec3 rgb;

	vec3 scatteredLight = ambientLight.rgb;
        rgb = min(objectColor.rgb * scatteredLight, vec3(1.0));

	FragColor = vec4(rgb, objectColor.a);
}
