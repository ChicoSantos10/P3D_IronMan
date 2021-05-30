#version 440 core

in vec2 texcoord;

layout(location = 1) uniform sampler2D text;

out vec4 fColor; 

void main()
{
	vec4 color = texture(text, texcoord);
	fColor = color;
}
