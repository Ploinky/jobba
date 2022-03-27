cbuffer perObjectBuffer {
    float4x4 worldViewMatrix;
}

void main(float3 position : POSITION, float4 color : COLOR, out float4 positionOut : SV_POSITION, out float4 colorOut : COLOR) {
    positionOut = mul(worldViewMatrix, float4(position, 1.0f));

    colorOut = color;
}