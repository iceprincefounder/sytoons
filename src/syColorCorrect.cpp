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

#include <shader_utils.h>

AI_SHADER_NODE_EXPORT_METHODS(syColorCorrectMethods);

#define COLOR_SPACE_RGB 0
#define COLOR_SPACE_XYZ 1
#define COLOR_SPACE_XYY 2
#define COLOR_SPACE_HSL 3
#define COLOR_SPACE_HSV 4

inline AtColor XYZToxyY(const AtColor& xyz)
{
   AtColor result;
   float sum = xyz.r + xyz.g + xyz.b;
   if (sum > 0.00001f)
      AiColorCreate(result, xyz.r / sum, xyz.g / sum, xyz.g);
   else
      result = AI_RGB_BLACK;
   return result;
}


inline AtColor xyYToXYZ(const AtColor& xyY)
{
   AtColor result;
   AiColorCreate(result,
                 xyY.b * xyY.r / xyY.g,
                 xyY.b,
                 xyY.b * (1.0f - xyY.r - xyY.g) / xyY.g);
   return result;
}

AtColor convertFromRGB(const AtColor& color, int to_space)
{
   if (to_space == COLOR_SPACE_HSL || to_space == COLOR_SPACE_HSV)
   {
      float cmax = AiColorMaxRGB(color);
      float cmin = std::min(std::min(color.r, color.g), color.b);
      float chroma = cmax - cmin;
      
      float hue;
      if (chroma == 0.0f)
         hue = 0.0f;
      else if (cmax == color.r)
         hue = (color.g - color.b) / chroma;
      else if (cmax == color.g)
         hue = (color.b - color.r) / chroma + 2.0f;
      else
         hue = (color.r - color.g) / chroma + 4.0f;
      hue *= 1.0f / 6.0f;
      if (hue < 0.0f)
         hue += 1.0f;
      
      if (to_space == COLOR_SPACE_HSL)
      {
         float lightness = (cmax + cmin) * 0.5f;
         float saturation;
         if (chroma == 0.0f)
            saturation = 0.0f;
         else
            saturation = chroma / (1.0f - fabsf(2.0f * lightness - 1.0f));
         AtColor result = { hue, saturation, lightness };
         return result;
      }
      else
      {
         // HSV
         float value = cmax;
         float saturation = chroma == 0.0f ? 0.0f : chroma / value;
         AtColor result = { hue, saturation, value };
         return result;
      }
   }
   else if (to_space == COLOR_SPACE_XYZ || to_space == COLOR_SPACE_XYY)
   {
      float X = (0.49f * color.r + 0.31f * color.g + 0.2f * color.b) / 0.17697f;
      float Y = (0.17697f * color.r + 0.81240f * color.g + 0.01063f * color.b) / 0.17697f;
      float Z = (0.0f * color.r + 0.01f * color.g + 0.99f * color.b) / 0.17697f;
      AtColor result;
      if (to_space == COLOR_SPACE_XYZ)
         AiColorCreate(result, X, Y, Z);
      else // xyY
      {
         float sum = X + Y + Z;
         if (sum > 0.00001f)
            AiColorCreate(result, X / sum, Y / sum, Y);
         else
            result = AI_RGB_BLACK;
      }
      return result;
   }
   
   // was RGB already (or unknown color space)
   return color;
}

