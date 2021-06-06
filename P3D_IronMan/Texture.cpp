#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include <iostream>

GLuint Texture::count = 0;

Texture::Texture(std::string texture) {
    LoadTexture(texture);
}

bool Texture::LoadTexture(std::string texture) {
	
    glGenTextures(1, &name);

    glActiveTexture(GL_TEXTURE0 + count);
    glBindTexture(GL_TEXTURE_2D, name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned char* imageData = stbi_load(texture.c_str(), &width, &height, &channels, 0);
    if (imageData) {
        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGB,
            width, height,
            0,
            channels == 4 ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE,
            imageData);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(imageData);

        count++;
    }
    else {
        std::cout << "Error loading texture!" << std::endl;
        glDeleteTextures(1, &name);
        return false;
    }

    return true;
}

void Texture::SetFlipVertically(bool flip) {
    stbi_set_flip_vertically_on_load(true);
}