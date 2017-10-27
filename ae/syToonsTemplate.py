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
		self.params["color_extra"] = Param("color_extra", "Extra Color", "", "rgb", presets=None)

		self.params["lambert_color"] = Param("lambert_color", "Lambert Color", "", "rgb", presets=None)
		self.params["shadow_ramp"] = Param("shadow_ramp", "Shadow Ramp", "", "rgb", presets=None)
		self.params["shadow_position"] = Param("shadow_position", "Shadow Position", "", "float", presets=None)

		self.params["casting_light"] = Param("casting_light", "Casting Light ", "", "bool", presets=None)
		self.params["casting_occlusion"] = Param("casting_occlusion", "Casting Occlusion", "", "bool", presets=None)

		self.params["aov_sytoons_beauty"] = Param("aov_sytoons_beauty", "Aov syToons Beauty", "", "rgb", presets=None)
		self.params["aov_color_major"] = Param("aov_color_major", "Aov Color Major", "", "rgb", presets=None)
		self.params["aov_color_shadow"] = Param("aov_color_shadow", "Aov Color Shadow", "", "rgb", presets=None)
		self.params["aov_color_mask"] = Param("aov_color_mask", "Aov Color Mask", "", "rgb", presets=None)
		self.params["aov_color_extra"] = Param("aov_color_extra", "Aov Color Extra", "", "rgb", presets=None)
		self.params["aov_diffuse_color"] = Param("aov_diffuse_color", "Aov Diffuse Color", "", "rgb", presets=None)
		self.params["aov_specular_color"] = Param("aov_specular_color", "Aov Specular Color", "", "rgb", presets=None)

		self.addSwatch()
		self.beginScrollLayout()
		self.addControl("engine", label="Engine", annotation="")

		self.beginLayout("Texture Shading", collapse=False)
		self.addControl("color_major", label="Major Color", annotation="")
		self.addControl("color_shadow", label="Shadow Color", annotation="")
		self.addControl("color_mask", label="Mask Color", annotation="")
		self.addControl("color_extra", label="Extra Color", annotation="")
		self.endLayout() 

		self.beginLayout("Light Shading", collapse=False)
		self.addControl("lambert_color", label="Diffuse Color", annotation="")
		self.addControl("shadow_ramp", label="Specular Color", annotation="")
		self.addControl("shadow_position", label="Roughness", annotation="")
		self.endLayout()

		self.addControl("casting_light", label="Casting Light", annotation="")
		self.addControl("casting_occlusion", label="Casting Occlusion", annotation="")

		self.beginLayout("AOVs", collapse=True)
		self.addControl("aov_sytoons_beauty", label="Aov syToons Beauty", annotation="")
		self.addControl("aov_color_major", label="Aov Color Major", annotation="")
		self.addControl("aov_color_shadow", label="Aov Color Shadow", annotation="")
		self.addControl("aov_color_mask", label="Aov Color Mask", annotation="")
		self.addControl("aov_color_extra", label="Aov Color Extra", annotation="")
		self.addControl("aov_diffuse_color", label="Aov Diffuse Color", annotation="")
		self.addControl("aov_specular_color", label="Aov Specular Color", annotation="")
		self.endLayout() # END AOVs

		pm.mel.AEdependNodeTemplate(self.nodeName)
		self.addExtraControls()

		self.endScrollLayout()
