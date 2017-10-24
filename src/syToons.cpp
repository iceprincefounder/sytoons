//
//Copyright 2017 Beijing ShengYing Film Animation Co.Ltd
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
#include <ai.h>
#include <cstring>

#include <al_util.h>
#include <kt_util.h>


AI_SHADER_NODE_EXPORT_METHODS(syToonsMethods);

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


enum EngineParams
{
	S_SCANLINE = 0,
	S_RAYTRACE
};

const char* engine_params[] = 
{
	"Scanline",
	"Raytrace",
	NULL
};

enum Params {
	p_engine,
	p_color_major,
	p_color_shadow,
	p_color_mask,
	p_color_extra,
	p_shadow_ramp,
	p_shadow_position,
	p_lambert_color,
	//p_specular_color,
	//p_roughness,
	//p_specbalance //hide parameter specbalance until we need ray-tracing render
	p_casting_light,
	p_casting_texture,
	p_aov_sytoons_beauty,
	p_aov_color_major,
	p_aov_color_shadow,
	p_aov_color_mask,
	p_aov_color_extra,
	p_aov_diffuse_color,
	p_aov_specular_color,
};

node_parameters
{
	AiParameterEnum("engine", S_SCANLINE, engine_params);
	AiParameterRGB("color_major", 1.0f, 1.0f, 1.0f);
	AiParameterRGB("color_shadow", 0.0f, 0.0f, 0.0f);
	AiParameterRGB("color_mask", 1.0f,1.0f,1.0f);
	AiParameterRGB("color_extra", 0.0f, 0.0f, 0.0f);
	AiParameterRGB("lambert_color", 1.0f, 1.0f, 1.0f);
	AiParameterRGB("shadow_ramp", 0.0f, 0.0f, 0.0f);
	AiParameterFLT("shadow_position", 0.5f);
	//AiParameterRGB("specular_color", 1.0f, 1.0f, 1.0f);
	//AiParameterFLT("roughness", 0.2f);
	//AiParameterFLT("specbalance", 0.1f); ////hide parameter specbalance until we need ray-tracing render
	AiParameterBool("casting_light", true);
	AiParameterBool("casting_texture", false);

	AiParameterStr("aov_sytoons_beauty", "aov_sytoons_beauty");
	AiParameterStr("aov_color_major", "aov_color_major");
	AiParameterStr("aov_color_shadow", "aov_color_shadow");
	AiParameterStr("aov_color_mask", "aov_color_mask");
	AiParameterStr("aov_color_extra", "aov_color_extra");
	AiParameterStr("aov_diffuse_color", "aov_diffuse_color");
	AiParameterStr("aov_specular_color", "aov_specular_color");
}

node_initialize
{
	ShaderData* data = new ShaderData();
	AiNodeSetLocalData(node, data);
}

node_update
{
	ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
	// set up AOVs
	REGISTER_AOVS_CUSTOM
}

node_finish
{
	if (AiNodeGetLocalData(node))
	{
		ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
		AiNodeSetLocalData(node, NULL);
		delete data;
	}
}

