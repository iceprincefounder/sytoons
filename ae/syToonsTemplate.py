import pymel.core as pm
from syShaders import *

class AEsyToonsTemplate(syShadersTemplate):
	controls = {}
	params = {}
	def setup(self):
		self.params.clear()
		self.params["engine"] = Param("engine", "Engine", "", "enum", presets=None)

		self.params["color_major"] = Param("color_major", "Major Color", "", "rgb", presets=None)
		self.params["color_shadow"] = Param("color_shadow", "Shadow Color", "", "rgb", presets=None)
		self.params["color_mask"] = Param("color_mask", "Mask Color", "", "rgb", presets=None)
		self.params["enable_outline"] = Param("enable_outline", "Enable Outline", "", "bool", presets=None)
		self.params["color_outline"] = Param("color_outline", "Outline Color", "", "rgb", presets=None)

		self.params["lambert_color"] = Param("lambert_color", "Lambert Color", "", "rgb", presets=None)
		self.params["shadow_ramp"] = Param("shadow_ramp", "Shadow Ramp", "", "rgb", presets=None)
		self.params["shadow_position"] = Param("shadow_position", "Shadow Position", "", "float", presets=None)

		self.params["casting_light"] = Param("casting_light", "Casting Light ", "", "bool", presets=None)
		self.params["casting_occlusion"] = Param("casting_occlusion", "Casting Occlusion", "", "bool", presets=None)

		self.params["sy_aov_sytoons_beauty"] = Param("sy_aov_sytoons_beauty", "Aov syToons Beauty", "", "rgb", presets=None)
		self.params["sy_aov_color_major"] = Param("sy_aov_color_major", "Aov Color Major", "", "rgb", presets=None)
		self.params["sy_aov_color_shadow"] = Param("sy_aov_color_shadow", "Aov Color Shadow", "", "rgb", presets=None)
		self.params["sy_aov_color_mask"] = Param("sy_aov_color_mask", "Aov Color Mask", "", "rgb", presets=None)
		self.params["sy_aov_outline"] = Param("sy_aov_outline", "Aov Outline", "", "rgb", presets=None)
		self.params["sy_aov_dynamic_shadow"] = Param("sy_aov_dynamic_shadow", "Aov Dynamic Shadow", "", "rgb", presets=None)
		self.params["sy_aov_dynamic_shadow_raw"] = Param("sy_aov_dynamic_shadow_raw", "Aov Dynamic Shadow Raw", "", "rgb", presets=None)
		self.params["sy_aov_normal"] = Param("sy_aov_normal", "Aov Normal", "", "rgb", presets=None)
		self.params["sy_aov_fresnel"] = Param("sy_aov_fresnel", "Aov Fresnel", "", "rgb", presets=None)
		self.params["sy_aov_depth"] = Param("sy_aov_depth", "Aov Depth", "", "rgb", presets=None)
		self.params["sy_aov_occlusion"] = Param("sy_aov_occlusion", "Aov Occlusion", "", "rgb", presets=None)

		self.addSwatch()
		self.beginScrollLayout()
		self.addControl("engine", label="Engine", annotation="")

		self.beginLayout("Texture Shading", collapse=False)
		self.addControl("color_major", label="Major Color", annotation="")
		self.addControl("color_shadow", label="Shadow Color", annotation="")
		self.addControl("color_mask", label="Mask Color", annotation="")
		self.endLayout() 

		self.beginLayout("Outline Shading", collapse=False)
		self.addControl("enable_outline", label="Enable Outline", annotation="")
		self.addControl("color_outline", label="Outline Color", annotation="")
		self.endLayout() 

		self.beginLayout("Light Shading", collapse=False)
		self.addControl("lambert_color", label="Lambert Color", annotation="")
		self.addControl("shadow_ramp", label="Shadow Ramp", annotation="")
		self.addCustomFlt("shadow_position")
		self.endLayout()

		self.beginLayout("Optimization", collapse=False)
		self.addControl("casting_light", label="Casting Light", annotation="")
		self.addControl("casting_occlusion", label="Casting Occlusion", annotation="")
		self.endLayout()

		self.beginLayout("AOVs", collapse=True)
		self.addControl("sy_aov_sytoons_beauty", label="Aov syToons Beauty", annotation="")
		self.addControl("sy_aov_color_major", label="Aov Color Major", annotation="")
		self.addControl("sy_aov_color_shadow", label="Aov Color Shadow", annotation="")
		self.addControl("sy_aov_color_mask", label="Aov Color Mask", annotation="")
		self.addControl("sy_aov_outline", label="Aov Outline", annotation="")
		self.addControl("sy_aov_dynamic_shadow", label="Aov Dynamic Shadow", annotation="")
		self.addControl("sy_aov_dynamic_shadow_raw", label="Aov Dynamic Shadow Raw", annotation="")
		self.addControl("sy_aov_normal", label="Aov Normal", annotation="")
		self.addControl("sy_aov_fresnel", label="Aov Fresnel", annotation="")
		self.addControl("sy_aov_depth", label="Aov Depth", annotation="")
		self.addControl("sy_aov_occlusion", label="Aov Occlusion", annotation="")
		self.endLayout() # END AOVs

		pm.mel.AEdependNodeTemplate(self.nodeName)
		self.addExtraControls()

		self.endScrollLayout()
