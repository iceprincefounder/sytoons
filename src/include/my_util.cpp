#include "MayaUtils.h"
#include <string>
#include <vector>

namespace
{

float _GetArrayFlt(AtArray *a, unsigned int i)
{
   return AiArrayGetFlt(a, i);
}

AtRGB _GetArrayRGB(AtArray *a, unsigned int i)
{
   return AiArrayGetRGB(a, i);
}

// This one is defined for the RampT template function to work properly
float RampLuminance(float v)
{
   return v;
}

float RampLuminance(const AtRGB &color)
{
   return (0.3f * color.r + 0.3f * color.g + 0.3f * color.b);
}

/*float RampLuminance(const AtRGBA &color)
{
   return (0.3f * color.r + 0.3f * color.g + 0.3f * color.b);
}*/

template <typename ValType>
void RampT(AtArray *p, AtArray *c, float t, RampInterpolationType it, ValType &result, ValType (*getv)(AtArray*, unsigned int), unsigned int *shuffle)
{
   unsigned int inext = p->nelements;

   for (unsigned int i = 0; (i < p->nelements); ++i)
   {
      if (t < AiArrayGetFlt(p, shuffle[i]))
      {
         inext = i;
         break;
      }
   }

   if (inext >= p->nelements)
   {
      result = getv(c, shuffle[p->nelements - 1]);
      return;
   }

   if (inext == 0)
   {
      result = getv(c, shuffle[0]);
      return;
   }

   unsigned int icur = inext - 1;
   float tcur = AiArrayGetFlt(p, shuffle[icur]);
   float tnext = AiArrayGetFlt(p, shuffle[inext]);
   ValType ccur = getv(c, shuffle[icur]);
   ValType cnext = getv(c, shuffle[inext]);
   float u = (t - tcur) / (tnext - tcur);

   switch (it)
   {
   case RIT_LINEAR:
      // u = u;
      break;
   case RIT_EXP_UP:
      u = u * u;
      break;
   case RIT_EXP_DOWN:
      u = 1.0f - (1.0f - u) * (1.0f - u);
      break;
   case RIT_SMOOTH:
      u = 0.5f * (static_cast<float>(cos((u + 1.0f) * AI_PI)) + 1.0f);
      break;
   case RIT_BUMP:
      {
         float lcur = RampLuminance(ccur);
         float lnext = RampLuminance(cnext);
         if (lcur < lnext)
         {
            u = sin(u * static_cast<float>(AI_PI) / 2.0f);
         }
         else
         {
            u = sin((u - 1.0f) * static_cast<float>(AI_PI) / 2.0f) + 1.0f;
         }
      }
      break;
   case RIT_SPIKE:
      {
         float lcur = RampLuminance(ccur);
         float lnext = RampLuminance(cnext);
         if (lcur > lnext)
         {
            u = sin(u * static_cast<float>(AI_PI) / 2.0f);
         }
         else
         {
            u = sin((u - 1.0f) * static_cast<float>(AI_PI) / 2.0f) + 1.0f;
         }
      }
      break;
   case RIT_NONE:
   default:
      u = 0.0f;
   }

   result = Mix(ccur, cnext, u);
}

};

bool isnan(float a)
{
   return a != a;
}

bool isinf(float a)
{
   return !isnan(a) && isnan(a-a);
}

// This one is defined for the RampT template function to work properly
float Luminosity(float v)
{
   return v;
}

float Luminosity(const AtRGB &color)
{
   return RGBtoHSL(color).z;
}

float Luminosity(const AtRGBA &color)
{
   return RGBtoHSL(AiRGBAtoRGB(color)).z;
}

// This one is defined for the RampT template function to work properly
float Luminance(float v)
{
   return v;
}

float Luminance(const AtRGB &color)
{
   return (0.3f * color.r + 0.59f * color.g + 0.11f * color.b);
}

float Luminance(const AtRGBA &color)
{
   return (0.3f * color.r + 0.59f * color.g + 0.11f * color.b);
}

float Mix(float a, float b, float t)
{
   return (a + t * (b - a));
}

