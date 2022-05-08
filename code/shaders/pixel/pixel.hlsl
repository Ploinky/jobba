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


float4 main(float4 worldPos : SV_POSITION, float4 wPosIn : TEXCOORD1, float3 normal : NORMAL, float2 uv : TEXCOORD) : SV_TARGET
{
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 att = float4(0.0f, 0.1f, 0.1f, 1.0f);

    // normal map vector rotated relative to vertex normal
    normal = normalize(normalMap.Sample(textureSampler, uv).rgb * normal);

    float4 diffuse = albedoMap.Sample( textureSampler, uv );

    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    //Create the vector between light position and pixels position
    float3 lightToPixelVec = pointLights[0].position.xyz - wPosIn;
        
    //Find the distance between the light pos and pixel pos
    float d = length(lightToPixelVec);
    
    //Create the ambient light
    float3 finalAmbient = diffuse * ambient;

    //If pixel is too far, return pixel color with ambient light
    if( d > 20 )
        return float4(finalAmbient, diffuse.a);
        
    //Turn lightToPixelVec into a unit length vector describing
    //the pixels direction from the lights position
    lightToPixelVec /= d; 
    
    //Calculate how much light the pixel gets by the angle
    //in which the light strikes the pixels surface
    float howMuchLight = saturate(max(0, dot(normal, lightToPixelVec)));
    // ----------------------------------------------------------- //

    //If light is striking the front side of the pixel
    if( howMuchLight > 0.0f )
    {    
        //Add light to the finalColor of the pixel
        finalColor += howMuchLight * diffuse * pointLights[0].color;
        
        //Calculate Light's Falloff factor
        finalColor /= att[0] + (att[1] * d) + (att[2] * (d*d));
    }
        
    //make sure the values are between 1 and 0, and add the ambient
    finalColor = saturate(finalColor + finalAmbient);
    
    //Return Final Color
    return float4(finalColor, diffuse.a);
}