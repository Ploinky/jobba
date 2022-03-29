cbuffer perObjectBuffer {
    float4x4 worldViewMatrix;
    float4x4 modelMatrix;
}

void main(float3 position : POSITION, float4 color : COLOR, out float4 positionOut : SV_POSITION, out float4 colorOut : COLOR) {
    positionOut = float4(position, 1.0f);
    positionOut = mul(positionOut, modelMatrix);
    positionOut = mul(positionOut, worldViewMatrix);
    colorOut = color;
}