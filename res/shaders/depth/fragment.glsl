#version 330 core

in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D textureMap;

uniform vec3  position;
uniform vec3  direction;
uniform float coneAngle;

void main() {
    // if(texture(textureMap, TexCoords).a < 0.0) {
    //     discard; // Don't count transparent stuff
    // }
}