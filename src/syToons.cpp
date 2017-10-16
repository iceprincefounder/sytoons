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


AI_SHADER_NODE_EXPORT_METHODS(syToonsMethods);

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
	p_Ka_color, 
	p_Kd_color, 
	p_Ks_color, 
	p_roughness,
	p_specbalance 
};

node_parameters
{
	AiParameterEnum("Engine", S_SCANLINE, engine_params);
	AiParameterRGB("Ka_color", 0.0f, 0.0f, 0.0f);
	AiParameterRGB("Kd_color", 0.7f, 0.7f, 0.7f);
	AiParameterRGB("Ks_color", 0.7f, 0.7f, 0.7f);
	AiParameterFLT("roughness", 0.2f);
	AiParameterFLT("specbalance", 0.1f);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
	// we provide two shading engine,traditional scanline and GI engine raytrace.
   int shading_engine = AiShaderEvalParamInt(p_engine);
   AtColor Ka = AiShaderEvalParamRGB(p_Ka_color);
   AtColor Kd = AiShaderEvalParamRGB(p_Kd_color);
   AtColor Ks = AiShaderEvalParamRGB(p_Ks_color);
   float roughness = 10 / AiShaderEvalParamFlt(p_roughness);
   // do shading
   switch (shading_engine)
   {
   		case S_SCANLINE:
   		{
			AiLightsPrepare(sg);
			AtColor La = AI_RGB_BLACK; // initialize light accumulator to = 0
			while (AiLightsGetSample(sg)) // loop over the lights
			{ 
			  float LdotN = AiV3Dot(sg->Ld, sg->Nf);
			  if (LdotN < 0) LdotN = 0;
			  AtVector H = AiV3Normalize(-sg->Rd + sg->Ld);
			  float spec = AiV3Dot(sg->Nf, H); // N dot H
			  if (spec < 0) spec = 0;
			  // Lambertian diffuse
			  La += sg->Li * sg->we * LdotN * Kd;
			  // Blinn-Phong specular
			  La += sg->Li * sg->we * pow(spec, roughness) * Ks;
			}
			// color = accumulated light + ambient
			sg->out.RGB = La + Ka;
			break;   	   			
   		}
   		case S_RAYTRACE:
   		{
			// Kd (diffuse color), Ks (specular color), and roughness (scalar)
			AtColor Kd = AiShaderEvalParamRGB(p_Kd_color);
			AtColor Ks = AiShaderEvalParamRGB(p_Ks_color);
			float roughness = AiShaderEvalParamFlt(p_roughness);
			float specbalance = AiShaderEvalParamFlt(p_specbalance);

			// direct specular and diffuse accumulators, 
			// and indirect diffuse and specular accumulators...
			AtColor Dsa,Dda,IDs,IDd;
			Dsa = Dda = IDs = IDd = AI_RGB_BLACK;
			void *spec_data = AiWardDuerMISCreateData(sg, NULL, NULL, roughness, roughness);
			void *diff_data = AiOrenNayarMISCreateData(sg, 0.0f);
			AiLightsPrepare(sg);
			while (AiLightsGetSample(sg)) // loop over the lights to compute direct effects
			{
			  // direct specular
			  if (AiLightGetAffectSpecular(sg->Lp))
			     Dsa += AiEvaluateLightSample(sg, spec_data, AiWardDuerMISSample, AiWardDuerMISBRDF, AiWardDuerMISPDF) * specbalance;
			  // direct diffuse
			  if (AiLightGetAffectDiffuse(sg->Lp))
			     Dda += AiEvaluateLightSample(sg, diff_data, AiOrenNayarMISSample, AiOrenNayarMISBRDF, AiOrenNayarMISPDF) * (1-specbalance);
			}
			// indirect specular
			IDs = AiWardDuerIntegrate(&sg->Nf, sg, &sg->dPdu, &sg->dPdv, roughness, roughness) * specbalance;
			// indirect diffuse
			IDd = AiOrenNayarIntegrate(&sg->Nf, sg, 0.0f) * (1-specbalance);

			// add up indirect and direct contributions
			sg->out.RGB = Kd * (Dda + IDd) + Ks * (Dsa + IDs);
			break;		
   		}
   		default:
   		{
   			sg->out.RGB = AI_RGB_RED;
   			break;
   		}
   }
}