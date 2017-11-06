#include <ai.h>
#include <cstring>

AI_SHADER_NODE_EXPORT_METHODS(SimpleMethods);

enum SimpleParams { p_color };

node_parameters
{
   AiParameterRGB("color", 0.7f, 0.7f, 0.7f);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   sg->out.RGB = AiShaderEvalParamRGB(p_color);
   sg->out_opacity = 0.0f;
}

node_loader
{
   if (i > 0)
      return false;

   node->methods     = SimpleMethods;
   node->output_type = AI_TYPE_RGB;
   node->name        = "opacity";
   node->node_type   = AI_NODE_SHADER;
   strcpy(node->version, AI_VERSION);
   return true;
}