shader_evaluate
{
	ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
	// we provide two shading engine,traditional scanline and GI engine raytrace.
	int shading_engine = AiShaderEvalParamInt(p_engine);
	AtColor result = AI_RGB_BLACK;
	AtColor color_major = AiShaderEvalParamRGB(p_color_major);
	AtColor color_shadow = AiShaderEvalParamRGB(p_color_shadow);
	AtColor color_mask = AiShaderEvalParamRGB(p_color_mask);
	AtColor color_extra = AiShaderEvalParamRGB(p_color_extra);
	bool casting_light = AiShaderEvalParamBool(p_casting_light);
	bool casting_texture = AiShaderEvalParamBool(p_casting_texture);

	// Set flat shader AOVs
	AiAOVSetRGB(sg, data->aovs_custom[k_aov_color_major].c_str(), color_major);
	AiAOVSetRGB(sg, data->aovs_custom[k_aov_color_shadow].c_str(), color_shadow);
	AiAOVSetRGB(sg, data->aovs_custom[k_aov_color_mask].c_str(), color_mask);
	AiAOVSetRGB(sg, data->aovs_custom[k_aov_color_extra].c_str(), color_extra);

	AtColor texture_result = lerp(color_shadow,color_major,color_mask.r) + color_extra;

	// do shading
	AtColor lighting = AI_RGB_BLACK;
	AtColor lighting_result = AI_RGB_BLACK;
	switch (shading_engine)
	{
		case S_SCANLINE:
		{
			AtColor Kd = AiShaderEvalParamRGB(p_lambert_color);
			//AtColor Ks = AiShaderEvalParamRGB(p_specular_color);
			float Wig = 0.28;
			//float roughness = 10 / AiShaderEvalParamFlt(p_roughness);
			AiLightsPrepare(sg);
			AtColor LaD = AI_RGB_BLACK; // initialize light accumulator to = 0
			//AtColor LaS = AI_RGB_BLACK; // initialize light accumulator to = 0
			while (AiLightsGetSample(sg)) // loop over the lights
			{
				float LdotN = AiV3Dot(sg->Ld, sg->Nf);
				if (LdotN < 0) LdotN = 0;
				AtVector H = AiV3Normalize(-sg->Rd + sg->Ld);
				//float spec = AiV3Dot(sg->Nf, H); // N dot H
				//if (spec < 0) spec = 0;
				// Lambertian diffuse
				LaD += sg->Li * Wig * sg->we * LdotN * Kd;
				// Blinn-Phong specular
				//LaS += sg->Li * Wig * sg->we * pow(spec, roughness) * Ks;
			}

			// add diffuse and specular into custom AOVs
			if (LaD != AI_RGB_BLACK)
				AiAOVSetRGB(sg, data->aovs_custom[k_aov_diffuse_color].c_str(), LaD);
			//if (LaS != AI_RGB_BLACK)
			//	AiAOVSetRGB(sg, data->aovs_custom[k_aov_specular_color].c_str(), LaS);

			// color = accumulated light + ambient
			//lighting_result = LaD + LaS;
			lighting = LaD;// from this time,we don`t provide specular.
			break;   	   			
		}
		case S_RAYTRACE:
		{
			// Kd (diffuse color), Ks (specular color), and roughness (scalar)
			AtColor Kd = AiShaderEvalParamRGB(p_lambert_color);
			//AtColor Ks = AiShaderEvalParamRGB(p_specular_color);
			//float roughness = AiShaderEvalParamFlt(p_roughness);
			float specbalance = 0.1f; //hide parameter specbalance until we need ray-tracing render

			// direct specular and diffuse accumulators, 
			// and indirect diffuse and specular accumulators...
			AtColor Dsa,Dda,IDs,IDd;
			Dsa = Dda = IDs = IDd = AI_RGB_BLACK;
			//void *spec_data = AiWardDuerMISCreateData(sg, NULL, NULL, roughness, roughness);
			void *diff_data = AiOrenNayarMISCreateData(sg, 0.0f);
			AiLightsPrepare(sg);
			while (AiLightsGetSample(sg)) // loop over the lights to compute direct effects
			{
				// direct specular
				//if (AiLightGetAffectSpecular(sg->Lp))
				//	Dsa += AiEvaluateLightSample(sg, spec_data, AiWardDuerMISSample, AiWardDuerMISBRDF, AiWardDuerMISPDF) * specbalance;
				// direct diffuse
				if (AiLightGetAffectDiffuse(sg->Lp))
					Dda += AiEvaluateLightSample(sg, diff_data, AiOrenNayarMISSample, AiOrenNayarMISBRDF, AiOrenNayarMISPDF) * (1-specbalance);
			}
			// indirect specular
			//IDs = AiWardDuerIntegrate(&sg->Nf, sg, &sg->dPdu, &sg->dPdv, roughness, roughness) * specbalance;
			// indirect diffuse
			IDd = AiOrenNayarIntegrate(&sg->Nf, sg, 0.0f) * (1-specbalance);

			// add direct diffuse and direct specular into custom AOVs
			if (Dda != AI_RGB_BLACK)
				AiAOVSetRGB(sg, data->aovs_custom[k_aov_diffuse_color].c_str(), Dda);
			//if (Dsa != AI_RGB_BLACK)
			//	AiAOVSetRGB(sg, data->aovs_custom[k_aov_specular_color].c_str(), Dsa);

			// add up indirect and direct contributions
			lighting = Kd * (Dda + IDd);
			break;		
			}
		default:
		{
			lighting = AI_RGB_BLACK;
			break;
		}
	}

	// we only do cel shading in the camera rays
	if (sg->Rt & AI_RAY_CAMERA)
	{
		AtRGB diffuseLUT[LUT_SIZE];
		AtArray* diffusePositions = NULL;
		AtArray* diffuseColors = NULL;
		RampInterpolationType diffuseInterp;

		getMayaRampArrays(node, "diffuse_color", &diffusePositions, &diffuseColors, &diffuseInterp);
	}
	if(casting_light)
		result = lighting_result;
	if(casting_texture)
		result = texture_result;
	AiAOVSetRGB(sg, data->aovs_custom[k_aov_sytoons_beauty].c_str(), result);
	sg->out.RGB = result;
}