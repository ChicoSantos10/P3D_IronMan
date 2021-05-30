#version 440 core

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec2 vtexcoord;
layout(location = 2) in vec3 vnormal;

layout(location = 0) uniform mat4 MVP;

out vec2 texcoord;

void main()
{ 
	gl_Position = MVP * vec4(vpos, 1.0f);

	texcoord = vtexcoord;
}