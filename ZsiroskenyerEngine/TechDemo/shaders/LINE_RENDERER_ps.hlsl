// ps_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile ps_5_0
// source file: shaders/LINE_RENDERER.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile ps_5_0
//program PS_MAIN
//semantic viewProjMat
//semantic color
//var float3 color :  : _color : -1 : 1
//var float4 In.posH : $vin.SV_POSITION :  : 0 : 0
//var float4 PS_MAIN.color : $vout.COLOR : COLOR : -1 : 1

#pragma pack_matrix(row_major)

struct VS_IN {
    float3 _posL : SV_Position;
};

struct VS_OUT {
    float4 _posH : SV_POSITION;
};

struct PS_OUT {
    float4 _color1 : SV_Target;
};

struct X1X {
    float4 _SV_POSITION : SV_POSITION;
};

float3 _color;

 // main procedure, the original name was PS_MAIN
PS_OUT main()
{

    PS_OUT _pout;

    _pout._color1 = float4(_color.x, _color.y, _color.z,  1.00000000000000000E000f);
    return _pout;
} // main end