AtRGB Mix(const AtRGB &c0, const AtRGB &c1, float t)
{
   return (c0 + t * (c1 - c0));
}

AtRGBA Mix(const AtRGBA &c0, const AtRGBA &c1, float t)
{
   AtRGBA rv;
   rv.r = c0.r + t * (c1.r - c0.r);
   rv.g = c0.g + t * (c1.g - c0.g);
   rv.b = c0.b + t * (c1.b - c0.b);
   rv.a = c0.a + t * (c1.a - c0.a);
   return rv;
}

float MapValue(float v, float vmin, float vmax)
{
   return ((v - vmin) / (vmax - vmin));
}

float UnmapValue(float v, float vmin, float vmax)
{
   return (vmin + (vmax - vmin) * v);
}

bool IsValidUV(AtPoint2 &uv)
{
   // place2dTexture return (UV_INVALID, UV_INVALID) for invalid UVs
   return (uv.x > UV_INVALID && uv.y > UV_INVALID);
}

float Integral(float t, float nedge)
{
   return ((1.0f - nedge) * FLOOR(t) + MAX(0.0f, t - FLOOR(t) - nedge));
}

float Mod(float n, float d)
{
   return (n - (floor(n / d) * d));
}

bool SortFloatIndexArray(AtArray *a, unsigned int *shuffle)
{
   bool modified = false;

   if (a && shuffle && a->nelements > 0)
   {
      float p0, p1;
      int tmp;

      bool swapped = true;
      AtUInt32 n = a->nelements;

      for (AtUInt32 i = 0; (i < n); ++i)
      {
         shuffle[i] = i;
      }

      while (swapped)
      {
         swapped = false;
         n -= 1;
         for (AtUInt32 i = 0; (i < n); ++i)
         {
            p0 = AiArrayGetFlt(a, shuffle[i]);
            p1 = AiArrayGetFlt(a, shuffle[i + 1]);
            if (p0 > p1)
            {
               swapped = true;
               modified = true;

               tmp = shuffle[i];
               shuffle[i] = shuffle[i + 1];
               shuffle[i + 1] = tmp;
            }
         }
      }
   }

   return modified;
}

const char* InterpolationNames[] =
{
   "none",
   "linear",
   "smooth",
   "spline",
   NULL
};

InterpolationType InterpolationNameToType(const char *n)
{
   return (InterpolationType) AiEnumGetValue(InterpolationNames, n);
}

template <typename ValType>
void InterpolateShuffleT(AtArray *p, AtArray *v, AtArray *it, float t, ValType &result, ValType (*getv)(AtArray*, unsigned int), unsigned int *shuffle)
{
   unsigned int inext = p->nelements;

   for (unsigned int i = 0; (i < p->nelements); ++i)
   {
      if (t < AiArrayGetFlt(p, shuffle[i]))
      {
         inext = i;
         break;
      }
   }

   if (inext >= p->nelements)
   {
      result = getv(v, shuffle[p->nelements - 1]);
      return;
   }

   int iprev = inext - 1;
   
   if (iprev < 0)
   {
      result = getv(v, shuffle[0]);
      return;
   }

   float p1 = AiArrayGetFlt(p, shuffle[iprev]);
   float p2 = AiArrayGetFlt(p, shuffle[inext]);

   ValType v1 = getv(v, shuffle[iprev]);
   ValType v2 = getv(v, shuffle[inext]);

   float dp = p2 - p1;
   float u = (t - p1) / dp;

   int itype = AiArrayGetInt(it, shuffle[iprev]);

   switch (itype)
   {
   case IT_NONE:
      result = v1;
      break;

   case IT_LINEAR:
      result = v1 + u * (v2 - v1);
      break;

   case IT_SMOOTH:
      {
         float u2 = u * u;
         float u3 = u * u2;
         float a =  2.0f * u3 - 3.0f * u2 + 1.0f;
         float b = -2.0f * u3 + 3.0f * u2 ;
         result = a * v1 + b * v2;
      }
      break;

   case IT_SPLINE:
      {
         ValType dv = v2 - v1;
         
         // Calculate previous to previous position and value
         float p0;
         ValType y0;
         if (iprev == 0)
         {
            p0 = p1 - dp;
            y0 = v1;
         }
         else
         {
            p0 = AiArrayGetFlt(p, shuffle[iprev-1]);
            y0 = getv(v, shuffle[iprev-1]);
         }
         
         // Calculate next to next position and value
         float p3;
         ValType v3;
         if (inext == v->nelements - 1)
         {
            p3 = p2 + dp;
            v3 = v2;
         }
         else
         {
            p3 = AiArrayGetFlt(p, shuffle[inext+1]);
            v3 = getv(v, shuffle[inext+1]);
         }
         
         // Compute tangents
         float tanSize = 0.2f;
         float tx = tanSize * dp;
         if( tx < AI_EPSILON )
         {
            tx = AI_EPSILON;
         }
         
         // Compute start tangent
         float sx = p2-p0;	
         ValType sy = v2-y0;
         if( sx < AI_EPSILON )
         {
            sx = AI_EPSILON;
         }
         sy *= tanSize * dp/sx;
         ValType m1 = sy / tx;
         
         // Compute end tangent
         sx = p3-p1;	
         sy = v3-v1;	
         if( sx < AI_EPSILON )
         {
            sx = AI_EPSILON;
         }
         sy *= tanSize * dp/sx;
         ValType m2 = sy / tx;
         
         float tFromP1 = (t - p1);
         
         float length = 1.0f / (dp * dp);
         ValType d1 = dp * m1;
         ValType d2 = dp * m2;
         
         ValType c0 = (d1 + d2 - 2*dv) * length / dp;
         ValType c1 = (dv + dv + dv - 2*d1 - d2) * length;
         result = tFromP1 * (tFromP1 * (tFromP1 * c0 + c1) + m1) + v1;
      }
      break;

   default:
      result = v1;
   }
}

