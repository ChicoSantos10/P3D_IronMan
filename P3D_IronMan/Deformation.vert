#version 440 core

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec2 vtexcoord;
layout(location = 2) in vec3 vnormal;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 viewModel;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform float time;
uniform sampler2D image;
uniform float deformation = 0;

out vec2 texcoord;
out vec3 positionView;
out vec3 fragPos;
out vec3 normalView;

void main()
{ 
	float size = length(vpos);
    float amplitude = 0.15;
    float speed = 1000;
    float interval = 3.14 * 10;
    // Multiplying size by time makes the wave increase/decrease in size with time
    float y = amplitude * sin(radians(size * time * interval + time * speed)) * deformation;
    
    // Position in camera view
    vec4 position = viewModel * vec4(vpos, 1.0f);
    // Apply the distortion to camera view
    position.x += y;
    position.y += y;
	
    texcoord = vtexcoord;   
    positionView = (viewModel * vec4(vpos, 1.0)).xyz;
    fragPos = (model * vec4(vpos, 1.0)).xyz;
    normalView = normalize(normalMatrix * vnormal);
    //normalView = vnormal;

	gl_Position = projection * position;
}