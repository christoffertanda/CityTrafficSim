struct input
{
	float4 Position	: SV_POSITION;
    float2 UV		: TEXCOORD;
    float1 IsPicked : ISPICKED;
};

Texture2D Texture;

SamplerState Sampler;

float4 main(input Input) : SV_Target
{
    float4 Color;
    
    if (Input.IsPicked > 0.0f)
    {
        // NOTE(Cristoffer): If pixel is marked as picked, do the extra coloring here.
        Color = Texture.Sample(Sampler, Input.UV);
        Color = Color * float4(1.2f, 1.2f, 1.2f, 1.0f);
    }
    else
    {
        Color = Texture.Sample(Sampler, Input.UV);
    }
    
    return Color;
}