const char* RampInterpolationNames[] =
{
   "none",
   "linear",
   "exponentialup",
   "exponentialdown",
   "smooth",
   "bump",
   "spike",
   NULL
};

RampInterpolationType RampInterpolationNameToType(const char *n)
{
   return (RampInterpolationType) AiEnumGetValue(RampInterpolationNames, n);
}

void InterpolateShuffle(AtArray *p, AtArray *v, AtArray *it, float t, float &out, unsigned int *shuffle)
{
   InterpolateShuffleT(p, v, it, t, out, _GetArrayFlt, shuffle);
}

void InterpolateShuffle(AtArray *p, AtArray *v, AtArray *it, float t, AtRGB &out, unsigned int *shuffle)
{
   InterpolateShuffleT(p, v, it, t, out, _GetArrayRGB, shuffle);
}

void Ramp(AtArray *p, AtArray *v, float t, RampInterpolationType it, float &out, unsigned int *shuffle)
{
   RampT(p, v, t, it, out, _GetArrayFlt, shuffle);
}

void Ramp(AtArray *p, AtArray *v, float t, RampInterpolationType it, AtRGB &out, unsigned int *shuffle)
{
   RampT(p, v, t, it, out, _GetArrayRGB, shuffle);
}


void AddMayaColorBalanceParams(AtList *params, AtMetaDataStore* mds)
{
   AiParameterRGB ("defaultColor", 0.5f, 0.5f, 0.5f);
   AiParameterRGB ("colorGain", 1.0f, 1.0f, 1.0f);
   AiParameterRGB ("colorOffset", 0.0f, 0.0f, 0.0f);
   AiParameterFLT ("alphaGain", 1.0f);
   AiParameterFLT ("alphaOffset", 0.0f);
   AiParameterBOOL("alphaIsLuminance", false);
   AiParameterBOOL("invert", false);
   AiParameterFLT ("exposure", 0.0f);

   AiMetaDataSetBool(mds, "colorGain", "always_linear", true);
   AiMetaDataSetBool(mds, "colorOffset", "always_linear", true);
}

