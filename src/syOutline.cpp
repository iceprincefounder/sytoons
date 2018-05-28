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

#include <syOutline.h>

AI_SHADER_NODE_EXPORT_METHODS(syOutlineMethods);



enum Params {
	p_color,
	p_sy_aov_outline,
	p_sy_aov_normal,
	p_sy_aov_fresnel,
	p_sy_aov_depth,
	p_enable_occlusion,
	p_sy_aov_occlusion,
};

node_parameters
{
	AiParameterRGB("color", 1.0f, 0.15f, 0.0f);
	AiParameterStr("sy_aov_outline", "sy_aov_outline");
	AiParameterStr("sy_aov_normal", "sy_aov_normal");
	AiParameterStr("sy_aov_fresnel", "sy_aov_fresnel");
	AiParameterStr("sy_aov_depth", "sy_aov_depth");
	AiParameterBool("enable_occlusion", false);
	AiParameterStr("sy_aov_occlusion", "sy_aov_occlusion");
}

node_initialize
{
	ShaderDataOutline* data = new ShaderDataOutline();
	AiNodeSetLocalData(node, data);
}

node_update
{
	ShaderDataOutline* data = (ShaderDataOutline*)AiNodeGetLocalData(node);
	// set up AOVs
	REGISTER_AOVS_CUSTOM
}

node_finish
{
	if (AiNodeGetLocalData(node))
	{
		ShaderDataOutline* data = (ShaderDataOutline*)AiNodeGetLocalData(node);
		AiNodeSetLocalData(node, NULL);
		delete data;
	}
}

shader_evaluate
{
	ShaderDataOutline* data = (ShaderDataOutline*)AiNodeGetLocalData(node);
	// we provide two shading engine,traditional scanline and GI engine raytrace.
	AtRGB color = AiShaderEvalParamRGB(p_color);
	bool enable_occlusion = AiShaderEvalParamBool(p_enable_occlusion);
	// caculate normal aov
	AtRGB normal = AtRGB(sg->N.x,sg->N.y,sg->N.z);
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_normal], normal);
	// caculate facingratio aov
	AtRGB fresnel = AtRGB(1-AiV3Dot(sg->Nf, -sg->Rd));
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_fresnel], fresnel);
	// caculate depth aov
	AtRGB depth = AtRGB(sg->Rl);
	// caculate occlusion aov
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_depth], depth);
	if(enable_occlusion)
	{

		AtVector N = sg->Nf;
		AtVector Ng = sg->Ngf;
		AtVector Ns = sg->Ns;

		float mint = 0.0f;
		float maxt = 2.0f;
		float spread = 1.0f;
		float falloff = 0.5f;
		float ndim = 2.0f;
		float nsamples = 4.0f;

		static const uint32_t seed = static_cast<uint32_t>(AiNodeEntryGetNameAtString(AiNodeGetNodeEntry(node)).hash());
		const AtSampler* sampler = AiSampler(seed, nsamples, ndim);
		AtVector Nbent = AtVector(1,1,1);

		// caculate occlusion,if falloff equal to zero,maya would crash
		if(falloff <= 0)
	    falloff = 0.001;
		AtRGB occlusion = AI_RGB_WHITE - AiOcclusion(N,Ng,sg,mint,maxt,spread,falloff,sampler,&Nbent);

		AiAOVSetRGB(sg, data->aovs[k_sy_aov_occlusion], occlusion);    		
	}
	AiAOVSetRGB(sg, data->aovs[k_sy_aov_outline], color);

	sg->out.RGB() = color;
}

