#version 440 core

in vec2 texcoord;
in vec3 positionView;
in vec3 fragPos;
in vec3 normalView;

uniform mat4 model;
uniform mat4 view;
uniform mat4 viewModel;
uniform sampler2D image;

struct Material {
	vec3 emissive;
	vec3 ambient;		
	vec3 diffuse;		
	vec3 specular;		
	float shininess;
};

uniform Material material;

struct AmbientLight {
	vec3 ambient;
	bool isActive;
};

uniform AmbientLight ambientLight;

struct DirectionalLight {
	vec3 direction;		
	vec3 ambient;		
	vec3 diffuse;		
	vec3 specular;	
	bool isActive;	
};

uniform DirectionalLight directionalLight;

struct PointLight {
	vec3 position;		
	vec3 ambient;		
	vec3 diffuse;		
	vec3 specular;		
	float constant;		
	float linear;		
	float quadratic;	
	bool isActive;
};

uniform PointLight pointLight;

struct SpotLight {
	vec3 position;
	vec3 ambient;		
	vec3 diffuse;		
	vec3 specular;		
	float constant;		
	float linear;		
	float quadratic;	
	float cutoff;
	float exponent;
	vec3 direction;
	bool isActive;
};

uniform SpotLight spotLight;

out vec4 fColor; 

vec4 computeAmbientLight(AmbientLight light);
vec4 computeDirectionalLight(DirectionalLight light);
vec4 computePointLight(PointLight light);
vec4 computeSpotLight(SpotLight light);
vec4 computeSpotLightDiff(SpotLight light);

void main()
{
	vec4 emissive = vec4(material.emissive, 1.0);

	vec4 ambient = ambientLight.isActive ? computeAmbientLight(ambientLight) : vec4(0);
	vec4 directional = directionalLight.isActive ? computeDirectionalLight(directionalLight) : vec4(0);
	vec4 point = pointLight.isActive ? computePointLight(pointLight) : vec4(0);
	vec4 spot = spotLight.isActive ? computeSpotLight(spotLight) : vec4(0); 
	//vec4 spot = computeSpotLightDiff(spotLight);

	vec4 color = texture(image, texcoord);
	fColor = (emissive + ambient + directional + point + spot) * color;
	//fColor = directional;
}

vec4 computeAmbientLight(AmbientLight light) {
	return vec4(material.ambient * light.ambient, 1.0); 
}

vec4 computeDirectionalLight(DirectionalLight light) {
	
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	vec3 lightDirectionView = (view * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionView); 
	vec3 N = normalize(normalView);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;
	
	vec3 V = normalize(-positionView);
	
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

	return ambient + diffuse + specular;
}

vec4 computePointLight(PointLight light) {
	
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0) * 0.1;

	vec3 lightPositionView = (view * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionView - positionView);
	vec3 N = normalize(normalView);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

	vec3 V = normalize(-positionView);

	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	float dist = length(mat3(view) * light.position - positionView);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	return (attenuation * (ambient + diffuse + specular));
}

vec4 computeSpotLight(SpotLight light) {

	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	vec3 lightPositionView = (view * vec4(light.position, 1.0)).xyz;
	vec3 lightDirectionView = (view * vec4(light.direction, 0.0)).xyz;
	vec3 D = normalize(lightPositionView - positionView);
	vec3 N = normalize(normalView);
	vec3 L = normalize(lightDirectionView);
	float NdotL = max(dot(N, D), 0.0);
	float DdotL = max(dot(-D, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

	vec3 V = normalize(-positionView);

	vec3 R = reflect(-D, N);
	float RdotV = max(dot(R, V), 0.0);
	
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	float dist = length(mat3(view) * light.position - positionView);
	float attenuation = 0;
	if (DdotL > light.cutoff) {
		attenuation = pow(DdotL, light.exponent) / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	}
	
	return (attenuation * (ambient + diffuse + specular));
}
