#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include <iostream>

GLuint Texture::count = 0;

// Single texture
Texture::Texture(std::string texture) {
    LoadTexture(texture);
}

// Cube map
Texture::Texture(std::vector<std::string> textures) {
    LoadTexture(textures);
}

// Single texture
bool Texture::LoadTexture(std::string texture) {
	
    glGenTextures(1, &name_);

    glActiveTexture(GL_TEXTURE0 + count);
    glBindTexture(GL_TEXTURE_2D, name_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned char* imageData = stbi_load(texture.c_str(), &width_, &height_, &channels_, 0);
    if (imageData) {
        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGB,
            width_, height_,
            0,
            channels_ == 4 ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE,
            imageData);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(imageData);

        count++;
    }
    else {
        std::cout << "Error loading texture!" << std::endl;
        return false;
    }

    return true;
}

// Cube map
bool Texture::LoadTexture(std::vector<std::string> textures) {

    glGenTextures(1, &name_);

    id = count;

    glActiveTexture(GL_TEXTURE0 + id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, name_);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLint face = 0;

    for (auto& file : textures) {
        unsigned char* imageData = stbi_load(file.c_str(), &width_, &height_, &channels_, 0);
        if (imageData) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                0,
                GL_RGB,
                width_, height_,
                0,
                channels_ == 4 ? GL_RGBA : GL_RGB,
                GL_UNSIGNED_BYTE,
                imageData);

            face++;

            stbi_image_free(imageData);
        }
        else {
            std::cout << "Error loading texture!" << std::endl;
            return false;
        }
    }

    count++;
}

void Texture::SetFlipVertically(bool flip) {
    stbi_set_flip_vertically_on_load(flip);
}