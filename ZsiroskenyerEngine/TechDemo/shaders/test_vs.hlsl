// vs_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile vs_5_0
// source file: shaders/test.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile vs_5_0
//program VS_MAIN
//semantic wvp : C0
//semantic world : C4
//semantic camPos : C8
//semantic diffuseTex : TEXUNIT0
//semantic normalTex : TEXUNIT1
//var float4x4 wvp : C0 : _wvp[0], 4 : -1 : 1
//var float4x4 world : C4 : _world[0], 4 : -1 : 1
//var float3 In.posL : $vin.POSITION : POSITION : 0 : 1
//var float3 In.normalL : $vin.NORMAL : NORMAL : 0 : 1
//var float3 In.tangentL : $vin.TANGENT : TANGENT : 0 : 1
//var float2 In.tex0 : $vin.TEXCOORD0 : TEXCOORD0 : 0 : 1
//var float4 VS_MAIN.posH : $vout.SV_POSITION :  : -1 : 1
//var float3 VS_MAIN.normalW : $vout.TEXCOORD0 :  : -1 : 1
//var float3 VS_MAIN.tangentW : $vout.TEXCOORD1 :  : -1 : 1
//var float2 VS_MAIN.tex0 : $vout.TEXCOORD2 :  : -1 : 1
//var float3 VS_MAIN.posW : $vout.TEXCOORD3 :  : -1 : 1
//var float3x3 VS_MAIN.TBN : $vout.TEXCOORD4 : , 3 : -1 : 1

#pragma pack_matrix(row_major)

struct VS_IN {
    float3 _posL : POSITION;
    float3 _normalL : NORMAL0;
    float3 _tangentL : TANGENT0;
    float2 _tex0 : TEXCOORD0;
};

struct VS_OUT {
    float4 _posH : SV_POSITION;
    float3 _normalW : TEXCOORD0;
    float3 _tangentW : TEXCOORD1;
    float2 _tex01 : TEXCOORD2;
    float3 _posW : TEXCOORD3;
    float3x3 _TBN : TEXCOORD4;
};

struct PS_OUT {
    float4 _color : COLOR0;
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

static float _TMP0;
static float _TMP2;
static float _TMP1;
static float3 _v0005;
static X1X cout;
float4x4 _wvp : register(c0);
float4x4 _world : register(c4);

 // main procedure, the original name was VS_MAIN
X1X main(in VS_IN _In)
{

    VS_OUT _vout;
    float3 _T;
    float3 _B;
    float3x3 _TMP3;

    _vout._tex01 = _In._tex0;
    _vout._posW = _In._posL.x*_world._11_12_13 + _In._posL.y*_world._21_22_23 + _In._posL.z*_world._31_32_33 + _world._41_42_43;
    _vout._normalW = _In._normalL.x*_world._11_12_13 + _In._normalL.y*_world._21_22_23 + _In._normalL.z*_world._31_32_33;
    _vout._tangentW = _In._tangentL.x*_world._11_12_13 + _In._tangentL.y*_world._21_22_23 + _In._tangentL.z*_world._31_32_33;
    _vout._posH = _In._posL.x*_wvp._11_12_13_14 + _In._posL.y*_wvp._21_22_23_24 + _In._posL.z*_wvp._31_32_33_34 + _wvp._41_42_43_44;
    _TMP0 = dot(_vout._tangentW, _vout._normalW);
    _v0005 = _vout._tangentW - _TMP0*_vout._normalW;
    _TMP1 = dot(_v0005, _v0005);
    _TMP2 = rsqrt(_TMP1);
    _T = _TMP2*_v0005;
    _B = _vout._normalW.yzx*_T.zxy - _vout._normalW.zxy*_T.yzx;
    _TMP3._11_12_13 = _T;
    _TMP3._21_22_23 = _B;
    _TMP3._31_32_33 = _vout._normalW;
    _vout._TBN[0] = _TMP3[0];
    _vout._TBN[1] = _TMP3[1];
    _vout._TBN[2] = _TMP3[2];
    cout._SV_POSITION = _vout._posH;
    cout._TEXCOORD0 = _vout._normalW;
    cout._TEXCOORD1 = _vout._tangentW;
    cout._TEXCOORD2 = _vout._tex01;
    cout._TEXCOORD3 = _vout._posW;
    cout._TEXCOORD4 = _vout._TBN[0];
    cout._TEXCOORD5 = _vout._TBN[1];
    cout._TEXCOORD6 = _vout._TBN[2];
    return cout;
} // main end
