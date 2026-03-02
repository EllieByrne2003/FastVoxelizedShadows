{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = with pkgs; [
        git
        gcc
        gdb
        gnumake
        bear
        valgrind

        libGL
        glfw
        glew
        glm
        imgui
        nlohmann_json
        gpu-screen-recorder-gtk
        pavucontrol
    ];
}