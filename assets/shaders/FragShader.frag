#version 330 core

out vec4 FragColor;
in vec3 tColor;

void main() {
    FragColor = vec4(tColor, 1.0f);
}
