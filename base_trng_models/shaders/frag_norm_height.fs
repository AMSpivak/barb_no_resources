#version 330 core
in vec3 ourColor;
//in vec3 ourColorView;
in vec3 v_Position;
in vec2 TexCoord;
in vec3 Normal;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
uniform sampler2D HeightMap; 
//uniform sampler2D ourTexture;
//uniform sampler2D UtilityTexture;
//uniform sampler2D NormalTexture;
void main()
{

    vec4 notUsed = texture2D(HeightMap, vec2(0.0, 0.0));

	vec4 texColor = vec4(0.9,0.5,0.2,1.0);//texture(ourTexture, TexCoord);
    //if(texColor.a < 0.1)
    //    discard;

    vec3 utility = vec3(0.0,0.9,0.1);//texture(UtilityTexture, TexCoord).xyz;
    vec4 pos = vec4(v_Position,utility.x);
    gPosition = pos;
    //vec3 normal = vec3(0.0,1.0,0.0);
    //vec3 normal = texture(NormalTexture, TexCoord).xyz;
    //normal = normalize(normal * 2.0 - 1.0); 
    //normal = normalize(TBN * normal); 
    //normal = normal * 0.5 + 0.5; 
    
    gNormal = vec4(ourColor.xyz, utility.y);
    //gNormal = vec4(vec3(0.0,1.0,0.0), utility.y);

	gAlbedoSpec = vec4(texColor.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(ourColor.xyz, 0.06 + utility.z*0.94);//texColor;

	
	//gAlbedoSpec = gNormal;
}