#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	
	vec4 Color = texture(skybox, TexCoords);
	float l=smoothstep(0.0,0.3, TexCoords.y);
	// l*=0.0;
	vec3 atmosphere = vec3(0.9,0.9,1.0);
	vec3 sky = vec3(0.0,0.6,1.0);
	//FragColor = vec4(TexCoords.xyz,1.0);
	//FragColor = vec4(Color.xyz,1.0);
	FragColor = vec4(atmosphere*(1.0-l)+sky*l,1.0);

}