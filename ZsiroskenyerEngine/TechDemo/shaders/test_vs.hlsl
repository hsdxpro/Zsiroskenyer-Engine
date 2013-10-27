// vs_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile vs_5_0
// source file: shaders/test.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile vs_5_0
//program VS_MAIN
//semantic wvp
//semantic world
//semantic diffuseTex
//var float4x4 wvp :  : _wvp[0], 4 : -1 : 1
//var float3 In.posL : $vin.POSITION : POSITION : 0 : 1
//var float3 In.normalL : $vin.NORMAL : NORMAL : 0 : 1
//var float2 In.tex0 : $vin.TEXCOORD0 : TEXCOORD0 : 0 : 1
//var float4 VS_MAIN.posH : $vout.SV_POSITION :  : -1 : 1
//var float3 VS_MAIN.normalW : $vout.TEXCOORD0 :  : -1 : 1
//var float2 VS_MAIN.tex0 : $vout.TEXCOORd1 :  : -1 : 1
//var float3 VS_MAIN.posW : $vout.TEXCOORD2 :  : -1 : 1

#pragma pack_matrix(row_major)

struct VS_IN {
    float3 _posL : POSITION;
    float3 _normalL : NORMAL0;
    float2 _tex0 : TEXCOORD0;
};

struct VS_OUT {
    float4 _posH : SV_POSITION;
    float3 _normalW : TEXCOORD0;
    float2 _tex01 : TEXCOORD1;
    float3 _posW : TEXCOORD2;
};

struct PS_OUT {
    float4 _color : COLOR0;
};

struct X1X {
    float4 _SV_POSITION : SV_POSITION;
    float3 _TEXCOORD0 : TEXCOORD0;
    float3 _TEXCOORD2 : TEXCOORD2;
    float2 _TEXCOORd1 : TEXCOORD1;
};

static X1X cout;
float4x4 _wvp;

 // main procedure, the original name was VS_MAIN
X1X main(in VS_IN _In)
{

    VS_OUT _vout;

    _vout._posW = float3(  0.00000000000000000E000f,  0.00000000000000000E000f,  0.00000000000000000E000f);
    _vout._posH = _In._posL.x*_wvp._11_12_13_14 + _In._posL.y*_wvp._21_22_23_24 + _In._posL.z*_wvp._31_32_33_34 + _wvp._41_42_43_44;
    _vout._normalW = _In._normalL;
    _vout._tex01 = _In._tex0;
    cout._SV_POSITION = _vout._posH;
    cout._TEXCOORD0 = _vout._normalW;
    cout._TEXCOORd1 = _vout._tex01;
    cout._TEXCOORD2 = _vout._posW;
    return cout;
} // main end