void MayaColorBalance(AtShaderGlobals* sg,
                        AtNode* node,
                        int p_start,
                        AtRGBA & result)
{
   const AtRGB colorGain      = AiShaderEvalParamFuncRGB(sg, node, p_start + 1);  // p_colorGain);
   const AtRGB colorOffset    = AiShaderEvalParamFuncRGB(sg, node, p_start + 2);  // p_colorOffset);
   const float alphaGain      = AiShaderEvalParamFuncFlt(sg, node, p_start + 3);  // p_alphaGain);
   const float alphaOffset    = AiShaderEvalParamFuncFlt(sg, node, p_start + 4);  // p_alphaOffset);
   const bool alphaIsLuminance     = AiShaderEvalParamFuncBool(sg, node, p_start + 5);  // alphaIsLuminance);
   const bool invert = AiShaderEvalParamFuncBool(sg, node, p_start + 6); // p_invert);
   const float exposure = powf(2.0f, AiShaderEvalParamFuncFlt(sg, node, p_start + 7)); // p_exposure

   if (invert)
   {
      result.r = 1.f - result.r;
      result.g = 1.f - result.g;
      result.b = 1.f - result.b;
      result.a = 1.f - result.a;
   }

   if (alphaIsLuminance)
   {
      result.a = Luminance(result);
   }

   result.r = result.r * colorGain.r * exposure + colorOffset.r;
   result.g = result.g * colorGain.g * exposure + colorOffset.g;
   result.b = result.b * colorGain.b * exposure + colorOffset.b;
   result.a = result.a * alphaGain   + alphaOffset;
}

void MayaDefaultColor(AtShaderGlobals* sg,
                        AtNode* node,
                        int p_start,
                        AtRGBA & result)
{
   const AtRGB defaultColor   = AiShaderEvalParamFuncRGB(sg, node, p_start + 0);  //p_defaultColor);
   AiRGBtoRGBA(defaultColor, result);
   result.a = 0.0f;
}

AtVector RGBtoHSV(AtRGB inRgb)
{
   AtVector output(AI_V3_ZERO);

   const float min = MIN3(inRgb.r, inRgb.g, inRgb.b);
   const float max = MAX3(inRgb.r, inRgb.g, inRgb.b);

   if (max > min)
   {
      if (max == inRgb.r)
      {
        output.x = (inRgb.g - inRgb.b) / (max - min);
        if (output.x < 0)
           output.x += 6;
      }
      else if (max == inRgb.g)
        output.x = (inRgb.b - inRgb.r) / (max - min) + 2;
      else
        output.x = (inRgb.r - inRgb.g) / (max - min) + 4;

      output.x *= 60;
   }

   if (max >= AI_EPSILON)
      output.y = (max - min) / max;

   output.z = max;

   return output;
}

AtVector RGBtoHSL(AtRGB inRgb)
{
   AtVector output(AI_V3_ZERO);

   const float min = MIN3(inRgb.r, inRgb.g, inRgb.b);
   const float max = MAX3(inRgb.r, inRgb.g, inRgb.b);

   // L
   output.z = 0.5f * (min + max);

   // S
   const float d = max - min;

   if (d != 0)
   {
      output.y = d / (output.z <= 0.5f ? (max + min) : (2.0f - max - min));

      // H
      if (max == inRgb.r)
      {
         output.x = (inRgb.g - inRgb.b) / d;
      }
      else if (max == inRgb.g)
      {
         output.x = 2.0f + (inRgb.b - inRgb.r) / d;
      }
      else
      {
         output.x = 4.0f + (inRgb.r - inRgb.g) / d;
      }

      output.x /= 6.0f;
   }

   return output;
}

AtRGB HSVtoRGB(AtVector inHsv)
{
   AtRGB output;

   if (inHsv.y < AI_EPSILON)
   {
      output.r = inHsv.z;
      output.g = inHsv.z;
      output.b = inHsv.z;
   }
   else
   {
      float f = inHsv.x / 60.0f;
      if(f >= 6.0f)
         f = 0.0f;

      int Hi = int(f) % 6;

      f -= Hi;

      float p = inHsv.z * (1 - inHsv.y);
      float q = inHsv.z * (1 - f * inHsv.y);
      float t = inHsv.z * (1 - (1 - f) * inHsv.y);

      switch (Hi)
      {
      case 0:
         output.r = inHsv.z;
         output.g = t;
         output.b = p;
         break;
      case 1:
         output.r = q;
         output.g = inHsv.z;
         output.b = p;
         break;
      case 2:
         output.r = p;
         output.g = inHsv.z;
         output.b = t;
         break;
      case 3:
         output.r = p;
         output.g = q;
         output.b = inHsv.z;
         break;
      case 4:
         output.r = t;
         output.g = p;
         output.b = inHsv.z;
         break;
      case 5:
      default:
         output.r = inHsv.z;
         output.g = p;
         output.b = q;
         break;
      }
   }

   return output;
}

