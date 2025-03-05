#version 330 core
out vec4 FragColor;

struct Light
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
 
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform sampler2D texture0;
uniform Light light;

void main()
{
	// ambient
	vec3 ambient = light.ambient * lightColor;
	
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * lightColor;

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = light.specular * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * texture(texture0, TexCoord).rgb;
	FragColor = vec4(result, 1.0);
}
