#version 330 core

in vec2  TexCoords;

uniform sampler2D textureMap;

out float fragColor;

void main() {
    // if(texture(textureMap, TexCoords).a < 0.0) {
    //     discard; // Don't count transparent stuff
    // }
}