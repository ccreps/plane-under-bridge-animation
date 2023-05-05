#version 330 core
// project 2 Fragment shader -- with directional lighting.
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
	float diffuse = max(0.0, dot(Normal, lightDirection));
	float specular = max (0.0, dot(Normal, halfVector));
        vec3 rgb;

	if (diffuse == 0.0)  {
		specular = 0.0;
	} else {
		specular = pow(specular, shininess);
	}

	vec3 scatteredLight = ambientLight.rgb + lightColor.rgb * diffuse;
        vec3 reflectedLight = lightColor.rgb * specular;
        rgb = min(objectColor.rgb * scatteredLight + reflectedLight, vec3(1.0));

	FragColor = vec4(rgb, objectColor.a);
}