// Taken from Advanced RenderMan
float FilteredPulseTrain(float edge, float period, float x, float dx)
{
   float invPeriod = 1.0f / period;

   float w = dx * invPeriod;
   float x0 = x * invPeriod - 0.5f * w;
   float x1 = x0 + w;
   float nedge = edge * invPeriod;

   float result;

   if (x0 == x1)
   {
     result = (x0 - FLOOR(x0) < nedge) ? 0.0f : 1.0f;
   }
   else
   {
      result = (Integral(x1, nedge) - Integral(x0, nedge)) / w;
   }

   return result;
}

float SmoothStep(float e0, float e1, float x)
{
   float t = (x - e0) / (e1 - e0);
   t = CLAMP(t, 0.0f, 1.0f);
   return t * t * (3.0f - 2.0f * t);
}

float Bias(float b, float x)
{
   return pow(x, log(b) / float(LOG_05));
}

float fBm(AtShaderGlobals *sg,
          const AtPoint &p,
          float time,
          float initialAmplitude,
          int octaves[2],
          float initialLacunarity,
          float frequencyRatio,
          float ratio)
{
   float amp = initialAmplitude;
   AtPoint pp = p;
   float sum = 0;
   int i = 0;
   float lacunarity = initialLacunarity;

   // NOTE: this is wrong, sg->area is "world-space" area
   // sg->area is 0.0 for volumetric shaders
   float pixelSize = float(AI_EPSILON);
   float nyquist = 2.0f * pixelSize;
   float pixel = 1.0f;

   while ((i < octaves[1] && pixel > nyquist) || i < octaves[0])
   {
      sum += amp * AiPerlin4(pp * lacunarity, time);
      amp *= ratio;
      lacunarity *= frequencyRatio;
      pixel /= frequencyRatio;
      ++i;
   }

   if (pixel > pixelSize && i <= octaves[1])
   {
      float weight = CLAMP(pixel/pixelSize - 1.0f, 0.0f, 1.0f);
      sum += weight * amp * AiPerlin4(pp * lacunarity, time);
   }

   return sum * 0.5f + 0.5f;
}

float fTurbulence(AtShaderGlobals *sg,
                  const AtPoint &point,
                  float time,
                  float lacunarity,
                  float frequencyRatio,
                  int octaves[2],
                  float ratio,
                  float ripples[3])
{
   int i = 0;
   float mix = 0.0f;
   float amp = 1.0f;
   AtPoint pp;
   AiV3Create(pp, ripples[0], ripples[1], ripples[2]);
   pp = point * pp / 2.0f;

   // NOTE: this is wrong, sg->area is "world-space" area
   // sg->area is 0.0 for volumetric shaders
   float pixelSize = float(AI_EPSILON);
   float niquist = 2.0f * pixelSize;
   float pixel = 1.0;

   while ((i < octaves[1] && pixel > niquist) || i < octaves[0])
   {
      AtPoint2 offset;
      AiV2Create(offset, lacunarity, lacunarity);
      mix += amp * fabs(AiPerlin4((pp + AiPerlin2(offset)) * lacunarity, time));
      lacunarity *= frequencyRatio;
      amp *= ratio;
      pixel /= frequencyRatio;
      ++i;
   }

   if (pixel > pixelSize && i <= octaves[1])
   {
      AtVector2 offset;
      AiV2Create(offset, lacunarity, lacunarity);
      float weight = CLAMP((pixel/pixelSize - 1.0f), 0.0f, 1.0f);
      mix += weight * amp * fabs(AiPerlin4((pp+AiPerlin2(offset)) * lacunarity, time));
   }

   return mix;
}

