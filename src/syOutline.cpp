#include <ai.h>
#include <string>
#include <vector>

AI_FILTER_NODE_EXPORT_METHODS(syOutlineMethods);

using namespace std;

struct Color
{
    float r, g, b;
    
    Color()                          : r(0.0f), g(0.0f), b(0.0f)    { }
    Color(const Color& c)            : r(c.r) , g(c.g) , b(c.b)     { }
    Color(float r, float g, float b) : r(r), g(g), b(b)             { }
    explicit Color(float f)          : r(f), g(f), b(f)             { }
    Color& operator =(const Color& c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        return *this;
    }
};

struct Pixel
{
  int x, y;
  Color color;
  Pixel(int i_x, int i_y, float i_r, float i_g, float i_b)
  {
    x = i_x;
    y = i_y;
    color.r = i_r;
    color.g = i_g;
    color.b = i_b;
  }
  Pixel(int i_x, int i_y, Color i_color)
  {
    x = i_x;
    y = i_y;
    color = i_color;
  }
  void set(int i_x, int i_y, float i_r, float i_g, float i_b)
  {
    x = i_x;
    y = i_y;
    color.r = i_r;
    color.g = i_g;
    color.b = i_b;
  }
  Pixel& operator =(const Pixel& c)
  {
    x = c.x;
    y = c.y;
    color = c.color;
    return *this;
  }
};

struct PixelSet
{
   Pixel* Set[];
   void add (Pixel* pixel)
   {
      Set[pixel->x*pixel->y + pixel->y] = pixel;
   }
};

void gaussianFilter(AtAOVSampleIterator* iterator, float width, AtRGBA& resColor)
{
  int x, y;
  float weight = 0.0f;
  AtRGBA value = AI_RGBA_ZERO;
  while (AiAOVSampleIteratorGetNext(iterator))
  {
    // take into account adaptive sampling
    float inv_density = AiAOVSampleIteratorGetInvDensity(iterator);
    if (inv_density <= 0.f)
       continue;

    // determine distance to filter center
    const AtVector2& offset = AiAOVSampleIteratorGetOffset(iterator);
    const float r = AiSqr(2 / width) * (AiSqr(offset.x) + AiSqr(offset.y));
    if (r > 1.0f)
       continue;
    // gaussian filter weight
    const float gaussian_weight = AiFastExp(2 * -r) * inv_density;
    // accumulate weights and colors
    value += gaussian_weight * AiAOVSampleIteratorGetRGBA(iterator);
    weight += gaussian_weight;
    AiAOVSampleIteratorGetPixel(iterator,x ,y);
  }

  if (weight != 0.0f)
    value /= weight;
  // AiMsgInfo("@@ size %i", flag);
  // filter_data -> array[x][y] = data_out;
  resColor = value;

}

struct FilterData
{
   PixelSet* pixelSet;
   float width;
   AtRGBA pix[2048][2048];
   // void* array[];
};

node_parameters
{
   AiParameterFlt("width", 2.0f);
}

node_initialize
{
   AiFilterInitialize(node, false, NULL);
   AiNodeSetLocalData(node, new FilterData());
}

node_update
{
   FilterData* filter_data = (FilterData*)AiNodeGetLocalData(node);
   filter_data->width = AiNodeGetFlt(node, "width");
   AiFilterUpdate(node, filter_data->width);
}

node_finish
{
  FilterData* filter_data = (FilterData*)AiNodeGetLocalData(node);
  delete filter_data;
}

filter_output_type
{
   switch (input_type)
   {
      case AI_TYPE_RGBA:
         return AI_TYPE_RGBA;
      default:
         return AI_TYPE_NONE;
   }
}

filter_pixel
{
/* static void FilterPixel(AtNode* node, 
*                          AtAOVSampleIterator* 
*                          iterator, 
*                          void* data_out, 
*                          uint8_t data_type)
*/
  FilterData* filter_data = (FilterData*)AiNodeGetLocalData(node);
  const float width = filter_data->width;
  AtRGBA result = AI_RGBA_ZERO;
  gaussianFilter(iterator, width, result);
  *((AtRGBA*)data_out) = result;
}



// node_loader
// {
//    if (i>0)
//       return false;
 
//    node->methods      = syOutlineMethods;
//    node->output_type  = AI_TYPE_NONE;
//    node->name         = "syOutline";
//    node->node_type    = AI_NODE_FILTER;
//    strcpy(node->version, AI_VERSION);
//    return true;
// }