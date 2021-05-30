#version 440 core

in vec3 vpos;
in vec2 texcoord;
in vec3 normal;

layout(location = 0) uniform mat4 MVP;

void main()
{ 
	gl_Position = MVP * vec4(vpos, 1.0f);
}