AtPoint AnimatedCellNoise(const AtPoint &p, float tt)
{
   float t = AiCellNoise4(p, tt);
   float tbase = floor(t);

   AtPoint n1 = AiVCellNoise4(p, tbase);
   float d = t - tbase;
   AtPoint n2 = AiVCellNoise4(p, tbase + 1.0f);
   n1 += d * (n2 - n1);

   return n1;
}

int SuspendedParticles(const AtPoint &Pn,
                       float time,
                       float particleRadius,
                       float jitter,
                       float octave,
                       float &f1,
                       AtPoint &pos1,
                       float &f2,
                       AtPoint &pos2,
                       AtPoint (&particlePos)[27])
{
   AtPoint thiscell;
   AiV3Create(thiscell, floor(Pn.x)+0.5f, floor(Pn.y)+0.5f, floor(Pn.z)+0.5f);

   f1 = f2 = 1e36f;
   int i, j, k;

   unsigned int curr_particle = 0;
   for (i=-1; i<=1; ++i)
   {
      for (j=-1; j<=1; ++j)
      {
         for (k=-1; k<=1; ++k)
         {
            AtVector testvec;
            AiV3Create(testvec, (float)i, (float)j, (float)k);
            AtPoint testcell = thiscell + testvec;

            if (jitter > 0)
            {
               AtVector jit = AnimatedCellNoise(testcell, time+1000.0f*octave) - 0.5f;
               testcell += jitter * jit;
            }

            float dist = AiV3Dist(testcell, Pn);

            if (dist < particleRadius)
            {
               particlePos[curr_particle] = testcell;
               ++curr_particle;
               if (dist < f1)
               {
                  f2 = f1;
                  pos2 = pos1;
                  f1 = dist;
                  pos1 = testcell;
               }
               else if (dist < f2)
               {
                  f2 = dist;
                  pos2 = testcell;
               }
            }
         }
      }
   }
   return curr_particle;
}

int SuspendedParticles2d(const AtPoint &Pn,
                         float time,
                         float particleRadius,
                         float jitter,
                         float octave,
                         AtPoint (&particlePos)[27])
{
   AtPoint thiscell;
   AiV3Create(thiscell, floor(Pn.x)+0.5f, floor(Pn.y)+0.5f, 0.0f);

   int i, j;
   unsigned int curr_particle = 0;

   for (i=-1; i<=1; ++i)
   {
      for (j=-1; j<=1; ++j)
      {
         AtVector testvec;
         AiV3Create(testvec, (float)i, (float)j, 0.0f);
         AtPoint testcell = thiscell + testvec;
         
         if (jitter > 0.0f)
         {
            AtVector vjit = AnimatedCellNoise(testcell, time+1000.0f*octave) - 0.5f;
            vjit *= jitter;
            vjit.z = 0.0f;
            testcell += vjit;
         }

         float dist = AiV3Dist(testcell, Pn);

         if (dist < particleRadius)
         {
            particlePos[curr_particle] = testcell;
            ++curr_particle;
         }
      }
   }
   return curr_particle;
}

float ParticleDensity(int falloff,
                      const AtPoint &particleCenter,
                      const AtPoint &P,
                      float radius)
{
   float distanceToCenter = AiV3Dist(particleCenter, P);
   float fadeout = 0;

   if (falloff == 0) // linear
   {
      fadeout = distanceToCenter / radius;
   }
   else if (falloff == 1) // smooth
   {
      fadeout = SmoothStep(0.0f, radius, distanceToCenter);
   }
   else
   {
      fadeout = distanceToCenter / radius;
      fadeout = Bias(0.25f, fadeout);

      if (falloff == 3)
      {
         fadeout = CLAMP(1.0f-fadeout, 0.0f, 1.0f);
         fadeout += 1 - SmoothStep(0.0, 0.1f, fadeout);
      }
   }
   return 1.0f - fadeout;
}

