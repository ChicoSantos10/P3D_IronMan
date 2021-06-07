#version 440 core

layout(early_fragment_tests) in;
layout (location = 0) in vec3 textureCoords;

uniform samplerCube skybox;

layout (location = 0) out vec4 fColor; // Cor final do fragmento

void main()
{
	fColor = texture(skybox, textureCoords);
}