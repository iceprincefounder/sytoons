#include <ai.h>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include "MayaUtils.h"
#include <string>
#include <cstring>

AI_SHADER_NODE_EXPORT_METHODS(MayaRampMtd);

namespace
{

enum MayaRampParams
{
   p_type = 0,
   p_interp,
   p_u_wave,
   p_v_wave,
   p_uvCoord,
   p_noise,
   p_noise_freq,
   p_positions,
   p_colors,
   p_hue_noise,
   p_sat_noise,
   p_val_noise,
   p_hue_noise_freq,
   p_sat_noise_freq,
   p_val_noise_freq,
   MAYA_COLOR_BALANCE_ENUM
};

enum RampType
{
   RT_V = 0,
   RT_U,
   RT_DIAGONAL,
   RT_RADIAL,
   RT_CIRCULAR,
   RT_BOX,
   RT_UV,
   RT_4CORNER,
   RT_TARTAN
};

const char* RampTypeNames[] =
{
   "v",
   "u",
   "diagonal",
   "radial",
   "circular",
   "box",
   "uv",
   "fourcorner",
   "tartan",
   NULL
};

static AtRGB hsvNoise(const AtRGB &in, float u, float v, float ha, float hf, float sa, float sf, float va, float vf)
{
   AtPoint2 p;

   AtVector hsv = RGBtoHSV(in);
   hsv.x /= 360.0f;

   if (ha > 0.0f)
   {
      p.x = (16 * hf * u + 0.75f);
      p.y = (16 * hf * v + 0.75f);
      hsv.x += ha * AiPerlin2(p);
      hsv.x = Mod(hsv.x, 1.000001f);
   }

   if (sa > 0.0f)
   {
      p.x = (16 * sf * u + 0.75f);
      p.y = (16 * sf * v + 0.75f);
      hsv.y += sa * AiPerlin2(p);
      hsv.y = CLAMP(hsv.y, 0.0f, 1.0f);
   }

   if (va > 0.0f)
   {
      p.x = (16 * vf * u + 0.75f);
      p.y = (16 * vf * v + 0.75f);
      hsv.z += va * AiPerlin2(p);
      hsv.z = CLAMP(hsv.z, 0.0f, 1.0f);
   }

   hsv.x *= 360.0f;
   return HSVtoRGB(hsv);
}

};

node_parameters
{
   AiParameterENUM("type", 1, RampTypeNames);
   AiParameterENUM("interpolation", 1, RampInterpolationNames);
   AiParameterFLT("uWave", 0.0f);
   AiParameterFLT("vWave", 0.0f);
   AiParameterPNT2("uvCoord", 0.0f, 0.0f);
   AiParameterFLT("noise", 0.0f);
   AiParameterFLT("noiseFreq", 0.5f);

   AiParameterARRAY("position", AiArray(0, 0, AI_TYPE_FLOAT));

   AiParameterARRAY("color", AiArray(0, 0, AI_TYPE_RGB));
   
   AiParameterFLT("hueNoise", 0.0f);
   AiParameterFLT("satNoise", 0.0f);
   AiParameterFLT("valNoise", 0.0f);
   AiParameterFLT("hueNoiseFreq", 0.5f);
   AiParameterFLT("satNoiseFreq", 0.5f);
   AiParameterFLT("valNoiseFreq", 0.5f);

   AddMayaColorBalanceParams(params, mds);

   AiMetaDataSetBool(mds, NULL, "maya.hide", true);
}

node_initialize
{
}

node_update
{
   // Unconnected render attributes (uvCoords, normalCamera, etc)
   // should use globals as following Maya's behavior
   if (!AiNodeGetLink(node, "uvCoord"))
   {
      AtPoint2 uv = AI_P2_ZERO;
      if (!AiNodeGetLink(node, "uvCoord.x")) uv.x = UV_GLOBALS;
      if (!AiNodeGetLink(node, "uvCoord.y")) uv.y = UV_GLOBALS;
      AiNodeSetPnt2(node, "uvCoord", uv.x, uv.y);
   }
}

node_finish
{
}

