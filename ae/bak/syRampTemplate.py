import pymel.core as pm
from alShaders import *

class AEsyRampTemplate(alShadersTemplate):
	controls = {}
	params = {}
	def setup(self):
		self.params.clear()
		self.params["type"] = Param("type", "Type", "", "enum", presets=None)
		self.params["interpolation"] = Param("interpolation", "Interpo", "", "enum", presets=None)

		self.params["uvCoord"] = Param("uvCoord", "UV Coord", "", "float2", presets=None)

		self.params["position"] = Param("position", "Position", "", "array", presets=None)
		self.params["color"] = Param("color", "Color", "", "array", presets=None)

		self.addSwatch()
		self.beginScrollLayout()

		self.addControl("type", label="Type", annotation="")
		self.addControl("interpolation", label="Interpo", annotation="")

		self.addControl("uvCoord", label="UVCoord", annotation="")

		self.addControl("position", label="Position", annotation="")
		self.addControl("color", label="Color", annotation="")

		pm.mel.AEdependNodeTemplate(self.nodeName)
		self.addExtraControls()

		self.endScrollLayout()
