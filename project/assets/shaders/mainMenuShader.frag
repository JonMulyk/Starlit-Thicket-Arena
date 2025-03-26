#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;
uniform bool useTexture;
uniform vec3 solidColor;
void main() {
    if (useTexture) {
        FragColor = texture(texture1, TexCoord);
    } else {
        FragColor = vec4(solidColor, 1.0);
    }
}