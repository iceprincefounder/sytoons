//
// Copyright 2017 Beijing ShengYing Film Animation Co.Ltd
//
// Own by Kevin Tsui
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include <ai.h>

#include <iostream>
#include <cstring>

extern AtNodeMethods* syToonsMethods;
extern AtNodeMethods* syRampMethods;
extern AtNodeMethods* syColorCorrectMethods;

enum SHADERS
{
   syToons,
   syRamp,
   syColorCorrect,
};

node_loader
{
   switch (i) 
   {     
      case syToons:
         node->methods     = (AtNodeMethods*) syToonsMethods;
         node->output_type = AI_TYPE_RGB;
         node->name        = "syToons";
         node->node_type   = AI_NODE_SHADER;
      break;
      case syRamp:
         node->methods     = (AtNodeMethods*) syRampMethods;
         node->output_type = AI_TYPE_RGB;
         node->name        = "syRamp";
         node->node_type   = AI_NODE_SHADER;
      break;
      case syColorCorrect:
         node->methods     = (AtNodeMethods*) syToonsMethods;
         node->output_type = AI_TYPE_RGB;
         node->name        = "syColorCorrect";
         node->node_type   = AI_NODE_SHADER;
      break;
      default:
         return false;      
   }

   strcpy(node->version, AI_VERSION);
   return true;
}