AtColor convertToRGB(const AtColor& color, int from_space)
{
   if (from_space == COLOR_SPACE_HSV || from_space == COLOR_SPACE_HSL)
   {
      float hue6 = color.r * 6.0f;
      float hue2 = hue6;
      if (hue6 > 4.0f)      hue2 -= 4.0f;
      else if (hue6 > 2.0f) hue2 -= 2.0f;
      
      float chroma;
      if (from_space == COLOR_SPACE_HSV)
         chroma = color.g * color.b;
      else // HSL
         chroma = (1.0f - fabsf(2.0f * color.b - 1.0f)) * color.g;
      
      float component = chroma * (1.0f - fabsf(hue2 - 1.0f));
      
      AtColor rgb;
      if (hue6 <= 0.0f) rgb = AI_RGB_BLACK;
      else if (hue6 < 1) AiColorCreate(rgb, chroma, component, 0.0f);
      else if (hue6 < 2) AiColorCreate(rgb, component, chroma, 0.0f);
      else if (hue6 < 3) AiColorCreate(rgb, 0.0f, chroma, component);
      else if (hue6 < 4) AiColorCreate(rgb, 0.0f, component, chroma);
      else if (hue6 < 5) AiColorCreate(rgb, component, 0.0f, chroma);
      else if (hue6 <= 6) AiColorCreate(rgb, chroma, 0.0f, component);
      
      float cmin;
      if (from_space == COLOR_SPACE_HSV)
         cmin = color.b - chroma;
      else // HSL
         cmin = color.b - chroma * 0.5f;
      rgb += AiColor(cmin);
      return rgb;
   }
   else if (from_space == COLOR_SPACE_XYZ || from_space == COLOR_SPACE_XYY)
   {
      // for documentation purposes, CIE->RGB needs color system data, and here
      // are some typical bits that are needed:
      //
      // Name               xRed     yRed     xGreen   yGreen   xBlue    yBlue    White point                Gamma
      // -----------------------------------------------------------------------------------------------------------------------
      // "NTSC",            0.67f,   0.33f,   0.21f,   0.71f,   0.14f,   0.08f,   WhitePoint::IlluminantC,   GAMMA_REC601
      // "EBU (PAL/SECAM)", 0.64f,   0.33f,   0.29f,   0.60f,   0.15f,   0.06f,   WhitePoint::IlluminantD65, GAMMA_REC709
      // "SMPTE",           0.630f,  0.340f,  0.310f,  0.595f,  0.155f,  0.070f,  WhitePoint::IlluminantD65, GAMMA_REC709
      // "HDTV",            0.670f,  0.330f,  0.210f,  0.710f,  0.150f,  0.060f,  WhitePoint::IlluminantD65, GAMMA_REC709 (2.35)
      // "sRGB",            0.670f,  0.330f,  0.210f,  0.710f,  0.150f,  0.060f,  WhitePoint::IlluminantD65, 2.2
      // "CIE",             0.7355f, 0.2645f, 0.2658f, 0.7243f, 0.1669f, 0.0085f, WhitePoint::IlluminantE,   GAMMA_REC709
      // "CIE REC 709",     0.64f,   0.33f,   0.30f,   0.60f,   0.15f,   0.06f,   WhitePoint::IlluminantD65, GAMMA_REC709
      //
      // typical white points are as follows:
      //
      // Name          x            y              Description
      // -----------------------------------------------------------------------------
      // IlluminantA   0.44757f,    0.40745f    Incandescent tungsten
      // IlluminantB   0.34842f,    0.35161f    Obsolete, direct sunlight at noon
      // IlluminantC   0.31006f,    0.31616f    Obsolete, north sky daylight
      // IlluminantD50 0.34567f,    0.35850f    Some print, cameras
      // IlluminantD55 0.33242f,    0.34743f    Some print, cameras
      // IlluminantD65 0.31271f,    0.32902f    For EBU and SMPTE, HDTV, sRGB
      // IlluminantD75 0.29902f,    0.31485f    ???
      // IlluminantE   0.33333333f, 0.33333333f CIE equal-energy illuminant
      // Illuminant93K 0.28480f,    0.29320f    High-efficiency blue phosphor monitors
      // IlluminantF2  0.37207f,    0.37512f    Cool white flourescent (CWF)
      // IlluminantF7  0.31285f,    0.32918f    Broad-band daylight flourescent
      // IlluminantF11 0.38054f,    0.37691f    Narrow-band white flourescent
      
      // we use the CIE equal-energy color space, as it is the most generic
      
      float xr, yr, zr, xg, yg, zg, xb, yb, zb;
      float xw, yw, zw;
      float rx, ry, rz, gx, gy, gz, bx, by, bz;
      float rw, gw, bw;
      
      float xc, yc, zc;
      if (from_space == COLOR_SPACE_XYZ)
      {
         xc = color.r;
         yc = color.g;
         zc = color.b;
      }
      else
      {
         AtColor xyz = xyYToXYZ(color);
         xc = xyz.r;
         yc = xyz.g;
         zc = xyz.b;
      }

      // these come from the tables above, using CIE equal-energy color space
      
      xr = 0.7355f; yr = 0.2654f; zr = 1.0f - (xr + yr);
      xg = 0.2658f; yg = 0.7243f; zg = 1.0f - (xg + yg);
      xb = 0.1669f; yb = 0.0085f; zb = 1.0f - (xb + yb);
      
      xw = 1.0f / 3.0f; yw = 1.0f / 3.0f; zw = 1.0f - (xw + yw);

      // xyz -> rgb matrix, before scaling to white

      rx = (yg * zb) - (yb * zg);  ry = (xb * zg) - (xg * zb);  rz = (xg * yb) - (xb * yg);
      gx = (yb * zr) - (yr * zb);  gy = (xr * zb) - (xb * zr);  gz = (xb * yr) - (xr * yb);
      bx = (yr * zg) - (yg * zr);  by = (xg * zr) - (xr * zg);  bz = (xr * yg) - (xg * yr);

      // white scaling factors; dividing by yw scales the white luminance to unity, as conventional

      rw = ((rx * xw) + (ry * yw) + (rz * zw)) / yw;
      gw = ((gx * xw) + (gy * yw) + (gz * zw)) / yw;
      bw = ((bx * xw) + (by * yw) + (bz * zw)) / yw;

      // xyz -> rgb matrix, correctly scaled to white

      rx = rx / rw;  ry = ry / rw;  rz = rz / rw;
      gx = gx / gw;  gy = gy / gw;  gz = gz / gw;
      bx = bx / bw;  by = by / bw;  bz = bz / bw;

      // rgb of the desired point

      float r = (rx * xc) + (ry * yc) + (rz * zc);
      float g = (gx * xc) + (gy * yc) + (gz * zc);
      float b = (bx * xc) + (by * yc) + (bz * zc);
      
      AtColor result = { r, g, b };
      return result;
   }
   
   // was RGB already (or unknown color space)
   return color;
}


