#include <ai.h>
#include "alUtil.h"

AI_SHADER_NODE_EXPORT_METHODS(alCel)

enum alCelParams
{
    p_surfaceShader,
    p_diffuseDirectStrength,
    p_diffuseRamp,
    p_diffuseIndirectStrength,
    p_diffuseIndirectSaturation,
    p_diffuseIndirectTint,

    p_aov_direct_diffuse_cel,
    p_aov_direct_diffuse_raw_cel,
    p_aov_indirect_diffuse_cel,



    p_aiEnableMatte,
    p_aiMatteColor,
    p_aiMatteColorA
};

node_parameters
{
    AiParameterRGB("surfaceShader", 0.0f, 0.0f, 0.0f);
    AiParameterFlt("diffuseDirectStrength", 0.75f);
    AiParameterRGB("diffuseRamp", 0.0f, 0.0f, 0.0f);
    AiParameterFlt("diffuseIndirectStrength", 1.0f);
    AiParameterFlt("diffuseIndirectSaturation", 1.0f);
    AiParameterRGB("diffuseIndirectTint", 1.0f, 1.0f, 1.0f);

    AiParameterStr("aov_direct_diffuse_cel", "direct_diffuse_cel");
    AiParameterStr("aov_direct_diffuse_raw_cel", "direct_diffuse_raw_cel");
    AiParameterStr("aov_indirect_diffuse_cel", "indirect_diffuse_cel");


    AiParameterBOOL("aiEnableMatte", false);
    AiParameterRGB("aiMatteColor", 0.0f, 0.0f, 0.0f);
    AiParameterFlt("aiMatteColorA", 0.0f);
}

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

#define LUT_SIZE 32
struct ShaderData
{
    /*
    AtRGB    diffuseLUT[LUT_SIZE];
    AtArray* diffusePositions;
    AtArray* diffuseColors;
    RampInterpolationType      diffuseInterp;
    */
    std::string aov_direct_diffuse_cel;
    std::string aov_direct_diffuse_raw_cel;
    std::string aov_indirect_diffuse_cel;
};

node_initialize
{
    ShaderData* shaderData = new ShaderData;
    /*
    shaderData->diffusePositions = NULL;
    shaderData->diffuseColors = NULL;
    */
    AiNodeSetLocalData(node, shaderData);
}

node_finish
{
    ShaderData* shaderData = (ShaderData*)AiNodeGetLocalData(node);
    delete shaderData;
    AiNodeSetLocalData(node, NULL); 
}

