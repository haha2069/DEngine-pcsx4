"\n"
"#include \"FullScreenTriangleVSOutput.fxh\"\n"
"#include \"BasicStructures.fxh\"\n"
"#include \"Shadows.fxh\"\n"
"\n"
"struct ConversionAttribs\n"
"{\n"
"    int iCascade;\n"
"    float fHorzFilterRadius;\n"
"    float fVertFilterRadius;\n"
"    float fEVSMPositiveExponent;\n"
"\n"
"    float fEVSMNegativeExponent;\n"
"    bool Is32BitEVSM;\n"
"};\n"
"\n"
"cbuffer cbConversionAttribs\n"
"{\n"
"    ConversionAttribs g_Attribs;\n"
"}\n"
"\n"
"Texture2DArray g_tex2DShadowMap;\n"
"\n"
"float GetSampleWeight(int x, float FilterRadius)\n"
"{\n"
"    float fTexelMin = max(float(x),       min(0.5 - FilterRadius, 0.0));\n"
"    float fTexelMax = min(float(x) + 1.0, max(0.5 + FilterRadius, 1.0));\n"
"    return fTexelMax - fTexelMin;\n"
"}\n"
"\n"
"float4 VSMHorzPS(FullScreenTriangleVSOutput VSOut) : SV_Target\n"
"{\n"
"    float2 f2Moments = float2(0.0, 0.0);\n"
"    int range = int(floor(g_Attribs.fHorzFilterRadius + 0.5));\n"
"    float fTotalWeight = 0.0;\n"
"    for (int i = -range; i <= +range; ++i)\n"
"    {\n"
"        float fWeight = GetSampleWeight(i, g_Attribs.fHorzFilterRadius);\n"
"        float fDepth = g_tex2DShadowMap.Load( int4( int(VSOut.f4PixelPos.x) + i, int(VSOut.f4PixelPos.y), g_Attribs.iCascade, 0) ).r;\n"
"        f2Moments += float2(fDepth, fDepth*fDepth) * fWeight;\n"
"        fTotalWeight += fWeight;\n"
"    }\n"
"    return float4(f2Moments / fTotalWeight, 0.0, 0.0);\n"
"}\n"
"\n"
"float4 EVSMHorzPS(FullScreenTriangleVSOutput VSOut) : SV_Target\n"
"{\n"
"    float2 f2Exponents = GetEVSMExponents(g_Attribs.fEVSMPositiveExponent, g_Attribs.fEVSMNegativeExponent, g_Attribs.Is32BitEVSM);\n"
"\n"
"    float4 f4Moments = float4(0.0, 0.0, 0.0, 0.0);\n"
"    int range = int(floor(g_Attribs.fHorzFilterRadius + 0.5));\n"
"    float fTotalWeight = 0.0;\n"
"    for (int i = -range; i <= +range; ++i)\n"
"    {\n"
"        float fWeight = GetSampleWeight(i, g_Attribs.fHorzFilterRadius);\n"
"        float fDepth = g_tex2DShadowMap.Load( int4( int(VSOut.f4PixelPos.x) + i, int(VSOut.f4PixelPos.y), g_Attribs.iCascade, 0) ).r;\n"
"        float2 f2EVSMDepth = WarpDepthEVSM(fDepth, f2Exponents);\n"
"        f4Moments += float4(f2EVSMDepth.x, f2EVSMDepth.x*f2EVSMDepth.x, f2EVSMDepth.y, f2EVSMDepth.y*f2EVSMDepth.y) * fWeight;\n"
"        fTotalWeight += fWeight;\n"
"    }\n"
"    return f4Moments / fTotalWeight;\n"
"}\n"
"\n"
"float4 VertBlurPS(FullScreenTriangleVSOutput VSOut) : SV_Target\n"
"{\n"
"    float4 f4Moments = float4(0.0, 0.0, 0.0, 0.0);\n"
"    int range = int(floor(g_Attribs.fVertFilterRadius + 0.5));\n"
"    float fTotalWeight = 0.0;\n"
"    for (int i = -range; i <= +range; ++i)\n"
"    {\n"
"        float fWeight = GetSampleWeight(i, g_Attribs.fVertFilterRadius);\n"
"        f4Moments += g_tex2DShadowMap.Load( int4( int(VSOut.f4PixelPos.x), int(VSOut.f4PixelPos.y) + i, 0, 0) ) * fWeight;\n"
"        fTotalWeight += fWeight;\n"
"    }\n"
"    return f4Moments / fTotalWeight;\n"
"}\n"
