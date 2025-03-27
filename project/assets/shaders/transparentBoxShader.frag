#version 330 core
out vec4 FragColor;
uniform vec4 boxColor; // (R, G, B, Alpha)

void main() {
    FragColor = boxColor;
}
