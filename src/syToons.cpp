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
#include <stdio.h>

#include <al_util.h>
#include <syToons.h>


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
	p_threshold,
	p_lambert_color,
	p_shadow_ramp,
	p_shadow_mask,
	p_shadow_position,
	p_normal,
	p_opacity,
	p_casting_light,
	p_enable_occlusion,
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
	AiParameterFlt("threshold", 0.1f);
	AiParameterRGB("lambert_color", 1.0f, 1.0f, 1.0f);
	AiParameterRGB("shadow_ramp", 0.15f, 0.15f, 0.15f);
	AiParameterRGB("shadow_mask", 1.0f, 1.0f, 1.0f);
	AiParameterFlt("shadow_position", 0.1f);
	AiParameterVec("normal", 1.0f, 1.0f, 1.0f);
	AiParameterRGB("opacity", 1.0f, 1.0f, 1.0f);
	AiParameterBool("casting_light", true);
	AiParameterBool("enable_occlusion", false);
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
	ShaderDataToons* data = new ShaderDataToons();
	data->hasChainedNormal = false;
	AiNodeSetLocalData(node, data);
}

node_update
{
	ShaderDataToons* data = (ShaderDataToons*)AiNodeGetLocalData(node);
	data->hasChainedNormal = AiNodeIsLinked(node, "normal");
	// set up AOVs
	REGISTER_AOVS_CUSTOM

    data->aovs.push_back(AiNodeGetStr(node, "sy_outline_uv"));
    data->aovs.push_back(AiNodeGetStr(node, "sy_outline_ptr"));
	AiAOVRegister("sy_outline_uv" , AI_TYPE_VECTOR2 );
	AiAOVRegister("sy_outline_ptr" , AI_TYPE_POINTER);
}

node_finish
{
	if (AiNodeGetLocalData(node))
	{
		ShaderDataToons* data = (ShaderDataToons*)AiNodeGetLocalData(node);
		AiNodeSetLocalData(node, NULL);
		delete data;
	}
}

