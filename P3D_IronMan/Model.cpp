#include <vector>
#include <glm/gtc/matrix_transform.hpp> 
#include <fstream>
#include <iostream>
#include <string>
#include<sstream>

#include "Model.h"
#include <GL/glew.h>

Model::Model(const char* fileName) {

    /*std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;*/

    std::ifstream file(fileName);

    if (!file.is_open()) {
        printf("Impossible to open the file !\n"); //TODO:mudar
        //return nullptr;
    }

    std::stringstream stream;
    std::string line = ""; 
    std::string prefix = "";

    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;

    std::vector<glm::vec3> tempPositions;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;

    std::vector<GLint> vertex_position_indicies;
    std::vector<GLint> vertex_uv_indicies;
    std::vector<GLint> vertex_normal_indicies;

    char* materialPath;

    while (std::getline(file, line)) {

        // Clears the variable because of the loop
        stream.clear();
        // Copies the line content to the stream
        stream.str(line);
        // Copies the first value/word to prefix
        stream >> prefix;

        if (prefix == "mtllib") {
            //stream >> materialPath;
        }
        else if (prefix == "v") {
            stream >> position.x >> position.y >> position.z;
            tempPositions.push_back(position);
        }
        else if (prefix == "vt") {
            stream >> uv.x >> uv.y;
            tempUvs.push_back(uv);
        }
        else if (prefix == "vn") {
            stream >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (prefix == "f") {
            int i = 0;
            GLint index = 0;

            while (stream >> index)
            {
                // v/vt/vn
                if (i == 0)
                    vertex_position_indicies.push_back(index);
                else if (i == 1)
                    vertex_uv_indicies.push_back(index);
                else if (i == 2)
                    vertex_normal_indicies.push_back(index);

                // Ignores undesirable characters
                if (stream.peek() == '/')
                {
                    i++;
                    stream.ignore(1, '/');
                }
                else if (stream.peek() == ' ')
                {
                    i++;
                    stream.ignore(1, ' ');
                }

                if (i > 2)
                    i = 0; 
            }
        }
        for (int i = 0; i < tempPositions.size(); i++)
        {
            positions.push_back(tempPositions[vertex_position_indicies[i] - 1]);
            uvs.push_back(tempUvs[vertex_uv_indicies[i] - 1]);
            normals.push_back(tempNormals[vertex_normal_indicies[i] - 1]);
            std::cout << i + 1 << ":" << std::endl;
            std::cout << positions[i].x << positions[i].y << positions[i].z << std::endl;
            std::cout << uvs[i].x << uvs[i].y << std::endl;
            std::cout << normals[i].x << normals[i].y << normals[i].z << std::endl;
        }
    }
}