float BillowNoise(const AtPoint &p,
                  float time,
                  int dim,
                  float radius,
                  float sizeRand,
                  float jitter,
                  int falloff,
                  float spottyness,
                  int octaves,
                  float frequencyRatio,
                  float ratio,
                  float amplitude)
{
   AtVector v;
   AiV3Create(v, 0.425f, 0.6f, dim == 3 ? 0.215f : 0.0f);
   AtPoint pp = p + v;

   int i, j;
   float lacunarity = 1.0f;

   AtPoint particles[27];
   float f1, f2;
   AtPoint /*p1,*/ p2;
   int numParticles;
   float sum = 0.0f;
   float amp = 1.0f;
   float ampSum = 0.0f;

   for (i=0; i<octaves; ++i)
   {
      if (dim == 3)
      {
         numParticles = SuspendedParticles(pp * lacunarity, time, radius, jitter, (float)i, f1, p2, f2, p2, particles);
      }
      else
      {
         numParticles = SuspendedParticles2d(pp * lacunarity, time, radius, jitter, (float)i, particles);
      }

      if (numParticles > 0)
      {
         for (j=0; j<numParticles; ++j)
         {
            float radiusScale = 1.0f;

            if (sizeRand != 0.0f)
            {
               radiusScale = (0.5f - CLAMP((AiPerlin3(particles[j])+1)*0.5f * 0.75f - 0.25f, 0.0f, 0.5f) * sizeRand) * 2.0f;
            }

            float density = ParticleDensity(falloff, particles[j], pp*lacunarity, radius*radiusScale);

            if (spottyness > 0)
            {
               AtVector v;
               AiV3Create(v, 1.0f, 7.0f, 1023.0f);
               float l = spottyness * (AiCellNoise3(particles[j]+v)*2.0f-1.0f);
               density += density * l;
               density = CLAMP(density, 0.0f, 1.0f);
            }

            sum += amp * density;
         }
      }

      ampSum += amp;
      amp *= ratio;
      lacunarity *= frequencyRatio;
   }
   return amplitude * sum / ampSum;
}

float CosWaves(float posX,
               float posY,
               float posTime,
               int numWaves)
{
   float x = posX * 2.0f * (float)AI_PI;
   float y = posY * 2.0f * (float)AI_PI;
   float time = posTime * 2.0f * (float)AI_PI;

   float dirX = 0;
   float dirY = 0;
   float norm = 0;
   float noiseValue = 0;
   int i = 0;

   for (i=1; i<=numWaves; ++i)
   {
      float generator = (float)i * (float)AI_PI / (float)numWaves;
      AtPoint v;
      AiV3Create(v, generator, 0.0f, 0.0f);
      dirX = AiPerlin3(v);
      AiV3Create(v, 0.0f, generator, 0.0f);
      dirY = AiPerlin3(v);
      AiV3Create(v, 0.0f, 0.0f, generator);
      float offset = AiPerlin3(v);
      generator = 50.0f * dirX * dirY;

      AtPoint2 v2;
      AiV2Create(v2, generator, generator);
      float freqNoise = AiPerlin2(v2);

      norm = sqrt(dirX * dirX + dirY * dirY);

      if (norm > AI_EPSILON)
      {
         dirX /= norm;
         dirY /= norm;
         noiseValue += static_cast<float>(cos((dirX*x + dirY*y) * (AI_PI) / (6.0f * freqNoise) + offset * time));
      }
   }

   noiseValue /= (float)numWaves;
   return noiseValue;
}

bool IsInShadingGroup(AtArray* set_ids, AtShaderGlobals* sg)
{
   if (set_ids != NULL && set_ids->nelements > 0)
   {
      //AiMsgInfo("set_ids length: %d", set_ids->nelements);
      //AtArray *sets = AiNodeGetArray(sg->Op, "shader");
      //if (sets != NULL && sets->nelements > 0)
      AtArray *sets = NULL;
      if (AiUDataGetArray("mtoa_shading_groups", &sets) && sets->nelements > 0)
      {
         if (sets->nelements == 1)
         {
            AtNode* shader = (AtNode*)AiArrayGetPtr(sets, 0);
            for (unsigned int i=0; i < set_ids->nelements; ++i)
            {
               if (shader == (AtNode*)AiArrayGetPtr(set_ids, i))
                  return true;
            }
            return false;
         }
//         else if (sets->nelements > 1)
//         {
//            //AiMsgInfo("here1 %s", AiNodeGetName(sg->Op));
//            AtArray* shidxs = AiNodeGetArray(sg->Op, "shidxs");
//            if (shidxs != NULL)
//            {
//               //AiMsgInfo("here2 %d %d", shidxs->nelements, sg->fi);
//               if (shidxs->nelements > 0)
//               {
//                  AtByte index = AiArrayGetByte(shidxs, sg->fi);
//                  if (set_id != AiArrayGetPtr(sets, index))
//                  {
//                     //AiMsgInfo("face assignment: %d != %d", set_id, AiArrayGetUInt(sets, index));
//                     return false;
//                  }
//               }
//            }
//         }
      }
   }
   return true;
}

