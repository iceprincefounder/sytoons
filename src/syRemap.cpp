//
//(c) 2017-2019 Beijing ShengYing Film Animation Co.Ltd All Rights Reserved.
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
#include <iostream>

#include <al_util.h>

AI_SHADER_NODE_EXPORT_METHODS(syRemapMethods);


enum Params
{
   p_input,
   p_gamma,
   p_saturation,
   p_hueOffset,
   p_contrast,
   p_contrastPivot,
   p_gain,
   p_exposure,
   p_mask,
};

node_parameters
{
   AiParameterRGB("input", 0.18f, 0.18f, 0.18f);
   AiParameterFLT("gamma", 1.0f);
   AiParameterFLT("saturation", 1.0f);
   AiParameterFLT("hueOffset", 0.0f);
   AiParameterFLT("contrast", 1.0f);
   AiParameterFLT("contrastPivot", 0.18f);
   AiParameterFLT("gain", 1.0f);
   AiParameterFLT("exposure", 0.f);
   AiParameterFLT("mask", 1.0f);
}

node_initialize
{

}

node_finish
{

}

node_update
{

}

shader_evaluate
{
   AtRGB input = AiShaderEvalParamRGB(p_input);
   float gamma = AiShaderEvalParamFlt(p_gamma);
   float saturation = AiShaderEvalParamFlt(p_saturation);
   float hueOffset = AiShaderEvalParamFlt(p_hueOffset);
   float contrastVal = AiShaderEvalParamFlt(p_contrast);
   float contrastPivot = AiShaderEvalParamFlt(p_contrastPivot);
   float gain = AiShaderEvalParamFlt(p_gain);
   float exposure = AiShaderEvalParamFlt(p_exposure);
   float mask = AiShaderEvalParamFlt(p_mask);

   AtRGB result = input;
   if (mask > 0.0f)
   {
      // gamma
      result = pow(input, 1.0f/gamma);

      // saturation
      if (saturation != 1.0f)
      {
         float l = luminance(result);
         result = lerp(rgb(l), result, saturation);
      }

      // hue
      if (hueOffset != 0.0f)
      {
         AtRGB hsv = rgb2hsv(result);
         hsv.r += hueOffset;
         result = hsv2rgb(hsv);
      }

      // contrast
      if (contrastVal != 1.0f)
      {
         result = contrast(result, contrastVal, contrastPivot);
      }

      // gain and exposure
      result = result * powf(2.0f, exposure) * gain;

      // mask
      if (mask < 1.0f)
      {
         result = lerp(input, result, mask);
      }
   }
   sg->out.RGB = result;
}
