#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	
	vec4 Color = texture(skybox, TexCoords);
	//FragColor = vec4(TexCoords.xyz,1.0);
	FragColor = vec4(Color.xyz,1.0);

}