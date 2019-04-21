#version 330 core
in vec3 ourColor;
//in vec3 ourColorView;
in vec3 v_Position;
in vec2 TexCoord;
in mat3 TBN;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
uniform sampler2D ourTexture;
uniform sampler2D UtilityTexture;
uniform sampler2D NormalTexture;
void main()
{
    float tex_mul = 80.0;
    vec2 TexCoord_X = vec2(v_Position.y,v_Position.z);
    vec2 TexCoord_Y = vec2(v_Position.x,v_Position.z);
    vec2 TexCoord_Z = vec2(v_Position.x,v_Position.y);

    vec3 weight = abs(normalize(TBN * vec3(0.0,0.0,1.0)));
    vec4 texColor = weight.x*texture(ourTexture, TexCoord_X) + weight.y*texture(ourTexture, TexCoord_Y) + weight.z*texture(ourTexture, TexCoord_Z);

    // if(texColor.a < 0.1)
    //     discard;

    vec3 utility = weight.x*texture(UtilityTexture, TexCoord_X).xyz + weight.y*texture(UtilityTexture, TexCoord_Y).xyz + weight.z*texture(UtilityTexture, TexCoord_Z).xyz;
    
    vec4 pos = vec4(v_Position,utility.x);
    gPosition = pos;
    // vec3 normal = vec3(0.0,0.0,1.0);//texture(NormalTexture, TexCoord).xyz;
    

    vec3 normal = weight.x*texture(NormalTexture, TexCoord_X).xyz + weight.y*texture(NormalTexture, TexCoord_Y).xyz + weight.z*texture(NormalTexture, TexCoord_Z).xyz;
    normal = normalize(normal * 2.0 - 1.0); 
    normal = normalize(TBN * normal); 
    //normal = normal * 0.5 + 0.5; 
    
    gNormal = vec4(normal, utility.y);
    //gNormal = vec4(ourColor.xyz, utility.y);
    //gNormal = vec4(normal.xyz, utility.y);
    float val = length(normal);

	// gAlbedoSpec = vec4(normal.xyz, 0.06 + utility.z*0.94);//texColor;
	gAlbedoSpec = vec4(texColor.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(normal, 0.06 + utility.z*0.94);//texColor;
	
	//gAlbedoSpec = gNormal;
}