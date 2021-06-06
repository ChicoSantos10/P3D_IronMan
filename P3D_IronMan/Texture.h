#pragma once
#include <string>
#include <gl/glew.h>

class Texture {
public: 
	Texture(std::string texture);
	~Texture();
	static void SetFlipVertically(bool flip);
	
private:
	GLuint name;
	int width, height, channels;
	bool LoadTexture(std::string texture);
	static GLuint count;
};
