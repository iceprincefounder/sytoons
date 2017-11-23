import pymel.core as pm
from syShaders import *

class AEsyOutlineTemplate(syShadersTemplate):
	controls = {}
	params = {}
	def setup(self):
		self.params.clear()
		self.params["color"] = Param("color", "Color", "", "rgb", presets=None)

		self.params["sy_aov_outline"] = Param("sy_aov_outline", "Aov Outline", "", "rgb", presets=None)
		self.params["sy_aov_normal"] = Param("sy_aov_normal", "Aov Normal", "", "rgb", presets=None)
		self.params["sy_aov_fresnel"] = Param("sy_aov_fresnel", "Aov Fresnel", "", "rgb", presets=None)
		self.params["sy_aov_depth"] = Param("sy_aov_depth", "Aov Depth", "", "rgb", presets=None)
		self.params["enable_occlusion"] = Param("enable_occlusion", "Enable Occlusion", "", "bool", presets=None)
		self.params["sy_aov_occlusion"] = Param("sy_aov_occlusion", "Aov Occlusion", "", "rgb", presets=None)

		self.addSwatch()
		self.beginScrollLayout()
		self.beginLayout("Outline", collapse=False)
		self.addControl("color", label="Color", annotation="")
		self.endLayout() # END Outline
		self.beginScrollLayout()
		self.beginLayout("Optimization", collapse=False)
		self.addControl("enable_occlusion", label="Enable Occlusion", annotation="")
		self.endLayout() # END Outline

		self.beginLayout("AOVs", collapse=False)
		self.addControl("sy_aov_outline", label="Aov Outline", annotation="")
		self.addControl("sy_aov_normal", label="Aov Normal", annotation="")
		self.addControl("sy_aov_fresnel", label="Aov Fresnel", annotation="")
		self.addControl("sy_aov_depth", label="Aov Depth", annotation="")
		self.addControl("sy_aov_occlusion", label="Aov Occlusion", annotation="")
		self.endLayout() # END AOVs
		
		pm.mel.AEdependNodeTemplate(self.nodeName)
		self.addExtraControls()

		self.endScrollLayout()
