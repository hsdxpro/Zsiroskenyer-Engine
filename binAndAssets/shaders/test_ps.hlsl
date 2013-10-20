// ps_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile ps_5_0
// source file: D:\ZsiroskenyerTeam\Zsiroskenyer-Engine\ZsiroskenyerEngine\Debug_\shaders/test.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile ps_5_0
//program PS_MAIN
//semantic wvp
//semantic diffuseTex
//var float4 In.posH : $vin.SV_POSITION :  : 0 : 0
//var float3 In.normalW : $vin.TEXCOORD0 :  : 0 : 1
//var float2 In.tex0 : $vin.TEXCOORd1 :  : 0 : 0
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
};

struct PS_OUT {
    float4 _color : SV_Target;
};

struct X1X {
    float4 _SV_POSITION : SV_POSITION;
    float3 _TEXCOORD0 : TEXCOORD0;
    float2 _TEXCOORd1 : TEXCOORD1;
};


 // main procedure, the original name was PS_MAIN
PS_OUT main( in X1X cin)
{

    VS_OUT _In;
    PS_OUT _pout;

    _In._normalW = cin._TEXCOORD0;
    _pout._color = float4(_In._normalW.x, _In._normalW.y, _In._normalW.z,  1.00000000000000000E000f);
    return _pout;
} // main end
