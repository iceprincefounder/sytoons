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

#include <cmath>
#include <string>
#include <cstring>

#include <my_util.h>

AI_SHADER_NODE_EXPORT_METHODS(syRampMethods);


enum Params
{
	p_type = 0,
	p_interp,

	p_uvCoord,

	p_positions,
	p_colors,

	p_file,
};

enum RampType
{
	RT_V = 0,
	RT_U,
};

const char* RampTypeNames[] =
{
	"v",
	"u",
	NULL
};


node_parameters
{
	AiParameterENUM("type", 1, RampTypeNames);
	AiParameterENUM("interpolation", 1, RampInterpolationNames);

	AiParameterPNT2("uvCoord", 0.0f, 0.0f);


	AiParameterARRAY("position", AiArray(0, 0, AI_TYPE_FLOAT));

	AiParameterARRAY("color", AiArray(0, 0, AI_TYPE_RGB));

	AiParameterSTR("file", "");



	//AiMetaDataSetBool(mds, NULL, "maya.hide", true);
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
		//MayaDefaultColor(sg, node, p_defaultColor, sg->out.RGBA);
		sg->out.RGB = result;
		return;
	}

	float u = uv.x;
	float v = uv.y;

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
				default:
				Ramp(positions, colors, v, interp, result, shuffle);
				break;
			}
		}
	}

	sg->out.RGB = result;

}