void getMayaRampArrays(AtNode* node, const char* paramName, AtArray** positions, AtArray** colors, RampInterpolationType* interp)
{
    *positions = NULL;
    *colors = NULL;
    if (AiNodeIsLinked(node, paramName))
    {
        AtNode* cn = AiNodeGetLink(node, paramName);
        const AtNodeEntry* cne = AiNodeGetNodeEntry(cn);
        if (!strcmp(AiNodeEntryGetName(cne), "MayaRamp"))
        {
            *positions = AiNodeGetArray(cn, "position");
            *colors = AiNodeGetArray(cn, "color");
            *interp = (RampInterpolationType)AiNodeGetInt(cn, "interpolation");

        }
        else
        {
            AiMsgWarning("[alCel] %s is connected but connection is not a MayaRamp", paramName);
        }
    }
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

// This one is defined for the RampT template function to work properly
float RampLuminance(float v)
{
   return v;
}

float RampLuminance(const AtRGB &color)
{
   return (0.3f * color.r + 0.3f * color.g + 0.3f * color.b);
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

float _GetArrayFlt(AtArray *a, unsigned int i)
{
   return AiArrayGetFlt(a, i);
}

AtRGB _GetArrayRGB(AtArray *a, unsigned int i)
{
   return AiArrayGetRGB(a, i);
}

void Ramp(AtArray *p, AtArray *v, float t, RampInterpolationType it, float &out, unsigned int *shuffle)
{
   RampT(p, v, t, it, out, _GetArrayFlt, shuffle);
}

void Ramp(AtArray *p, AtArray *v, float t, RampInterpolationType it, AtRGB &out, unsigned int *shuffle)
{
   RampT(p, v, t, it, out, _GetArrayRGB, shuffle);
}

void generateRampLUT(AtArray* positions, AtArray* colors, RampInterpolationType interp, AtRGB* lut)
{
    unsigned int* shuffle = new unsigned int[positions->nelements];
    SortFloatIndexArray(positions, shuffle);
    for (int i=0; i < LUT_SIZE; ++i)
    {
        float t = float(i)/float(LUT_SIZE-1);
        Ramp(positions, colors, t, interp, lut[i], shuffle);
    }

    delete[] shuffle;
}

AtRGB rampLUTLookup(AtRGB* lut, float t)
{
    float tt = clamp(t*(LUT_SIZE-1), 0.0f, float(LUT_SIZE-1));
    int i = int(tt);
    int in = std::min(i+1, LUT_SIZE-1);
    tt -= float(i);
    return lerp(lut[i], lut[in], tt);
}

node_update
{
    ShaderData* shaderData = (ShaderData*)AiNodeGetLocalData(node);

    shaderData->aov_direct_diffuse_cel = params[p_aov_direct_diffuse_cel].STR;
    shaderData->aov_direct_diffuse_raw_cel = params[p_aov_direct_diffuse_raw_cel].STR;
    shaderData->aov_indirect_diffuse_cel = params[p_aov_indirect_diffuse_cel].STR;

    AiAOVRegister(shaderData->aov_direct_diffuse_cel.c_str(), AI_TYPE_RGB, AI_AOV_BLEND_OPACITY);
    AiAOVRegister(shaderData->aov_direct_diffuse_raw_cel.c_str(), AI_TYPE_RGB, AI_AOV_BLEND_OPACITY);
    AiAOVRegister(shaderData->aov_indirect_diffuse_cel.c_str(), AI_TYPE_RGB, AI_AOV_BLEND_OPACITY);

    // get the arrays from the connected Maya Ramp node
    //getMayaRampArrays(node, "diffuseRamp", &shaderData->diffusePositions, &shaderData->diffuseColors, &shaderData->diffuseInterp);
    //getMayaRampArrays(node, "diffuseColorRamp", &shaderData->colorPositions, &shaderData->colorColors, &shaderData->colorInterp);

    //if (shaderData->diffusePositions && shaderData->diffuseColors) evalRamp(shaderData->diffusePositions, shaderData->diffuseColors, shaderData->diffuseInterp, shaderData->diffuseLUT);
}

shader_evaluate
{
    ShaderData* shaderData = (ShaderData*)AiNodeGetLocalData(node);

    sg->out_opacity = AI_RGB_WHITE;

    // pull on the connected surface shader
    AtRGB result = AiShaderEvalParamRGB(p_surfaceShader);


    // we only do cel shading in the camera rays
    if (sg->Rt & AI_RAY_CAMERA)
    {
        AtRGB diffuseLUT[LUT_SIZE];
        AtArray* diffusePositions = NULL;
        AtArray* diffuseColors = NULL;
        RampInterpolationType diffuseInterp;

        getMayaRampArrays(node, "diffuseRamp", &diffusePositions, &diffuseColors, &diffuseInterp);

        // if the diffuse array is connected
        if (diffusePositions && diffuseColors)
        {
            // grab the results from the surface shader
            AtRGB direct_diffuse_raw = AI_RGB_BLACK;
            AtRGB diffuse_color = AI_RGB_BLACK;
            AtRGB direct_specular = AI_RGB_BLACK;
            AtRGB indirect_diffuse = AI_RGB_BLACK;
            //AiAOVGetRGB(sg, "direct_diffuse_raw", direct_diffuse_raw);
            AiStateGetMsgRGB("als_diffuse_color", &diffuse_color);
            AiStateGetMsgRGB("als_direct_diffuse_raw", &direct_diffuse_raw);
            AiStateGetMsgRGB("als_direct_specular", &direct_specular);
            AiStateGetMsgRGB("als_indirect_diffuse", &indirect_diffuse);

            // remap and clamp it
            float diffuseDirectStrength = AiShaderEvalParamFlt(p_diffuseDirectStrength);
            float diff_t = direct_diffuse_raw.r * diffuseDirectStrength;
            diff_t = clamp(diff_t, 0.0f, 1.0f);

            // lookup the diffuse ramp
            unsigned int* shuffle = (unsigned int*)AiShaderGlobalsQuickAlloc(sg, sizeof(unsigned int) * diffusePositions->nelements);
            SortFloatIndexArray(diffusePositions, shuffle);
            Ramp(diffusePositions, diffuseColors, diff_t, diffuseInterp, direct_diffuse_raw, shuffle);

            AtRGB direct_diffuse = direct_diffuse_raw * diffuse_color;

            float diffuseIndirectStrength = AiShaderEvalParamFlt(p_diffuseIndirectStrength);
            float diffuseIndirectSaturation = AiShaderEvalParamFlt(p_diffuseIndirectSaturation);
            AtRGB diffuseIndirectTint = AiShaderEvalParamRGB(p_diffuseIndirectTint);

            if (diffuseIndirectSaturation != 1.0f)
            {
                float lum = luminance(indirect_diffuse);
                indirect_diffuse = lerp(AiColor(lum), indirect_diffuse, diffuseIndirectSaturation);
            }

            indirect_diffuse *= diffuseIndirectStrength * diffuseIndirectTint;

            AiAOVSetRGB(sg, shaderData->aov_direct_diffuse_cel.c_str(), direct_diffuse);
            AiAOVSetRGB(sg, shaderData->aov_direct_diffuse_raw_cel.c_str(), direct_diffuse_raw);
            AiAOVSetRGB(sg, shaderData->aov_indirect_diffuse_cel.c_str(), indirect_diffuse);

            result = direct_diffuse + indirect_diffuse + direct_specular;
        }

    }

    sg->out.RGB = result;
}


node_loader
{
   if (i>0) return 0;
   node->methods     = alCel;
   node->output_type = AI_TYPE_RGB;
   node->name        = "alCel";
   node->node_type   = AI_NODE_SHADER;
   strcpy(node->version, AI_VERSION);
   return true;
}