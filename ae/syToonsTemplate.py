import pymel.core as pm
from alShaders import *

class AEsyToonsTemplate(alShadersTemplate):
	controls = {}
	params = {}
	def setup(self):
		self.params.clear()
		self.params["engine"] = Param("engine", "Engine", "", "enum", presets=None)
		self.params["casting_light"] = Param("casting_light", "Casting Light Only", "", "bool", presets=None)
		self.params["casting_texture"] = Param("casting_texture", "Casting Texture Only", "", "bool", presets=None)

		self.params["color_major"] = Param("color_major", "Major Color", "", "rgb", presets=None)
		self.params["color_shadow"] = Param("color_shadow", "Shadow Color", "", "rgb", presets=None)
		self.params["color_mask"] = Param("color_mask", "Mask Color", "", "rgb", presets=None)
		self.params["color_extra"] = Param("color_extra", "Extra Color", "", "rgb", presets=None)

		self.params["diffuse_color"] = Param("diffuse_color", "Diffuse Color", "", "rgb", presets=None)
		self.params["specular_color"] = Param("specular_color", "Specular Color", "", "rgb", presets=None)
		self.params["roughness"] = Param("roughness", "Roughness", "", "float", presets=None)

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

		self.beginLayout("Texture Shader", collapse=False)
		self.addControl("color_major", label="Major Color", annotation="")
		self.addControl("color_shadow", label="Shadow Color", annotation="")
		self.addControl("color_mask", label="Mask Color", annotation="")
		self.addControl("color_extra", label="Extra Color", annotation="")
		self.addControl("casting_texture", label="Casting Texture Only", annotation="")
		self.endLayout() 

		self.beginLayout("Lighting Shader", collapse=False)
		self.addControl("diffuse_color", label="Diffuse Color", annotation="")
		self.addControl("specular_color", label="Specular Color", annotation="")
		self.addControl("roughness", label="Roughness", annotation="")
		self.addControl("casting_light", label="Casting Light Only", annotation="")
		self.endLayout()

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
