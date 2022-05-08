cbuffer perFrameBuffer {
    float4x4 projMatrix;
    float4x4 cameraMatrix;
    struct pointLight {
        float4 position;
        float4 color;
    } pointLights[4];
	float4 camPos;
}

cbuffer perObjectBuffer {
    float4x4 modelMatrix;
}

SamplerState textureSampler;

Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D roughnessMap : register(t2);
Texture2D metallicMap : register(t3);

float4 main(float4 worldPos : SV_POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD) : SV_TARGET
{
   float3 ToLight = pointLights[0].position.xyz - worldPos;
   float3 ToEye = camPos.xyz - worldPos;
   float DistToLight = length(ToLight);
   
   // Phong diffuse
   ToLight /= DistToLight; // Normalize
   float NDotL = saturate(dot(ToLight, normalMap.Sample(textureSampler, uv).rgb));
   float3 finalColor = pointLights[0].color.rgb * NDotL;
   
   // Blinn specular
   ToEye = normalize(ToEye);
   float3 HalfWay = normalize(ToEye + ToLight);
   float NDotH = saturate(dot(HalfWay, normalMap.Sample(textureSampler, uv).rgb));
   finalColor += pointLights[0].color.rgb * pow(NDotH, roughnessMap.Sample(textureSampler, uv).rgb) * roughnessMap.Sample(textureSampler, uv).rgb;
   
   // Attenuation
   float DistToLightNorm = 1.0 - saturate(DistToLight * 1); // PointLightRangeRcp);
   float Attn = DistToLightNorm * DistToLightNorm;  
   finalColor *= albedoMap.Sample(textureSampler, uv).rgb * Attn;
   
    return float4(worldPos / 1000);
}