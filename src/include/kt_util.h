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
#pragma once
#include <vector>
#include <string>
#include <math.h>
#include <cassert>

#include <my_util.h>

#ifndef REGISTER_AOVS_CUSTOM
#define REGISTER_AOVS_CUSTOM \
data->aovs_custom.clear(); \
data->aovs_custom.push_back(params[p_aov_sytoons_beauty].STR); \
data->aovs_custom.push_back(params[p_aov_color_major].STR); \
data->aovs_custom.push_back(params[p_aov_color_shadow].STR); \
data->aovs_custom.push_back(params[p_aov_color_mask].STR); \
data->aovs_custom.push_back(params[p_aov_color_extra].STR); \
data->aovs_custom.push_back(params[p_aov_diffuse_color].STR); \
data->aovs_custom.push_back(params[p_aov_specular_color].STR); \
assert(data->aovs_custom.size() == 7 && "NUM_AOVs does not match size of aovs array!"); \
for (size_t i=0; i < data->aovs_custom.size(); ++i) \
	AiAOVRegister(data->aovs_custom[i].c_str(), AI_TYPE_RGB, AI_AOV_BLEND_OPACITY);
#endif

enum AovIndices
{
	k_aov_sytoons_beauty = 0,
	k_aov_color_major,
	k_aov_color_shadow,
	k_aov_color_mask,
	k_aov_color_extra,
	k_aov_diffuse_color,
	k_aov_specular_color
};


struct ShaderData
{
    // AOV names
    std::vector<std::string> aovs;
    std::vector<std::string> aovs_rgba;
    std::vector<std::string> aovs_custom;
};


inline void getMayaRampArrays(AtNode* node, const char* paramName, AtArray** positions, AtArray** colors, RampInterpolationType* interp)
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
            AiMsgWarning("[syToons] %s is connected but connection is not a MayaRamp", paramName);
        }
    }
}
