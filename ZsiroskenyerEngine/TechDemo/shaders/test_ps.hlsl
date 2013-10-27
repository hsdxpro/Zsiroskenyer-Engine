// ps_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile ps_5_0
// source file: shaders/test.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile ps_5_0
//program PS_MAIN
//semantic wvp
//semantic world
//semantic diffuseTex
//var float4 In.posH : $vin.SV_POSITION :  : 0 : 0
//var float3 In.normalW : $vin.TEXCOORD0 :  : 0 : 1
//var float2 In.tex0 : $vin.TEXCOORd1 :  : 0 : 0
//var float3 In.posW : $vin.TEXCOORD2 :  : 0 : 1
//var float4 PS_MAIN.color : $vout.COLOR : COLOR : -1 : 1

#pragma pack_matrix(row_major)

struct VS_IN {
    float3 _posL : SV_Position;
    float3 _normalL : NORMAL;
    float2 _tex0 : TEXCOORD0;
};

struct VS_OUT {
    float4 _posH : SV_POSITION;
    float3 _normalW : TEXCOORD0;
    float2 _tex01 : TEXCOORD1;
    float3 _posW : TEXCOORD2;
};

struct PS_OUT {
    float4 _color : SV_Target;
};

struct X1X {
    float4 _SV_POSITION : SV_POSITION;
    float3 _TEXCOORD0 : TEXCOORD0;
    float3 _TEXCOORD2 : TEXCOORD2;
    float2 _TEXCOORd1 : TEXCOORD1;
};

static float _TMP1;
static float _TMP0;
static float3 _v0003;

 // main procedure, the original name was PS_MAIN
PS_OUT main( in X1X cin)
{

    VS_OUT _In;
    PS_OUT _pout;
    float3 _lightVec;
    float _intensity;

    _In._normalW = cin._TEXCOORD0;
    _In._posW = cin._TEXCOORD2;
    _v0003 = float3(  2.00000000000000000E001f,  5.00000000000000000E001f,  0.00000000000000000E000f) - _In._posW;
    _TMP0 = dot(_v0003, _v0003);
    _TMP1 = rsqrt(_TMP0);
    _lightVec = _TMP1*_v0003;
    _intensity = dot(_In._normalW, _lightVec);
    _pout._color = float4(_intensity, _intensity, _intensity,  1.00000000000000000E000f);
    return _pout;
} // main end
