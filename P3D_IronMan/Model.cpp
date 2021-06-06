#include <vector>
#include <glm/gtc/matrix_transform.hpp> 
#include <fstream>
#include <iostream>
#include <string>
#include<sstream>

#include "Model.h"
#include "FileReader.h"
#include <GL/glew.h>

Model::Model(const char* fileName) {

    std::string mtl;
    if (FileReader::OBJReader(fileName, positions, uvs, normals, mtl)) {
        
        float ns;
        glm::vec3 ka, kd, ks;
        std::string texturePath;

        if (FileReader::MTLReader(mtl.c_str(), ns, ka, kd, ks, texturePath)) {
            texture = new Texture(texturePath);
            material = new Material(ns, ka, kd, ks);
        }
    }
}

Model::~Model() {
    delete material;
}