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

void main(float3 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, out float4 positionOut : SV_POSITION, out float4 wPosOut : TEXCOORD1, out  float3 normalOut : NORMAL, out float2 uvOut : TEXCOORD) {
    positionOut = float4(position, 1.0f);
    positionOut = mul(positionOut, modelMatrix);
    positionOut = mul(positionOut, cameraMatrix);
    positionOut = mul(positionOut, projMatrix);
    wPosOut = mul(float4(position, 1.0f), modelMatrix);
    uvOut = uv;
    normalOut = normal;
}