AtArray* StringArrayToNodeArray(AtArray* setNames)
{
   std::vector<AtNode*> setNodes;
   for (unsigned int i=0; i < setNames->nelements; i++)
   {
      const char* nodeName = AiArrayGetStr(setNames, i);
      AtNode* setNode = AiNodeLookUpByName(nodeName);
      if (setNode != NULL)
         setNodes.push_back(setNode);
   }
   if (setNodes.size() > 0)
   {
      return AiArrayConvert((int)setNodes.size(), 1, AI_TYPE_NODE, &setNodes[0]);
   }
   else
   {
      return NULL;
   }
}


// Set sg->P to Pref if Pref is existing on the object.
//
// How to use it in your shader :
// At the beginning of shader_evaluate, place these lines :
//
//       AtPoint tmpPts;
//      bool usePref = SetRefererencePoints(sg, tmpPts);
//
// Use  RestorePoints() when you don't use Pref anymore :
//
//      if (usePref) RestorePoints(sg, tmpPts);
//
// @param sg      shading global.
// @param tmpTps   keep the original sg->P in this variable.
// @return          True if Pref was found and sg->P replaced.
//
bool SetRefererencePoints(AtShaderGlobals *sg, AtPoint &tmpPts)
{
   AtPoint Pref;
   bool usePref = AiUDataGetPnt("Pref",&Pref);
   if (usePref)
   {
      tmpPts = sg->P;
      sg->P = Pref;
      return true;
   }
   return false;
}

// Set sg->N to Nref if Nref is existing on the object.
//
// How to use it in your shader :
// At the beginning of shader_evaluate, place these lines :
//
//      AtVector tmpNmrs;
//      bool useNref = SetRefererenceNormals(sg, tmpNmrs);
//
// Use  RestoreNormals() when you don't use Nref anymore :
//
//      if (useNref) RestoreNormals(sg, tmpNmrs);
//
// @param sg        shading global.
// @param tmpTps    keep the original sg->N in this variable.
// @return          True if Nref was found and sg->N replaced.
//
bool SetRefererenceNormals(AtShaderGlobals *sg, AtVector &tmpNmrs)
{
   AtVector Nref;
   bool useNref = AiUDataGetVec("Nref",&Nref);
   if (useNref)
   {
      tmpNmrs = sg->N;
      sg->N = Nref;
      return true;
   }
   return false;
}

// Restore sg->P with original P. Use in combinaison with SetRefererencePoints.
//
// @param sg      shading global.
// @param tmpTps   The original P, given by SetRefererencePoints.
//
void RestorePoints(AtShaderGlobals *sg, AtPoint tmpPts)
{
   sg->P = tmpPts;
}

// Restore sg->N with original N. Use in combinaison with SetRefererenceNormals.
// @param sg      shading global.
// @param tmpNmrs   The original P, given by tmpNmrsv.
//
void RestoreNormals(AtShaderGlobals *sg, AtVector tmpNmrs)
{
   sg->N = tmpNmrs;
}

float sfrand(unsigned int *seed)
{
   union
   {
       float fres;
       unsigned int ires;
   };

   seed[0] *= 16807;
   *((unsigned int *) &ires) = ( ((unsigned int)seed[0])>>9 ) | 0x40000000;
   return fres-3.0f;
}

float rand01(unsigned int *seed)
{
   return sfrand(seed)*0.5f + 0.5f;
}
