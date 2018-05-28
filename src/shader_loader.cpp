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

extern const AtNodeMethods* syToonsMethods;
extern const AtNodeMethods* syOutlineMethods;

enum SHADERS
{
   syToons,
   syOutline,
};

node_loader
{
   switch (i) 
   {     
      case syToons:
         node->methods     = syToonsMethods;
         node->output_type = AI_TYPE_CLOSURE;
         node->name        = "syToons";
         node->node_type   = AI_NODE_SHADER;
      break;
      case syOutline:
         node->methods     = syOutlineMethods;
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