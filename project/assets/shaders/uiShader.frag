#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform vec4 barColor;

uniform sampler2D texture0;

void main()
{
	FragColor = barColor;
}
