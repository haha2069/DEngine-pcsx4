"\n"
"// This function for analytical evaluation of particle density integral is \n"
"// provided by Eric Bruneton\n"
"// http://www-evasion.inrialpes.fr/Membres/Eric.Bruneton/\n"
"//\n"
"// optical depth for ray (r,mu) of length d, using analytic formula\n"
"// (mu=cos(view zenith angle)), intersections with ground ignored\n"
"float2 GetDensityIntegralAnalytic(float r, float mu, float d, float EarthRadius, float4 ParticleScaleHeight) \n"
"{\n"
"    float2 f2A = sqrt( (float2(0.5, 0.5) * ParticleScaleHeight.zw) * r );\n"
"    float4 f4A01 = f2A.xxyy * float2(mu, mu + d / r).xyxy;\n"
"    float4 f4A01s = sign(f4A01);\n"
"    float4 f4A01sq = f4A01*f4A01;\n"
"    \n"
"    float2 f2X;\n"
"    f2X.x = f4A01s.y > f4A01s.x ? exp(f4A01sq.x) : 0.0;\n"
"    f2X.y = f4A01s.w > f4A01s.z ? exp(f4A01sq.z) : 0.0;\n"
"    \n"
"    float4 f4Y = f4A01s / (2.3193*abs(f4A01) + sqrt(1.52*f4A01sq + 4.0)) * float3(1.0, exp(-float2(d,d)*ParticleScaleHeight.zw*(d/(2.0*r)+mu))).xyxz;\n"
"\n"
"    return sqrt((6.2831*ParticleScaleHeight.xy)*r) * exp((EarthRadius-r) * ParticleScaleHeight.zw) * (f2X + float2( dot(f4Y.xy, float2(1.0, -1.0)), dot(f4Y.zw, float2(1.0, -1.0)) ));\n"
"}\n"
"\n"
"\n"
"float3 GetExtinctionUnverified(float3 f3StartPos,\n"
"                               float3 f3EndPos,\n"
"                               float3 f3EyeDir,\n"
"                               float3 f3EarthCentre,\n"
"                               float  fEarthRadius,\n"
"                               float4 f4ParticleScaleHeight)\n"
"{\n"
"#if 0\n"
"    float2 f2ParticleDensity = IntegrateParticleDensity(f3StartPos, f3EndPos, f3EarthCentre, 20);\n"
"#else\n"
"    float r = length(f3StartPos-f3EarthCentre);\n"
"    float fCosZenithAngle = dot(f3StartPos-f3EarthCentre, f3EyeDir) / r;\n"
"    float2 f2ParticleDensity = GetDensityIntegralAnalytic(r, fCosZenithAngle, length(f3StartPos - f3EndPos), fEarthRadius, f4ParticleScaleHeight);\n"
"#endif\n"
"\n"
"    // Get optical depth\n"
"    float3 f3TotalRlghOpticalDepth = g_MediaParams.f4RayleighExtinctionCoeff.rgb * f2ParticleDensity.x;\n"
"    float3 f3TotalMieOpticalDepth  = g_MediaParams.f4MieExtinctionCoeff.rgb      * f2ParticleDensity.y;\n"
"        \n"
"    // Compute extinction\n"
"    float3 f3Extinction = exp( -(f3TotalRlghOpticalDepth + f3TotalMieOpticalDepth) );\n"
"    return f3Extinction;\n"
"}\n"
"\n"
"float3 GetExtinction(float3 f3StartPos,\n"
"                     float3 f3EndPos,\n"
"                     float3 f3EarthCentre,\n"
"                     float  fEarthRadius,\n"
"                     float  fAtmTopRadius,\n"
"                     float4 f4ParticleScaleHeight)\n"
"{\n"
"    float3 f3EyeDir = f3EndPos - f3StartPos;\n"
"    float fRayLength = length(f3EyeDir);\n"
"    f3EyeDir /= fRayLength;\n"
"\n"
"    float2 f2RayAtmTopIsecs = float2(0.0, 0.0); \n"
"    // Compute intersections of the view ray with the atmosphere\n"
"    GetRaySphereIntersection(f3StartPos, f3EyeDir, f3EarthCentre, fAtmTopRadius, f2RayAtmTopIsecs);\n"
"    // If the ray misses the atmosphere, there is no extinction\n"
"    if( f2RayAtmTopIsecs.y < 0.0 )return float3(1.0, 1.0, 1.0);\n"
"\n"
"    // Do not let the start and end point be outside the atmosphere\n"
"    f3EndPos = f3StartPos + f3EyeDir * min(f2RayAtmTopIsecs.y, fRayLength);\n"
"    f3StartPos += f3EyeDir * max(f2RayAtmTopIsecs.x, 0.0);\n"
"\n"
"    return GetExtinctionUnverified(f3StartPos, f3EndPos, f3EyeDir, f3EarthCentre, fEarthRadius, f4ParticleScaleHeight);\n"
"}\n"
