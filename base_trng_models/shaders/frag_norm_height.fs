#version 330 core
in vec3 ourColor;
//in vec3 ourColorView;
in vec3 v_Position;
in vec3 v_materials;
in vec2 TexCoord;
//in vec3 Normal;
in mat3 TBN;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
uniform sampler2D HeightMap; 

uniform sampler2D Albedo_0;
uniform sampler2D Normal_0;
uniform sampler2D Utility_0;

uniform sampler2D Albedo_1;
uniform sampler2D Normal_1;
uniform sampler2D Utility_1;

uniform sampler2D Albedo_2;
uniform sampler2D Normal_2;
uniform sampler2D Utility_2;

void main()
{
    float tex_mul = 80.0;
    vec4 notUsed = texture2D(HeightMap, vec2(0.0, 0.0));

    vec2 Coord = TexCoord*tex_mul;
	//vec4 texColor = vec4(0.9,0.5,0.2,1.0);//texture(AlbedoTexture, TexCoord);
	vec4 texColor = texture(Albedo_0, Coord);
    //if(texColor.a < 0.1)
    //    discard;

    //vec3 utility = texture(UtilityTexture, TexCoord*tex_mul).xyz;//texture(UtilityTexture, TexCoord).xyz;
    //utility.x = 0.0;
    //utility.z = 0.0;
    //vec3 utility = vec3(0.0,0.9,0.1);
    float r_intensivity = 0.6;
    vec3 utility = vec3(0.0,1.0 - r_intensivity + texture(Utility_0, Coord).y * r_intensivity,0.1);
    //vec3 utility = vec3(0.0,1.0 - texture(UtilityTexture, TexCoord*tex_mul).y *0.5,0.1);


    vec4 pos = vec4(v_Position,utility.x);
    gPosition = pos;

    //vec3 normal = vec3(0.0,1.0,0.0);
    vec3 normal = texture(Normal_0, Coord).xyz;

    normal = normalize(normal * 2.0 - 1.0); 
    normal = normalize(TBN * normal); 
    //normal = normal * 0.5 + 0.5; 
    
    gNormal = vec4(normal.xyz, utility.y);
    //gNormal = vec4(vec3(0.0,1.0,0.0), utility.y);

	gAlbedoSpec = vec4(texColor.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(normal.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(ourColor.xyz, 0.06 + utility.z*0.94);//texColor;

	
	//gAlbedoSpec = gNormal;
}