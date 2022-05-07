cbuffer perFrameBuffer {
    float4x4 projMatrix;
    float4x4 cameraMatrix;
	float4 lightPositions[4];
	float4 lightColours[4];
	float4 camPos;
}

cbuffer perObjectBuffer {
    float4x4 modelMatrix;
}

SamplerState textureSampler;

TextureCube irradianceMap;
TextureCube preFilterMap;
Texture2D brdfLUT;

Texture2D normalMap : register(t3);
Texture2D roughnessMap : register(t4);
Texture2D metallicMap : register(t5);

static const float PI = 3.14159265359;

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
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
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float4 main(float4 worldPos : SV_POSITION, float3 normal : NORMAL, float4 color : COLOR, float2 uv : TEXCOORD) : SV_TARGET
{
	float3 WorldPos = worldPos;
	
	float3 albedo = color.rgb;
	
	float3 Normal = normal * normalMap.Sample(textureSampler, uv).rgb;
	float roughness = roughnessMap.Sample(textureSampler, uv).r;
	float metallic = metallicMap.Sample(textureSampler, uv).r;

	float ao = 1.0;

	float3 N = normalize(Normal);
    float3 V = normalize(camPos.xyz - WorldPos);
	float3 R = reflect(-V, N);

    float3 F0 = float3(0.04, 0.04, 0.04); 
    F0 = lerp(F0, albedo, metallic);
	           
    // reflectance equation
    float3 Lo = float3(0.0, 0.0, 0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        float3 L = normalize(lightPositions[i].xyz - WorldPos);
        float3 H = normalize(V + L);
        float distance    = length(lightPositions[i].xyz - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        //float attenuation = 10.0 / (distance);
		float3 radiance     = lightColours[i].xyz * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        float3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        float3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        float3 specular     = numerator / max(denominator, 0.001);  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   

	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	float3 kS = F;
	float3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	float3 irradiance = irradianceMap.Sample(textureSampler, N).rgb;
	float3 diffuse = irradiance * albedo;

	const float MAX_REFLECTION_LOD = 4.0;
	float3 prefilteredColor = preFilterMap.SampleLevel(textureSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
	float2 envBRDF = brdfLUT.Sample(textureSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
	float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	float3 ambient = (kD * diffuse + specular) * ao;

    float3 c = ambient + Lo;
	
    c = c / (c + float3(1.0, 1.0, 1.0));
    c = pow(c, float3(1.0/2.2, 1.0/2.2, 1.0/2.2));  
   
    return float4(prefilteredColor, 1.0);
}