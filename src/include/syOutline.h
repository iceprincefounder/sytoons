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
#include <ai.h>
#include <vector>
#include <string>
#include <math.h>
#include <cassert>

#define NUM_AOVs 5

#ifndef REGISTER_AOVS_CUSTOM
#define REGISTER_AOVS_CUSTOM                                                  \
    data->aovs.clear();                                                \
    data->aovs.push_back(AiNodeGetStr(node, "sy_aov_outline"));        \
    data->aovs.push_back(AiNodeGetStr(node, "sy_aov_normal"));         \
    data->aovs.push_back(AiNodeGetStr(node, "sy_aov_fresnel"));        \
    data->aovs.push_back(AiNodeGetStr(node, "sy_aov_depth"));          \
    data->aovs.push_back(AiNodeGetStr(node, "sy_aov_occlusion"));      \
    assert(NUM_AOVs == data->aovs.size() &&                            \
           "NUM_AOVs does not match size of aovs array!");             \
    for (size_t i = 0; i < data->aovs.size(); ++i)                     \
        AiAOVRegister(data->aovs[i].c_str(), AI_TYPE_RGB,              \
                      AI_AOV_BLEND_OPACITY);                           

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
    std::vector<AtString> aovs;
    std::vector<AtString> aovs_rgba;
    std::vector<AtString> aovs_custom;
};