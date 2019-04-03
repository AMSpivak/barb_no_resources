#version 330 core

#define M_PI 3.1415926535897932384626433832795


//out vec4 FragColor;
in vec2 TexCoords;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;


uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;
uniform sampler2DShadow shadowMap;
//uniform sampler2D shadowMap;

uniform mat4 lightSpaceMatrix;

uniform vec3 LightDir;
uniform vec3 LightColor;
uniform vec3 viewPos;

float ShadowCalculation(vec4 PosLight, vec3 tNormal)
{
    
    vec4 fragPosLightSpace = lightSpaceMatrix * PosLight;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    if(fragPosLightSpace.x<-1.0||fragPosLightSpace.x>1.0||fragPosLightSpace.y<-1.0||fragPosLightSpace.y>1.0)
        return 1.0;
        //discard;

    float light_fall = dot(tNormal,LightDir);

        //    return 0.0;
        projCoords = projCoords * 0.5 + 0.5;
        float currentDepth = projCoords.z;

        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        //float bias = max(0.01 * smoothstep(0.65,0.85,(1.0 - max(dot(tNormal,LightDir),0.0))), 0.005);
        //light_fall = light_fall*light_fall;
        float sqr_ang = (1.0 - max(light_fall,0.0));
        sqr_ang =sqr_ang ;//* sqr_ang* sqr_ang;
        float bias = 0.02 * sqr_ang;
        //float bias = 0.0005*tan(acos(max(dot(tNormal,LightDir),0.0))); // cosTheta is dot( n,l ), clamped between 0 and 1
        //float bias = max(0.05 * (1.0 - dot(tNormal,LightDir)), 0.005);
        currentDepth -= 0.1*bias;
        vec2 shift = texelSize*vec2(0.0,1.0);
        //float res = (textureProj(shadowMap,vec4(projCoords,1.0)));
        float res = texture(shadowMap,vec3(projCoords.xy,currentDepth));
    //    float res = texture(shadowMap,vec3(projCoords.xy,currentDepth - bias));
        //float depth = texture(shadowMap,projCoords.xy).r;
        //float res = smoothstep(-0.1,0.1,depth - currentDepth);

    //  res = smoothstep(0.0,1.0, (texture(shadowMap,projCoords.xy) - currentDepth + bias));

        // res *=1.0;
        // float swadowing = 0.0;
        // shift = texelSize*vec2(-2.0,2.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));
        // shift =texelSize*vec2(2.0,-2.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));
        // shift =texelSize*vec2(2.0, 2.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));
        // shift = texelSize*vec2(-2.0,-2.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));

        // if(swadowing + res < 1.35)  return 0.0;

        //float swadowing =0.6 * res;
        float swadowing = res;
        // shift = texelSize*vec2(0.0,1.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));
        // shift =texelSize*vec2(0.0,-1.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));
        // shift =texelSize*vec2(1.0, 0.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));
        // shift = texelSize*vec2(-1.0, 0.0);
        // swadowing += 0.1*(texture(shadowMap,vec3(projCoords.xy + shift,currentDepth)));
        
        return swadowing*swadowing;

}

float GGX_PartialGeometry(float cosThetaN, float alpha) {
    float cosTheta_sqr = clamp(cosThetaN*cosThetaN, 0.0, 1.0);
    float tan2 = ( 1 - cosTheta_sqr ) / cosTheta_sqr;
    float GP = 2 / ( 1 + sqrt( 1 + alpha * alpha * tan2 ) );
    return GP;
}

float GGX_Distribution(float cosThetaNH, float alpha) {
    float alpha2 = alpha * alpha;
    float NH_sqr = clamp(cosThetaNH*cosThetaNH, 0.0, 1.0);
    float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
    return alpha2 / ( M_PI * den * den );
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.10);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


void main()
{
    // gAlbedoSpec =vec4(1.0,1.0,1.0,1.0);

    // gNormal =vec4(0.0,0.0,0.0,1.0);



	vec4 texColor = texture(DiffuseMap, TexCoords);
    if(texColor.a < 0.05)
        discard;
    vec4 normal_map = texture(NormalMap, TexCoords);
	vec3 texNormal= normal_map.xyz;

	vec3 FragPos= texture(PositionMap, TexCoords).xyz;
	float norm_l = dot(texNormal,LightDir);
    
     
    if(norm_l > 0.01)
    {
	    norm_l = max(norm_l,0.01);
        
        vec3 viewDir = normalize(viewPos - FragPos);

        vec3 halfwayDir = normalize(LightDir + viewDir); 
        float dotHV = max(dot(viewDir, halfwayDir), 0.0);
        float dotNH = max(dot(texNormal, halfwayDir), 0.0);
        float dotNV = max(dot(texNormal, viewDir), 0.10);

        vec3 F0 = vec3(0.04);
        F0      = mix(F0, texColor.rgb, texColor.a);

        vec3 shlick =fresnelSchlick(dotNV,F0);

        float roug_sqr = (normal_map.w)*(normal_map.w);
    

        float D =  DistributionGGX(texNormal, halfwayDir, roug_sqr);       
        float G   = GeometrySmith(texNormal, viewDir, LightDir, roug_sqr); 



        float shadow_res = (ShadowCalculation(vec4(FragPos.xyz,1.0),texNormal));
        //float shadow_norm =1.0 -  smoothstep(0.6,1.0,1.0-norm_l);//norm_l;//smoothstep(shadow_res,1.0,norm_l);
        //shadow_res = 1.0 - (1.0 - shadow_res) ;//* norm_l;

        vec3 numerator    = D * G * shlick;
        float denominator = 4.0 * max(dotNV, 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);
        vec3 kS = shlick;
        vec3 kD = vec3(1.0) - kS;

        vec3 diffuse =kD/M_PI;
        vec3 ShadowLightColor = shadow_res* LightColor;
        gAlbedoSpec =vec4(ShadowLightColor *norm_l* diffuse,1.0);
        gNormal =vec4(ShadowLightColor * specular,1.0);

    }
    else
    {
        gAlbedoSpec =vec4(0.0,0.0,0.0,1.0);
        gNormal =vec4(0.0,0.0,0.0,1.0);
    }
    //gNormal =vec4(0.0);
}
