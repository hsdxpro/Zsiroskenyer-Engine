// vs_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile vs_5_0
// source file: shaders/LINE_RENDERER.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile vs_5_0
//program VS_MAIN
//semantic viewProjMat
//semantic color
//var float4x4 viewProjMat :  : _viewProjMat[0], 4 : -1 : 1
//var float3 In.posL : $vin.POSITION : POSITION : 0 : 1
//var float4 VS_MAIN.posH : $vout.SV_POSITION :  : -1 : 1

#pragma pack_matrix(row_major)

struct VS_IN {
    float3 _posL : POSITION;
};

struct VS_OUT {
    float4 _posH : SV_POSITION;
};

struct PS_OUT {
    float4 _color1 : COLOR0;
};

struct X1X {
    float4 _SV_POSITION : SV_POSITION;
};

static X1X cout;
float4x4 _viewProjMat;

 // main procedure, the original name was VS_MAIN
X1X main(in VS_IN _In)
{

    VS_OUT _vout;

    _vout._posH = _In._posL.x*_viewProjMat._11_12_13_14 + _In._posL.y*_viewProjMat._21_22_23_24 + _In._posL.z*_viewProjMat._31_32_33_34 + _viewProjMat._41_42_43_44;
    cout._SV_POSITION = _vout._posH;
    return cout;
} // main end