shader_evaluate
{
	ShaderDataToons* data = (ShaderDataToons*)AiNodeGetLocalData(node);
	// we provide two shading engine,traditional scanline and GI engine raytrace.
	int shading_engine = AiShaderEvalParamInt(p_engine);

	AtRGB color_major = AiShaderEvalParamRGB(p_color_major);
	AtRGB color_shadow = AiShaderEvalParamRGB(p_color_shadow);
	AtRGB color_mask = AiShaderEvalParamRGB(p_color_mask);

	bool enable_outline = AiShaderEvalParamBool(p_enable_outline);	
	AtRGB color_outline = AiShaderEvalParamRGB(p_color_outline);
	float threshold = AiShaderEvalParamFlt(p_threshold);

	AtRGB lambert_color = AiShaderEvalParamRGB(p_lambert_color);
	AtRGB shadow_ramp = AiShaderEvalParamRGB(p_shadow_ramp);
	AtRGB shadow_mask = AiShaderEvalParamRGB(p_shadow_mask);
	float shadow_position = AiShaderEvalParamFlt(p_shadow_position);
	
	bool casting_light = AiShaderEvalParamBool(p_casting_light);
	bool enable_occlusion = AiShaderEvalParamBool(p_enable_occlusion);
	bool use_ramp_color = AiShaderEvalParamBool(p_use_ramp_color);
	// do shading
	AtRGB result = AI_RGB_ZERO;
	AtRGB result_opacity = AiShaderEvalParamRGB(p_opacity);
	AtRGB diffuse_raw = AI_RGB_ZERO;
	AtRGB texture_result = AI_RGB_ZERO;
	AtRGB lighting_result = AI_RGB_WHITE;
	AtRGB shadow_result = AI_RGB_WHITE;
	AtRGB shadow_raw_result = AI_RGB_WHITE;

	if (data->hasChainedNormal)
	{
		sg->Nf = sg->N = AiShaderEvalParamVec(p_normal);
	}

	// texture shading
	texture_result = lerp(color_shadow,color_major,color_mask.r);

	// light shading
	switch (shading_engine)
	{
		case S_SCANLINE:
		{
			if(casting_light && sg->Rt & AI_RAY_CAMERA)
			{
				AtRGB Kd = lambert_color;
				AtRGB Ks = AI_RGB_WHITE;
				float Wig = 0.28;
				float roughness = 10 / 0.2;
				AiLightsPrepare(sg);
				AtRGB LaD = AI_RGB_ZERO; // initialize light accumulator to = 0
				AtRGB LaS = AI_RGB_ZERO; // initialize light accumulator to = 0
			 	AtLightSample ls; 
				while (AiLightsGetSample(sg, ls)) // loop over the lights
				{
					float LdotN = AiV3Dot(ls.Ld, sg->Nf);
					if (LdotN < 0) LdotN = 0;
					AtVector H = AiV3Normalize(-sg->Rd + ls.Ld);
					float spec = AiV3Dot(sg->Nf, H); // N dot H
					if (spec < 0) spec = 0;
					// Lambertian diffuse
					LaD += ls.Li * Wig * (1.0f / ls.pdf) * LdotN * Kd;
					// Blinn-Phong specular
					LaS += ls.Li * Wig * (1.0f / ls.pdf) * pow(spec, roughness) * Ks;
				}


				// color = accumulated light + ambient
				diffuse_raw = LaD;
				lighting_result = LaD + LaS;

				// caculate flat shadow
				float diff_t = clamp(diffuse_raw.r, 0.0f, 1.0f);				
				bool isMayaRamp = false;
				AtRGB diffuseLUT[LUT_SIZE];
				AtArray* diffusePositions = NULL;
				AtArray* diffuseColors = NULL;
				kt::RampInterpolationType diffuseInterp;
				// if Shadow Ramp connected with MayaRamp,we will caculate ramp color with MayaRamp value
				isMayaRamp = kt::getMayaRampArrays(node, "shadow_ramp", &diffusePositions, &diffuseColors, &diffuseInterp);
				if(isMayaRamp) // if MayaRamp connected
				{
					unsigned int* shuffle = (unsigned int*)AiShaderGlobalsQuickAlloc(sg, sizeof(unsigned int) * AiArrayGetNumElements(diffusePositions));
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
						shadow_raw_result = AI_RGB_ZERO;
					}			
				}

			} // ending if

			// result
			if(use_ramp_color)
				result = lerp(texture_result,texture_result*shadow_result,shadow_mask.r);
			else
				result = lerp(texture_result,lerp(color_shadow,texture_result,shadow_raw_result.r),shadow_mask.r);
			break;
		}
		case S_RAYTRACE:
		{
			// // Kd (diffuse color), Ks (specular color), and roughness (scalar)
			// AtRGB Kd = AiShaderEvalParamRGB(p_lambert_color);
			// AtRGB Ks = AI_RGB_WHITE;
			// float roughness = 0.2f;
			// float specbalance = 0.1f; //hide parameter specbalance until we need ray-tracing render

			// // direct specular and diffuse accumulators, 
			// // and indirect diffuse and specular accumulators...
			// AtRGB Dsa,Dda,IDs,IDd;
			// Dsa = Dda = IDs = IDd = AI_RGB_ZERO;
			// void *spec_data = AiWardDuerMISCreateData(sg, NULL, NULL, roughness, roughness);
			// void *diff_data = AiOrenNayarMISCreateData(sg, 0.0f);
			// AiLightsPrepare(sg);
			// while (AiLightsGetSample(sg)) // loop over the lights to compute direct effects
			// {
			// 	// direct specular
			// 	if (AiLightGetAffectSpecular(sg->Lp))
			// 		Dsa += AiEvaluateLightSample(sg, spec_data, AiWardDuerMISSample, AiWardDuerMISBRDF, AiWardDuerMISPDF) * specbalance;
			// 	// direct diffuse
			// 	if (AiLightGetAffectDiffuse(sg->Lp))
			// 		Dda += AiEvaluateLightSample(sg, diff_data, AiOrenNayarMISSample, AiOrenNayarMISBRDF, AiOrenNayarMISPDF) * (1-specbalance);
			// }
			// // indirect specular
			// IDs = AiWardDuerIntegrate(&sg->Nf, sg, &sg->dPdu, &sg->dPdv, roughness, roughness) * specbalance;
			// // indirect diffuse
			// IDd = AiOrenNayarIntegrate(&sg->Nf, sg, 0.0f) * (1-specbalance);

			// // add up indirect and direct contributions
			// lighting_result = Kd * (Dda + IDd) + Ks * (Dsa + IDs);
			// result = lighting_result;
			// break;
			result = AI_RGB_ZERO;
		}
		default:
		{
			result = AI_RGB_RED;
			break;
		}
	}

	// set textures aovs
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_color_major], color_major);
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_color_shadow], color_shadow);
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_color_mask], color_mask);

	// set dynamic shadow aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_dynamic_shadow], shadow_result);
	// set dynamic shadow raw aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_dynamic_shadow_raw], shadow_raw_result);
	// set beauty aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_sytoons_beauty], result);

	// caculate normal aov
	AtRGB normal = AtRGB (sg->N.x,sg->N.y,sg->N.z);
	// set normal aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_normal], normal);

	// caculate facing ratio aov
	AtRGB fresnel = AtRGB (1-AiV3Dot(sg->Nf, -sg->Rd));
	// set facing ratio aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_fresnel], fresnel);

	AtVector p[3];
	AiShaderGlobalsGetTriangle(sg, 1, p);
	for (unsigned int i =0; i<3; i++)
	{
		p[3];
		// printf("$%f %f %f\n", p[i].x,p[i].y,p[i].z);
	}


	////////////////////////////
	//Construct the contour ray.
	AtNode* option = AiUniverseGetOptions();
	AiNodeGetInt(options, "AA_samples");
	// AtRay ray;
	// float samples[2];
	// AtSamplerIterator *samit = AiSamplerIterator(data->sampler, sg);

	// AtShaderGlobals hitpoint;
	// AtVector N_total = AtVector(0.f, 0.f, 0.f);
	// AtVector orig = sg->P + Nn*offset_dist;
	// int n = 0;
	// float trace_dist = data->radius*4.f;
	// float weight_total = 0.f;



	// caculate outline aov
	AtRGB outline = AI_RGB_ZERO;
	// if (AiV3Dot(sg->Ng, -sg->Rd) < threshold)
	// {
	// 	outline = color_outline;
	// 	if (enable_outline)
	// 		result = outline;
	// }
	outline = fresnel*color_outline*(1-AiWireframe(sg, threshold, false,AI_WIREFRAME_POLYGONS ));
	// set outline aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_outline], outline);

	// caculate depth aov
	AtRGB depth = AtRGB (sg->Rl);
	// set depth aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_depth], depth);

	// caculate occlusion aov
	AtRGB occlusion = AI_RGB_ZERO;
	if(enable_occlusion)
	{

		AtVector N = sg->Nf;
		AtVector Ng = sg->Ngf;
		AtVector Ns = sg->Ns;

		float mint = 0.0f;
		float maxt = 2000.0f;
		float spread = 1.0f;
		float falloff = 0.5f;
		float ndim = 2.0f;
		float nsamples = 4.0f;

		static const uint32_t seed = static_cast<uint32_t>(AiNodeEntryGetNameAtString(AiNodeGetNodeEntry(node)).hash());
		AtSampler* sampler = AiSampler(seed, nsamples, ndim);
		AtVector Nbent = AtVector(1,1,1);

		// caculate occlusion,if falloff equal to zero,maya would crash
		if(falloff <= 0)
		    falloff = 0.001;
		occlusion = AI_RGB_WHITE - AiOcclusion(N,Ng,sg,mint,maxt,spread,falloff,sampler,&Nbent);
	}
	// set OCC aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_occlusion], occlusion);    		

	// sg->out.RGB() = result;
	// sg->out_opacity = result_opacity;
	// new, opacity must be premultiplied into other closures
	AtClosureList closures;
	closures.add(AiClosureEmission(sg, result));
	closures *= result_opacity;
	closures.add(AiClosureTransparent(sg, 1 - result_opacity));
	sg->out.CLOSURE() = closures;

}