#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
uniform vec3 overrideColor;
uniform bool useOverrideColor;

uniform sampler2D texture0;

void main()
{
	if(useOverrideColor)
	{
		vec4 tex = texture(texture0, TexCoord);
		FragColor = vec4(overrideColor, 1.0) * tex;
	}
	else
	{
		FragColor = texture(texture0, TexCoord);
	}
}