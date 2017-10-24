#pragma once

#include <ai.h>

#define UV_INVALID -1000000.0f
#define UV_GLOBALS -2000000.0f

#ifndef _WIN32
#ifndef LONG_MAX
#define LONG_MAX __LONG_MAX__
#endif
#endif

bool isnan(float a);
bool isinf(float a);

float Luminance(float v);
float Luminance(const AtRGB &color);
float Luminance(const AtRGBA &color);

float Mix(float f0, float f1, float t);
AtRGB Mix(const AtRGB &c0, const AtRGB &c1, float t);
AtRGBA Mix(const AtRGBA &c0, const AtRGBA &c1, float t);

float MapValue(float v, float vmin, float vmax);
float UnmapValue(float v, float vmin, float vmax);

bool IsValidUV(AtPoint2 &uv);
float Integral(float t, float nedge);
float Mod(float n, float d);

bool SortFloatIndexArray(AtArray *a, unsigned int *shuffle);

enum InterpolationType
{
   IT_NONE = 0,
   IT_LINEAR,
   IT_SMOOTH,
   IT_SPLINE
};

extern const char* InterpolationNames[];

InterpolationType InterpolationNameToType(const char *n);

void InterpolateShuffle(AtArray *p, AtArray *v, AtArray *it, float t, float &out, unsigned int *shuffle);
void InterpolateShuffle(AtArray *p, AtArray *v, AtArray *it, float t, AtRGB &out, unsigned int *shuffle);

enum RampInterpolationType
{
   RIT_NONE = 0,
   RIT_LINEAR,
   RIT_EXP_UP,
   RIT_EXP_DOWN,
   RIT_SMOOTH,
   RIT_BUMP,
   RIT_SPIKE
};

extern const char* RampInterpolationNames[];

RampInterpolationType RampInterpolationNameToType(const char *n);
void Ramp(AtArray *p, AtArray *v, float t, RampInterpolationType rit, float &out, unsigned int *shuffle);
void Ramp(AtArray *p, AtArray *v, float t, RampInterpolationType rit, AtRGB &out, unsigned int *shuffle);

#ifndef LOG_05
#  define LOG_05 -0.693147180559945  /* log(0.5) */
#endif


#define MAYA_COLOR_BALANCE_ENUM \
   p_defaultColor,              \
   p_colorGain,                 \
   p_colorOffset,               \
   p_alphaGain,                 \
   p_alphaOffset,               \
   p_alphaIsLuminance,          \
   p_invert,                    \
   p_exposure

void AddMayaColorBalanceParams(AtList *params, AtMetaDataStore* mds);
   
void MayaColorBalance(AtShaderGlobals* sg,
                             AtNode* node,
                             int p_start,
                             AtRGBA & result);

void MayaDefaultColor(AtShaderGlobals* sg,
                             AtNode* node,
                             int p_start,
                             AtRGBA & result);

AtVector RGBtoHSV(AtRGB inRgb);
AtVector RGBtoHSL(AtRGB inRgb);
AtRGB HSVtoRGB(AtVector inHsv);

float FilteredPulseTrain(float edge, float period, float x, float dx);

float SmoothStep(float e0, float e1, float x);

float Bias(float b, float x);

float fBm(AtShaderGlobals *sg, const AtPoint &p, float time, float initialAmplitude,
                 int octaves[2], float initialLacunarity, float frequencyRatio, float ratio);

float fTurbulence(AtShaderGlobals *sg, const AtPoint &point, float time, float lacunarity,
                         float frequencyRatio, int octaves[2], float ratio, float ripples[3]);

AtPoint AnimatedCellNoise(const AtPoint &p, float tt);

int SuspendedParticles(const AtPoint &Pn, float time, float particleRadius, float jitter, float octave,
                              float &f1, AtPoint &pos1, float &f2, AtPoint &pos2, AtPoint (&particlePos)[27]);

int SuspendedParticles2d(const AtPoint &Pn, float time, float particleRadius, float jitter, float octave, AtPoint (&particlePos)[27]);

float ParticleDensity(int falloff, const AtPoint &particleCenter, const AtPoint &P, float radius);

float BillowNoise(const AtPoint &p, float time, int dim, float radius, float sizeRand, float jitter,
                         int falloff, float spottyness, int octaves, float frequencyRatio, float ratio, float amplitude);

float CosWaves(float posX, float posY, float posTime, int numWaves);

// used by AOV nodes
bool IsInShadingGroup(AtArray* set_ids, AtShaderGlobals* sg);
AtArray* StringArrayToNodeArray(AtArray* setNames);

bool SetRefererencePoints(AtShaderGlobals *sg, AtPoint &tmpPts);
bool SetRefererenceNormals(AtShaderGlobals *sg, AtVector &tmpNmrs);
void RestorePoints(AtShaderGlobals *sg, AtPoint tmpPts);
void RestoreNormals(AtShaderGlobals *sg, AtVector tmpNmrs);

// some random tools from Borja
float sfrand(unsigned int *seed);
float rand01(unsigned int *seed);
