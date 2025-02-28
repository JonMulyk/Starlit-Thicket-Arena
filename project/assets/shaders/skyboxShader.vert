#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    
    // Set w = 1.0 but ignore translation by using mat3(view)
    vec4 pos = projection * vec4(mat3(view) * aPos, 1.0);
    
    gl_Position = pos.xyww; // Force w to 1 to make depth always max (far plane)
}