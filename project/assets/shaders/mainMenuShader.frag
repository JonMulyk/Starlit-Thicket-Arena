#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform bool useTexture;
uniform bool useSolidColor;

uniform vec3 solidColor;
uniform vec4 boxColor;

void main() {
    if (useTexture) {
        FragColor = texture(texture1, TexCoord);
    } 
    else if (useSolidColor) {
        FragColor = vec4(solidColor, 1.0);
    } 
    else {
        FragColor = boxColor;
    }
}