shader_evaluate
{
   AtRGB result = AI_RGB_BLACK;
   
   float hNoiseAmp = AiShaderEvalParamFlt(p_hue_noise);
   float hNoiseFreq = AiShaderEvalParamFlt(p_hue_noise_freq);
   float sNoiseAmp = AiShaderEvalParamFlt(p_sat_noise);
   float sNoiseFreq = AiShaderEvalParamFlt(p_sat_noise_freq);
   float vNoiseAmp = AiShaderEvalParamFlt(p_val_noise);
   float vNoiseFreq = AiShaderEvalParamFlt(p_val_noise_freq);
   bool applyHsvNoise = (hNoiseAmp > 0.0f || sNoiseAmp > 0.0f || vNoiseAmp > 0.0f);

   AtPoint2 uv = {0.0f, 0.0f};
   if (!AiStateGetMsgPnt2("maya_ramp_uv_override", &uv))
   {
      uv = AiShaderEvalParamPnt2(p_uvCoord);
      // Will be set to GLOBALS by update if unconnected
      if (uv.x == UV_GLOBALS) uv.x = sg->u;
      if (uv.y == UV_GLOBALS) uv.y = sg->v;
   }

   if (!IsValidUV(uv))
   {
      // early out
      MayaDefaultColor(sg, node, p_defaultColor, sg->out.RGBA);
      return;
   }
   
   float u = uv.x;
   float v = uv.y;

   float uWave = AiShaderEvalParamFlt(p_u_wave);
   float vWave = AiShaderEvalParamFlt(p_v_wave);
   float noiseAmp = AiShaderEvalParamFlt(p_noise);
   float noiseFreq = AiShaderEvalParamFlt(p_noise_freq);

   if (noiseAmp > 0.0f)
   {
      AtPoint2 puv;
      puv.x = u * 16 * noiseFreq + 0.75f;
      puv.y = v * 16 * noiseFreq + 0.75f;
      float n = noiseAmp * AiPerlin2(puv);
      u += n;
      v += n;
   }

   float preWaveV = v;

   if (vWave > 0.0f)
   {
      v += vWave * static_cast<float>(sin(u * AI_PITIMES2));
   }

   if (uWave > 0.0f)
   {
      u -= uWave * static_cast<float>(sin(preWaveV * AI_PITIMES2));
   }

   // for hsvNoise, we should use non-wrapped uvs or we'll have discontinuities
   float nu = u;
   float nv = v;
   u = Mod(u, 1.000001f);
   v = Mod(v, 1.000001f);
   
   // Read positions and colors
   AtArray* positions = AiShaderEvalParamArray(p_positions);
   AtArray* colors = AiShaderEvalParamArray(p_colors);

   if (positions->nelements > 0)
   {
      if (positions->nelements == 1)
      {
         // Only one color entry then it's a plain color / texture
         result = AiArrayGetRGB(colors, 0);
      }
      else // (positions->nelements > 1)
      {
         // get array with sorted index
         unsigned int* shuffle = (unsigned int*)AiShaderGlobalsQuickAlloc(sg, positions->nelements * sizeof(unsigned int));
         SortFloatIndexArray(positions, shuffle);

         int type = AiShaderEvalParamInt(p_type);
         RampInterpolationType interp = (RampInterpolationType) AiShaderEvalParamInt(p_interp);
         switch (type)
         {
         case RT_U:
            Ramp(positions, colors, u, interp, result, shuffle);
            break;
         case RT_V:
            Ramp(positions, colors, v, interp, result, shuffle);
            break;
         case RT_DIAGONAL:
            {
               float t = 0.5f * (u + v);
               Ramp(positions, colors, t, interp, result, shuffle);
            }
            break;
         case RT_RADIAL:
            {
               float x = u - 0.5f;
               float y = v - 0.5f;
               float a = atan2(y, x);
               float t = static_cast<float>(a * AI_ONEOVER2PI);
               t = - t - 0.25f;
               if (t < 0.0f)
                  t += 1.0f;
               Ramp(positions, colors, t, interp, result, shuffle);
            }
            break;
         case RT_CIRCULAR:
            {
               float x = u - 0.5f;
               float y = v - 0.5f;
               float t = float(sqrt(2.0) * sqrt(x*x + y*y));
               Ramp(positions, colors, t, interp, result, shuffle);
            }
            break;
         case RT_BOX:
            {
               float t = 2.0f * MAX(fabs(u-0.5f), fabs(v-0.5f));
               Ramp(positions, colors, t, interp, result, shuffle);
            }
            break;
         case RT_UV:
            {
               float t0 = 2.0f * fabs(u - 0.5f);
               float t1 = 2.0f * fabs(v - 0.5f);
               AtRGB c0, c1;
               Ramp(positions, colors, t0, interp, c0, shuffle);
               Ramp(positions, colors, t1, interp, c1, shuffle);
               result = c0 * c1;
            }
            break;
         case RT_4CORNER:
            {
               if (positions->nelements < 1)
               {
                  result.r = 0.0f;
                  result.g = 0.0f;
                  result.b = 0.0f;
               }
               else
               {
                  // Maya do not use the ordered colors but the created order
                  result = (1.0f - u) * (1.0f - v) * AiArrayGetRGB(colors, 0);
                  if (positions->nelements > 1)
                  {
                     result = result + (u * (1.0f - v) * AiArrayGetRGB(colors, 1));
                  }
                  if (positions->nelements > 2)
                  {
                     result = result + ((1.0f - u) * v * AiArrayGetRGB(colors, 2));
                  }
                  if (positions->nelements > 3)
                  {
                     result = result + (u * v * AiArrayGetRGB(colors, 3));
                  }
               }
            }
            break;
         case RT_TARTAN:
            {
               float t0 = 2.0f * fabs(u - 0.5f);
               float t1 = 2.0f * fabs(v - 0.5f);
               AtRGB c0, c1;
               Ramp(positions, colors, t0, interp, c0, shuffle);
               Ramp(positions, colors, t1, interp, c1, shuffle);
               result = 0.5f * (c0 + c1);
            }
            break;
         default:
            Ramp(positions, colors, v, interp, result, shuffle);
            break;
         }
      }
   }

   if (applyHsvNoise)
   {
      AiRGBtoRGBA(hsvNoise(result, nu, nv, hNoiseAmp, hNoiseFreq, sNoiseAmp, sNoiseFreq, vNoiseAmp, vNoiseFreq), sg->out.RGBA);
   }
   else
   {
      AiRGBtoRGBA(result, sg->out.RGBA);
   }

   // Alpha output is always the luminance
   // so translator should always set alphaIsLuminance = true;
   MayaColorBalance(sg, node, p_defaultColor, sg->out.RGBA);


}

node_loader
{
   if (i > 0)
      return false;
   node->methods     = MayaRampMtd;
   node->output_type = AI_TYPE_RGBA;
   node->name        = "SSMayaRamp";
   node->node_type   = AI_NODE_SHADER;
   strcpy(node->version, AI_VERSION);
   return true;
}