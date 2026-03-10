#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
using namespace glm;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;

    vec3 bitangent;
    vec3 tangent;

    Vertex(const vec3 &position, const vec3 &normal, const vec2 &uv, const vec3 &bitangent = vec3(0.0f), const vec3 &tangent = vec3(0.0f)) {
        this->position = position;
        this->normal = normal;
        this->uv = uv;

        this->bitangent = bitangent;
        this->tangent = tangent;
    }
};

int readOBJ(const std::string &filepath, GLuint &VAO, GLuint &VBO, GLuint &EBO) {
    std::ifstream file(filepath);
    if(!file.is_open()) {
        return 0; // TODO make this return a default object
    }

    // std::vector<unsigned int> positionIndices, texIndices, normalIndices;
    std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> rawIndices;
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<vec2> texs;

    std::string line;
    while(std::getline(file, line)) {
        // std::cout << "reading: " << line << std::endl;
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if(type == "v") {
            float x, y, z;
            iss >> x >> y >> z;

            positions.push_back(vec3(x, y, z));
        } else if(type == "vn") {
            float x, y, z;
            iss >> x >> y >> z;

            normals.push_back(vec3(x, y, z));
        } else if(type == "vt") {
            float x, y;
            iss >> x >> y;

            texs.push_back(vec2(x, y));
        }else if(type == "f") {
            std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> faceIndices;
            std::string vertexData;

            while(iss >> vertexData) {
                std::istringstream viss(vertexData);
                std::string indexStr;

                unsigned int posIndex, texIndex = 0, norIndex = 0;
                
                std::getline(viss, indexStr, '/');
                if(!indexStr.empty()) {
                    posIndex = std::stoi(indexStr) - 1;
                } else{
                    break; // Each vertex needs a position at least
                }

                std::getline(viss, indexStr, '/');
                if(!indexStr.empty()) {
                    texIndex = std::stoi(indexStr) - 1;
                }

                std::getline(viss, indexStr, '/');
                if(!indexStr.empty()) {
                    norIndex = std::stoi(indexStr) - 1;
                }

                faceIndices.push_back(std::make_tuple(posIndex, texIndex, norIndex));
            }

            if(faceIndices.size() == 3) {
                rawIndices.push_back(faceIndices[0]);
            } else if(faceIndices.size() == 4) {
                rawIndices.push_back(faceIndices[0]);
                rawIndices.push_back(faceIndices[1]);
                rawIndices.push_back(faceIndices[2]);

                rawIndices.push_back(faceIndices[0]);
                rawIndices.push_back(faceIndices[2]);
                rawIndices.push_back(faceIndices[3]);
            } else {
                // TODO crash or some, idk. Can't handle this
            }
        }
    }

    // Add some values in case none existed
    if(positions.size() <= 0) {
        positions.push_back(vec3(0.0f));
    }

    if(normals.size() <= 0) {
        normals.push_back(vec3(0.0f));
    }

    if(texs.size() <= 0) {
        texs.push_back(vec2(0.0f));
    }

    file.close();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::map<std::tuple<unsigned int, unsigned int, unsigned int>, unsigned int> indexMap;

    for(const std::tuple<unsigned int, unsigned int, unsigned int> &rawIndex : rawIndices) {
        if(indexMap.find(rawIndex) != indexMap.end()) {
            // Reuse index
            indices.push_back(indexMap[rawIndex]);
        } else {
            // Create new vertex from rawIndex
            const vec3 &pos = positions[std::get<0>(rawIndex)];
            const vec2 &tex =      texs[std::get<1>(rawIndex)];
            const vec3 &nor =   normals[std::get<2>(rawIndex)];
            
            vertices.push_back(Vertex(pos, nor, tex));

            // Use index of new vertex
            indices.push_back(vertices.size() - 1);

            // Add index to map
            indexMap[rawIndex] = vertices.size() - 1;
        }
    }

    // Work out tangents and bitangents for each triangle
    for(int i = 0; i < indices.size(); i += 3) {
        // Get vertices
        Vertex &v0 = vertices[indices[i]];
        Vertex &v1 = vertices[indices[i + 1]];
        Vertex &v2 = vertices[indices[i + 2]];

        // Get edges in model space
        vec3 edge1 = v1.position - v0.position;
        vec3 edge2 = v2.position - v0.position;

        // Get edges in texture space
        vec2 deltaUV1 = v1.uv - v0.uv;
        vec2 deltaUV2 = v2.uv - v0.uv;

        // Calculate tangent and bitangent
        const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        const vec3 tangent   = (deltaUV2.y * edge1 - deltaUV1.y * edge2) * f;
        const vec3 bitangent = (deltaUV2.x * edge1 - deltaUV1.x * edge2) * f;

        // Add to vertices values (they start at 0.0f)
        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    // Normalize tangent and bitangent
    for(int i = 0; i < vertices.size(); i++) {
        // Get vertex, tangent and bitangent
        Vertex &vertex    = vertices[i];
        vec3   &tangent   = vertex.tangent;
        vec3   &bitangent = vertex.bitangent; 

        // Normalize values
        tangent   = glm::normalize(tangent - glm::dot(tangent, vertex.normal) * vertex.normal);
        bitangent = glm::normalize(bitangent - glm::dot(bitangent, vertex.normal) * vertex.normal - glm::dot(bitangent, tangent) * tangent);
    }

    // Move data to GPU
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return indices.size();
}