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

#include <kt_util.h>

AI_SHADER_NODE_EXPORT_METHODS(syOutlineMethods);



enum Params {
	p_color_major,
	p_casting_occlusion,
	p_sy_aov_normal,
	p_sy_aov_fresnel,
	p_sy_aov_depth,
	p_sy_aov_occlusion,
};

node_parameters
{
	AiParameterRGB("color_major", 1.0f, 1.0f, 1.0f);
	AiParameterBool("casting_occlusion", false);
	AiParameterStr("sy_aov_normal", "sy_aov_normal");
	AiParameterStr("sy_aov_fresnel", "sy_aov_fresnel");
	AiParameterStr("sy_aov_depth", "sy_aov_depth");
	AiParameterStr("sy_aov_occlusion", "sy_aov_occlusion");
}

node_initialize
{
	ShaderData* data = new ShaderData();
	data->hasChainedNormal = false;
	AiNodeSetLocalData(node, data);
}

node_update
{
	ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
	data->hasChainedNormal = AiNodeIsLinked(node, "normal");
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
	AtColor color_major = AiShaderEvalParamRGB(p_color_major);
	bool casting_occlusion = AiShaderEvalParamBool(p_casting_occlusion);
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
		AtVector N = sg->Nf;
		AtVector Ng = sg->Ngf;
		float mint = 0.0f;
		float maxt = 2000.0f;
		float spread = 1.0f;
		float falloff = 0.5f;
		AtSampler * sampler = AiSampler(6,2);
		AtVector Nbent;
		AtColor occlusion = AI_RGB_WHITE-AiOcclusion(&N,&Ng,sg,mint,maxt,spread,falloff,sampler,&Nbent);
		AiAOVSetRGB(sg, data->aovs_custom[k_sy_aov_occlusion].c_str(), occlusion);    		
	}

	sg->out.RGB = color_major;
}

