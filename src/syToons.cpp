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
	S_SCANLINE= 0,
	S_RAYTRACE
};

const char* engine_params[] = 
{
	"Scanline",
	"Raytrace(beta)",
	NULL
};

enum Params {
	p_engine,
	p_color_major,
	p_color_shadow,
	p_color_mask,
	p_enable_outline,
	p_color_outline,
	p_lambert_color,
	p_shadow_ramp,
	p_shadow_position,
	p_casting_light,
	p_casting_occlusion,
	p_use_ramp_color,
	p_sy_aov_sytoons_beauty,
	p_sy_aov_color_major,
	p_sy_aov_color_shadow,
	p_sy_aov_color_mask,
	p_sy_aov_outline,
	p_sy_aov_dynamic_shadow,
	p_sy_aov_dynamic_shadow_raw,
	p_sy_aov_normal,
	p_sy_aov_fresnel,
	p_sy_aov_depth,
	p_sy_aov_occlusion,
};

node_parameters
{
	AiParameterEnum("engine", S_SCANLINE, engine_params);
	AiParameterRGB("color_major", 1.0f, 1.0f, 1.0f);
	AiParameterRGB("color_shadow", 0.0f, 0.0f, 0.0f);
	AiParameterRGB("color_mask", 1.0f,1.0f,1.0f);
	AiParameterBool("enable_outline", false);
	AiParameterRGB("color_outline", 0.0f, 0.0f, 0.0f);
	AiParameterRGB("lambert_color", 1.0f, 1.0f, 1.0f);
	AiParameterRGB("shadow_ramp", 0.15f, 0.15f, 0.15f);
	AiParameterFLT("shadow_position", 0.1f);
	AiParameterBool("casting_light", true);
	AiParameterBool("casting_occlusion", false);
	AiParameterBool("use_ramp_color", false);

	AiParameterStr("sy_aov_sytoons_beauty", "sy_aov_sytoons_beauty");
	AiParameterStr("sy_aov_color_major", "sy_aov_color_major");
	AiParameterStr("sy_aov_color_shadow", "sy_aov_color_shadow");
	AiParameterStr("sy_aov_color_mask", "sy_aov_color_mask");
	AiParameterStr("sy_aov_outline", "sy_aov_outline");
	AiParameterStr("sy_aov_dynamic_shadow", "sy_aov_dynamic_shadow");
	AiParameterStr("sy_aov_dynamic_shadow_raw", "sy_aov_dynamic_shadow_raw");
	AiParameterStr("sy_aov_normal", "sy_aov_normal");
	AiParameterStr("sy_aov_fresnel", "sy_aov_fresnel");
	AiParameterStr("sy_aov_depth", "sy_aov_depth");
	AiParameterStr("sy_aov_occlusion", "sy_aov_occlusion");
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
	AtColor color_outline = AiShaderEvalParamRGB(p_color_outline);
	bool enable_outline = AiShaderEvalParamBool(p_enable_outline);	
	bool casting_light = AiShaderEvalParamBool(p_casting_light);
	bool casting_occlusion = AiShaderEvalParamBool(p_casting_occlusion);
	bool use_ramp_color = AiShaderEvalParamBool(p_use_ramp_color);

	// do shading
	AtColor result = AI_RGB_BLACK;
	AtColor diffuse_raw = AI_RGB_BLACK;
	AtColor texture_result = AI_RGB_BLACK;
	AtColor lighting_result = AI_RGB_WHITE;
	AtColor shadow_result = AI_RGB_WHITE;
	AtColor shadow_raw_result = AI_RGB_WHITE;

	// texture shading
	texture_result = lerp(color_shadow,color_major,color_mask.r);

	// light shading
	switch (shading_engine)
	{
		case S_SCANLINE:
		{
			if(casting_light && sg->Rt & AI_RAY_CAMERA)
			{
				AtColor Kd = AiShaderEvalParamRGB(p_lambert_color);
				AtColor Ks = AI_RGB_WHITE;
				float Wig = 0.28;
				float roughness = 10 / 0.2;
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


				// color = accumulated light + ambient
				diffuse_raw = LaD;
				lighting_result = LaD + LaS;

				// caculate flat shadow
				float diff_t = clamp(diffuse_raw.r, 0.0f, 1.0f);
				AtColor shadow_ramp = AiShaderEvalParamRGB(p_shadow_ramp);
				float shadow_position = AiShaderEvalParamFlt(p_shadow_position);
				
				bool isMayaRamp = false;
				AtRGB diffuseLUT[LUT_SIZE];
				AtArray* diffusePositions = NULL;
				AtArray* diffuseColors = NULL;
				kt::RampInterpolationType diffuseInterp;
				// if Shadow Ramp connected with MayaRamp,we will caculate ramp color with MayaRamp value
				isMayaRamp = kt::getMayaRampArrays(node, "shadow_ramp", &diffusePositions, &diffuseColors, &diffuseInterp);
				if(isMayaRamp) // if MayaRamp connected
				{
					unsigned int* shuffle = (unsigned int*)AiShaderGlobalsQuickAlloc(sg, sizeof(unsigned int) * diffusePositions->nelements);
					kt::SortFloatIndexArray(diffusePositions, shuffle);
					kt::Ramp(diffusePositions, diffuseColors, diff_t, diffuseInterp, shadow_result, shuffle);
					shadow_raw_result = (shadow_result.r+shadow_result.g+shadow_result.b)/3;
				}
				else // if no MayaRamp connected,use defalut shadow ramp
				{
					if(diff_t >= shadow_position)
					{
						shadow_result = AI_RGB_WHITE;
						shadow_raw_result = AI_RGB_WHITE;	
					}
					else
					{
						shadow_result = shadow_ramp;
						shadow_raw_result = AI_RGB_BLACK;
					}			
				}

			} // ending if casting light

			// result
			if(use_ramp_color)
				result = texture_result*shadow_result;
			else
				result = lerp(color_shadow,texture_result,shadow_raw_result.r);
			break;
		}
		case S_RAYTRACE:
		{
			// Kd (diffuse color), Ks (specular color), and roughness (scalar)
			AtColor Kd = AiShaderEvalParamRGB(p_lambert_color);
			AtColor Ks = AI_RGB_WHITE;
			float roughness = 0.2f;
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

			// add up indirect and direct contributions
			lighting_result = Kd * (Dda + IDd) + Ks * (Dsa + IDs);
			result = lighting_result;
			break;
		}
		default:
		{
			result = AI_RGB_RED;
			break;
		}
	}

	if(enable_outline)
	{	
		// set outline aov
		result = color_outline;
		AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_outline].c_str(), color_outline);
	}
	// set flat shader aovs
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_color_major].c_str(), color_major);
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_color_shadow].c_str(), color_shadow);
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_color_mask].c_str(), color_mask);
	// set dynamic shadow aov
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_dynamic_shadow].c_str(), shadow_result);
	// set dynamic shadow raw aov
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_dynamic_shadow_raw].c_str(), shadow_raw_result);
	// set beauty aov
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_sytoons_beauty].c_str(), result);

	// caculate normal aov
	AtColor normal = AiColor(sg->N.x,sg->N.y,sg->N.z);
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_normal].c_str(), normal);
	// caculate facingratio aov
	AtColor fresnel = AiColor(1-AiV3Dot(sg->Nf, -sg->Rd));
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_fresnel].c_str(), fresnel);
	// caculate depth aov
	AtColor depth = AiColor(sg->Rl);
	// caculate occlusion aov
	AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_depth].c_str(), depth);
	if(casting_occlusion)
	{
		AtVector Nbent;
		AtColor occlusion = AI_RGB_WHITE-AiOcclusion(&sg->N, &sg->Ng, sg, 0.0f, 2000.0f, 1.0f, 0.0f, AiSampler(8,2), &Nbent);
		AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_occlusion].c_str(), occlusion);    		
	}

	sg->out.RGB = result;
}

