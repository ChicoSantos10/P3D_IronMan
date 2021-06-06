#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "FileReader.h"

bool FileReader::OBJReader(const char* fileName, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::string &mtl) {
    
    std::ifstream file(fileName);

    if (!file.is_open()) {
        FileReader::Error_OpenFile(fileName);
        return false;
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

    while (std::getline(file, line)) {

        // Clears the variable because of the loop
        stream.clear();
        // Copies the line content to the stream
        stream.str(line);
        // Copies the first value/word to prefix
        stream >> prefix;

        if (prefix == "mtllib") {
            stream >> mtl;
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
                    stream.ignore(1, '/');

                i++;

                // Cycle i between 0 and 2
                i %= 3;
            }
        }
    }

    for (int i = 0; i < vertex_position_indicies.size(); i++)
    {
        positions.push_back(tempPositions[vertex_position_indicies[i] - 1]);
        uvs.push_back(tempUvs[vertex_uv_indicies[i] - 1]);
        normals.push_back(tempNormals[vertex_normal_indicies[i] - 1]);
    }

    return true;
}

bool FileReader::MTLReader(const char* fileName, float& ns, glm::vec3& ka, glm::vec3& kd, glm::vec3& ks, std::string &texture) {
    
    std::ifstream fileMtl(fileName);
    std::stringstream stream;
    std::string line = "";
    std::string prefix = "";

    if (!fileMtl.is_open()) {
        Error_OpenFile(fileName);
        return false;
    }

    while (std::getline(fileMtl, line))
    {
        // Clears the variable because of the loop
        stream.clear();
        // Copies the line content to the stream
        stream.str(line);
        // Copies the first value/word to prefix
        stream >> prefix;

        if (prefix == "Ns")
        {
            stream >> ns;
        }
        else if (prefix == "Ka")
        {
            stream >> ka.x >> ka.y >> ka.z;
        }
        else if (prefix == "Kd")
        {
            stream >> kd.x >> kd.y >> kd.z;
        }
        else if (prefix == "Ks")
        {
            stream >> ks.x >> ks.y >> ks.z;
        }
        else if (prefix == "map_Kd")
        {
            stream >> texture;
        }
    }

    return true;
}

void FileReader::Error_OpenFile(const char* fileName) {
    std::cout << "Can't open the file: " << fileName << std::endl;
}