enum Params
{
   p_input,
   p_hue_shift,
   p_saturation,
   p_brighten,
   p_contrast,
   p_contrast_pivot,
   p_exposure,
   p_gain,
   p_gamma,
   p_mask,
};

node_parameters
{
   AiParameterRGB("input",          0.0f, 0.0f, 0.0f);
   AiParameterFlt("hue_shift",      0.0f);
   AiParameterFlt("saturation",     1.0f);
   AiParameterFlt("brighten",       0.0f);
   AiParameterFlt("contrast",       1.0f);
   AiParameterFlt("contrast_pivot", 0.5f);
   AiParameterFlt("exposure",       0.0f);
   AiParameterFlt("gain",           1.0f);
   AiParameterFlt("gamma",          1.0f);
   AiParameterFLT("mask", 1.0f);
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
   AtColor input = AiShaderEvalParamRGB(p_input);
   float hue_shift = AiShaderEvalParamFlt(p_hue_shift);
   float saturation = AiShaderEvalParamFlt(p_saturation);
   float brighten = AiShaderEvalParamFlt(p_brighten);
   float contrast = AiShaderEvalParamFlt(p_contrast);
   float exposure = AiShaderEvalParamFlt(p_exposure);
   float gain = AiShaderEvalParamFlt(p_gain);
   float gamma = AiShaderEvalParamFlt(p_gamma);
   float mask = AiShaderEvalParamFlt(p_mask);
   // apply hue and saturation changes (but only if the parameters are
   // non-identity; the conversion to/from HSL isn't amazingly cheap)
   AtColor result = AI_RGB_BLACK;
   if (mask > 0.0f)
   {
	   if (hue_shift != 0.0f || saturation != 1.0f)
	   {
	      AtColor hsl = convertFromRGB(input, COLOR_SPACE_HSL);
	      
	      // apply hue, keep it in 0..1
	      hsl.r += hue_shift;
	      hsl.r = hsl.r - floorf(hsl.r);
	      
	      // apply saturation
	      hsl.g *= saturation;
	      result = convertToRGB(hsl, COLOR_SPACE_HSL);
	   }
	   
	   // apply contrast
	   if (contrast != 1.0f)
	   {
	      AtColor contrast_pivot = AiColor(AiShaderEvalParamFlt(p_contrast_pivot));
	      result = (result - contrast_pivot) * contrast + contrast_pivot;
	   }
	   
	   if (brighten != 0.0f)
	      result += brighten;
	   
	   if (exposure != 0.0f)
	      result *= powf(2.0f, exposure);

	   if (gain != 1.0f)
	      result *= gain;
	   
	   if (gamma != 1.0f)
	      AiColorGamma(&result, gamma);

		// mask
		if (mask < 1.0f)
		{
			result = lerp(input, result, mask);
		}

   }

   sg->out.RGB = result;
}