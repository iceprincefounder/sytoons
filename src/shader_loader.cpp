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

#include <iostream>
#include <cstring>

// extern AtNodeMethods* syToonsMethods;
// extern AtNodeMethods* syRampMethods;
// extern AtNodeMethods* syRemapMethods;
extern const AtNodeMethods  *syOutlineMethods;

enum SHADERS
{
   // syToons,
   // syRamp,
   // syRemap,
   syOutline,
};

node_loader
{
   switch (i) 
   {     
      // case syToons:
      //    node->methods     = (AtNodeMethods*) syToonsMethods;
      //    node->output_type = AI_TYPE_RGB;
      //    node->name        = "syToons";
      //    node->node_type   = AI_NODE_SHADER;
      // break;
      // case syRamp:
      //    node->methods     = (AtNodeMethods*) syRampMethods;
      //    node->output_type = AI_TYPE_RGB;
      //    node->name        = "syRamp";
      //    node->node_type   = AI_NODE_SHADER;
      // break;
      // case syRemap:
      //    node->methods     = (AtNodeMethods*) syRemapMethods;
      //    node->output_type = AI_TYPE_RGB;
      //    node->name        = "syRemap";
      //    node->node_type   = AI_NODE_SHADER;
      // break;
      case syOutline:
         node->methods     = (AtNodeMethods*) syOutlineMethods;
         node->output_type = AI_TYPE_RGB;
         node->name        = "syOutline";
         node->node_type   = AI_NODE_SHADER;
      break;
      default:
         return false;      
   }

   strcpy(node->version, AI_VERSION);
   return true;
}