#pragma once
#include <string>
#include <gl/glew.h>
#include <vector>

class Texture {
public: 
	Texture(std::string texture);
	Texture(std::vector<std::string> textures);
	//~Texture();
	static void SetFlipVertically(bool flip);
	GLuint id;
	static GLuint count;
	
private:
	bool LoadTexture(std::string texture);
	bool LoadTexture(std::vector<std::string> textures);
	GLuint name_;
	int width_, height_, channels_;
};
