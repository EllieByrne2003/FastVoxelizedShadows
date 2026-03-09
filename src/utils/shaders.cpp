#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

GLuint createShader(const std::string &vertexPath, const std::string &fragmentPath) {
    // Load files into strings
    std::string vertexCode;
    std::ifstream vertexFile(vertexPath);
    if(vertexFile.is_open()) {
        std::stringstream sstr;
        sstr << vertexFile.rdbuf();
        vertexCode = sstr.str();
        vertexFile.close();
    } else {
        std::cerr << "Couldn't load: " << vertexPath << std::endl;
        return 0;
    }

    std::string fragmentCode;
    std::ifstream fragmentFile(fragmentPath);
    if(fragmentFile.is_open()) {
        std::stringstream sstr;
        sstr << fragmentFile.rdbuf();
        fragmentCode = sstr.str();
        fragmentFile.close();
    } else {
        std::cerr << "Couldn't load: " << fragmentPath << std::endl;
        return 0;
    }

    // Error checking info
    int ok = 0;
    int infoLogLength;

    // Compile vertex shader
    unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    const char *const vertexCodePtr = vertexCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexCodePtr, NULL);
    glCompileShader(vertexShaderID);

    // Check vertex compilation
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &ok);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(!ok) {
        if(infoLogLength > 0) {
            std::vector<char> log(infoLogLength + 1);
            glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &log[0]);
            std::cerr << &log[0] << std::endl;
        }

        glDeleteShader(vertexShaderID);

        return 0;
    }

    // Compile fragment shader
    unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    const char *const fragmentCodePtr = fragmentCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentCodePtr, NULL);
    glCompileShader(fragmentShaderID);

    // Check fragment compilation
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &ok);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(!ok) {
        if(infoLogLength > 0) {
            std::vector<char> log(infoLogLength + 1);
            glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &log[0]);
            std::cerr << &log[0] << std::endl;
        }

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        return 0;
    }

    // Link program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
        
    // Check program linking
    glGetProgramiv(programID, GL_LINK_STATUS, &ok);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

    if(!ok) {
        if(infoLogLength > 0) {
            std::vector<char> log(infoLogLength + 1);
            glGetProgramInfoLog(programID, infoLogLength, NULL, &log[0]);
            std::cerr << &log[0] << std::endl;
        }
        
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        glDeleteProgram(programID);

        return 0;
    }

    // Cleanup vertex and fragment
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}