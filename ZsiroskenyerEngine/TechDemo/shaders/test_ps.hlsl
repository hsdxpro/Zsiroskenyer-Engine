// ps_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile ps_5_0
// source file: shaders/test.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile ps_5_0
//program PS_MAIN
//semantic wvp : C0
//semantic world : C4
//semantic camPos : C8
//semantic diffuseTex : TEXUNIT0
//semantic normalTex : TEXUNIT1
//var float3 camPos : C8 : _camPos : -1 : 1
//var sampler2D diffuseTex : TEXUNIT0 : _diffuseTex 0 : -1 : 1
//var sampler2D normalTex : TEXUNIT1 : _normalTex 1 : -1 : 1
//var float4 In.posH : $vin.SV_POSITION :  : 0 : 0
//var float3 In.normalW : $vin.TEXCOORD0 :  : 0 : 0
//var float3 In.tangentW : $vin.TEXCOORD1 :  : 0 : 0
//var float2 In.tex0 : $vin.TEXCOORD2 :  : 0 : 1
//var float3 In.posW : $vin.TEXCOORD3 :  : 0 : 1
//var float3x3 In.TBN : $vin.TEXCOORD4 : , 3 : 0 : 1
//var float4 PS_MAIN.color : $vout.COLOR : COLOR : -1 : 1

#pragma pack_matrix(row_major)

struct VS_IN {
    float3 _posL : SV_Position;
    float3 _normalL : NORMAL;
    float3 _tangentL : TANGENT;
    float2 _tex0 : TEXCOORD0;
};

struct VS_OUT {
    float4 _posH : SV_POSITION;
    float3 _normalW1 : TEXCOORD0;
    float3 _tangentW : TEXCOORD1;
    float2 _tex01 : TEXCOORD2;
    float3 _posW : TEXCOORD3;
    float3x3 _TBN : TEXCOORD4;
};

struct PS_OUT {
    float4 _color : SV_Target;
};

struct X1X {
    float4 _SV_POSITION : SV_POSITION;
    float3 _TEXCOORD0 : TEXCOORD0;
    float3 _TEXCOORD1 : TEXCOORD1;
    float2 _TEXCOORD2 : TEXCOORD2;
    float3 _TEXCOORD3 : TEXCOORD3;
    float3 _TEXCOORD4 : TEXCOORD4;
    float3 _TEXCOORD5 : TEXCOORD5;
    float3 _TEXCOORD6 : TEXCOORD6;
};

static float4 _TMP7;
static float _TMP6;
static float _TMP5;
static float _TMP4;
static float _TMP9;
static float _TMP8;
static float _TMP2;
static float _TMP10;
static float _TMP11;
static float _TMP1;
static float4 _TMP0;
static float3 _v0013;
static float3 _r0015;
static float3 _v0017;
static float _TMP18;
static float3 _v0019;
static float3 _v0023;
Texture2D<float4> _TMP24 : register(t1);
Texture2D<float4> _TMP25 : register(t0);
float3 _camPos : register(c8);
SamplerState _diffuseTex : TEXUNIT0;
SamplerState _normalTex : TEXUNIT1;

 // main procedure, the original name was PS_MAIN
PS_OUT main( in X1X cin)
{

    VS_OUT _In;
    PS_OUT _pout;
    float3 _normalT;
    float3 _lightVec;
    float _intensity;
    float3 _h1;

    _In._tex01 = cin._TEXCOORD2;
    _In._posW = cin._TEXCOORD3;
    _In._TBN[0] = cin._TEXCOORD4;
    _In._TBN[1] = cin._TEXCOORD5;
    _In._TBN[2] = cin._TEXCOORD6;
    _TMP0 = _TMP24.Sample(_normalTex, _In._tex01);
    _v0013 = _TMP0.xyz* 2.00000000000000000E000f -  1.00000000000000000E000f;
    _TMP8 = dot(_v0013, _v0013);
    _TMP9 = rsqrt(_TMP8);
    _normalT = _TMP9*_v0013;
    _r0015 = _normalT.x*_In._TBN._11_12_13;
    _r0015 = _r0015 + _normalT.y*_In._TBN._21_22_23;
    _r0015 = _r0015 + _normalT.z*_In._TBN._31_32_33;
    _v0017 = float3(  7.00000000000000000E001f,  3.00000000000000000E001f,  0.00000000000000000E000f) - _In._posW;
    _TMP8 = dot(_v0017, _v0017);
    _TMP9 = rsqrt(_TMP8);
    _lightVec = _TMP9*_v0017;
    _TMP1 = dot(_r0015, _lightVec);
    _TMP2 = saturate(_TMP1);
    _v0019 = _In._posW - float3(  7.00000000000000000E001f,  3.00000000000000000E001f,  0.00000000000000000E000f);
    _TMP10 = dot(_v0019, _v0019);
    _TMP11 = rsqrt(_TMP10);
    _TMP18 =  1.00000000000000000E000f/_TMP11;
    _intensity = (_TMP2/_TMP18)* 8.00000000000000000E001f;
    if (_intensity >  0.00000000000000000E000f) { // if begin
        _v0023 = (_lightVec + _camPos) - _In._posW;
        _TMP8 = dot(_v0023, _v0023);
        _TMP9 = rsqrt(_TMP8);
        _h1 = _TMP9*_v0023;
        _TMP4 = dot(_r0015, _h1);
        _TMP5 = saturate(_TMP4);
        _TMP6 = pow(_TMP5,  2.55000000000000000E002f);
        _intensity = _intensity + _TMP6;
    } // end if
    _TMP7 = _TMP25.Sample(_diffuseTex, _In._tex01);
    _pout._color = _TMP7*float4(_intensity, _intensity, _intensity,  1.00000000000000000E000f);
    return _pout;
} // main end
