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
	p_diffuse_color,
	p_specular_color,
	p_roughness,
	//p_specbalance //hide parameter specbalance until we need ray-tracing render
	p_casting_light,
	p_casting_texture,
	
	p_aov_color_major,
	p_aov_color_shadow,
	p_aov_color_mask,
	p_aov_color_extra,
	p_aov_diffuse_color,
	p_aov_specular_color,
};

node_parameters
{
	AiParameterEnum("Engine", S_SCANLINE, engine_params);
	AiParameterRGB("color_major", 1.0f, 1.0f, 1.0f);
	AiParameterRGB("color_shadow", 0.0f, 0.0f, 0.0f);
	AiParameterFLT("color_mask", 0.0f);
	AiParameterRGB("color_extra", 0.0f, 0.0f, 0.0f);
	AiParameterRGB("diffuse_color", 0.7f, 0.7f, 0.7f);
	AiParameterRGB("specular_color", 1.0f, 1.0f, 1.0f);
	AiParameterFLT("roughness", 0.2f);
	//AiParameterFLT("specbalance", 0.1f); ////hide parameter specbalance until we need ray-tracing render
	AiParameterBool("casting_light", true);
	AiParameterBool("casting_texture", true);

	AiParameterStr("aov_color_major", "color_major");
	AiParameterStr("aov_color_shadow", "color_shadow");
	AiParameterStr("aov_color_mask", "color_mask");
	AiParameterStr("aov_color_extra", "color_extra");
	AiParameterStr("aov_diffuse_color", "diffuse_color");
	AiParameterStr("aov_specular_color", "specular_color");
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
	
	AtColor color_major = AiShaderEvalParamRGB(p_color_major);
	AtColor color_shadow = AiShaderEvalParamRGB(p_color_shadow);
	AtColor color_mask = AiShaderEvalParamRGB(p_color_mask);
	AtColor color_extra = AiShaderEvalParamRGB(p_color_extra);
	bool casting_light = AiShaderEvalParamBool(p_casting_light);
	bool casting_texture = AiShaderEvalParamBool(p_casting_texture);

	// Set flat shader AOVs
	AiAOVSetRGB(sg, data->aovs_custom[k_color_major].c_str(), color_major);
	AiAOVSetRGB(sg, data->aovs_custom[k_color_shadow].c_str(), color_shadow);
	AiAOVSetRGB(sg, data->aovs_custom[k_color_mask].c_str(), color_mask);
	AiAOVSetRGB(sg, data->aovs_custom[k_color_extra].c_str(), color_extra);
	// do shading
	AtColor lighting_result = AI_RGB_BLACK;
	switch (shading_engine)
	{
		case S_SCANLINE:
		{
			AtColor Kd = AiShaderEvalParamRGB(p_diffuse_color);
			AtColor Ks = AiShaderEvalParamRGB(p_specular_color);
			float Wig = 0.28;
			float roughness = 10 / AiShaderEvalParamFlt(p_roughness);
			AiLightsPrepare(sg);
			AtColor LaD = AI_RGB_BLACK; // initialize light accumulator to = 0
			AtColor LaS = AI_RGB_BLACK; // initialize light accumulator to = 0
			while (AiLightsGetSample(sg)) // loop over the lights
			{
				float LdotN = AiV3Dot(sg->Ld, sg->Nf);
				if (LdotN < 0) LdotN = 0;
				AtVector H = AiV3Normalize(-sg->Rd + sg->Ld);
				float spec = AiV3Dot(sg->Nf, H); // N dot H
				if (spec < 0) spec = 0;
				// Lambertian diffuse
				LaD += sg->Li * Wig * sg->we * LdotN * Kd;
				// Blinn-Phong specular
				LaS += sg->Li * Wig * sg->we * pow(spec, roughness) * Ks;
			}

			// add diffuse and specular into custom AOVs
			if (LaD != AI_RGB_BLACK)
				AiAOVSetRGB(sg, data->aovs_custom[k_diffuse_color].c_str(), LaD);
			if (LaS != AI_RGB_BLACK)
				AiAOVSetRGB(sg, data->aovs_custom[k_specular_color].c_str(), LaS);

			// color = accumulated light + ambient
			lighting_result = LaD + LaS;
			break;   	   			
		}
		case S_RAYTRACE:
		{
			// Kd (diffuse color), Ks (specular color), and roughness (scalar)
			AtColor Kd = AiShaderEvalParamRGB(p_diffuse_color);
			AtColor Ks = AiShaderEvalParamRGB(p_specular_color);
			float roughness = AiShaderEvalParamFlt(p_roughness);
			float specbalance = 0.1f; //hide parameter specbalance until we need ray-tracing render

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

			// add direct diffuse and direct specular into custom AOVs
			if (Dda != AI_RGB_BLACK)
				AiAOVSetRGB(sg, data->aovs_custom[k_diffuse_color].c_str(), Dda);
			if (Dsa != AI_RGB_BLACK)
				AiAOVSetRGB(sg, data->aovs_custom[k_specular_color].c_str(), Dsa);

			// add up indirect and direct contributions
			lighting_result = Kd * (Dda + IDd) + Ks * (Dsa + IDs);
			break;		
			}
		default:
		{
			lighting_result = AI_RGB_RED;
			break;
			}
	}
	//sg->out.RGB = color_major + color_secondary*color_shadow;
	AtColor texture_result = lerp(color_shadow,color_major,color_mask.r) + color_extra;
	if(casting_light)
		sg->out.RGB = lighting_result;
	if(casting_texture)
		sg->out.RGB = texture_result;
}