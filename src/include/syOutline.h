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


#ifndef REGISTER_AOVS_CUSTOM
#define REGISTER_AOVS_CUSTOM \
data->aovs_custom.clear(); \
data->aovs_custom.push_back(params[p_sy_aov_outline].STR); \
data->aovs_custom.push_back(params[p_sy_aov_normal].STR); \
data->aovs_custom.push_back(params[p_sy_aov_fresnel].STR); \
data->aovs_custom.push_back(params[p_sy_aov_depth].STR); \
data->aovs_custom.push_back(params[p_sy_aov_occlusion].STR); \
assert(data->aovs_custom.size() == 5 && "NUM_AOVs does not match size of aovs array!"); \
for (size_t i=0; i < data->aovs_custom.size(); ++i) \
	AiAOVRegister(data->aovs_custom[i].c_str(), AI_TYPE_RGB, AI_AOV_BLEND_OPACITY);
#endif

enum AovIndicesOutline
{
    k_sy_aov_outline = 0,
    k_sy_aov_normal,
    k_sy_aov_fresnel,
    k_sy_aov_depth,
    k_sy_aov_occlusion,
};

struct ShaderDataOutline
{
    // AOV names
    std::vector<std::string> aovs;
    std::vector<std::string> aovs_rgba;
    std::vector<std::string> aovs_custom;
};