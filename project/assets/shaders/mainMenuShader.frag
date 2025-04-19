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
        vec4 texColor = texture(texture1, TexCoord);
        if (texColor.a < 0.1) {
            discard; 
        }
        FragColor = texColor;
    } 
    else if (useSolidColor) {
        FragColor = vec4(solidColor, 1.0);
    } 
    else {
        FragColor = boxColor;
    }
}
