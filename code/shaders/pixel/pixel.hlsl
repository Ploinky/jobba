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

Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D roughnessMap : register(t2);
Texture2D metallicMap : register(t3);

float4 main(float4 worldPos : SV_POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD) : SV_TARGET
{
    return float4(albedoMap.Sample(textureSampler, uv).rgb, 1.0);
}