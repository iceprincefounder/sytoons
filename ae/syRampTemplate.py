import pymel.core as pm
import maya.cmds as cmds
from syShaders import *

def AEaddFloatSliderGlobalCB( plug, slider ):
	val = cmds.floatSlider( slider, q=1, v=1 )
	cmds.setAttr( plug, val )

class AEsyRampTemplate(ShaderAETemplate):
	def filenameEdit(self, mData) :
		attr = self.nodeAttr('file')
		cmds.setAttr(attr,mData,type="string")
	def LoadFilenameButtonPush(self, *args):
		basicFilter = 'All Files (*.*)'
		ret = cmds.fileDialog2(fileFilter=basicFilter, dialogStyle=2,cap='Load File',okc='Load',fm=4)
		if ret is not None and len(ret):
			self.filenameEdit(ret[0])
			cmds.textFieldButtonGrp("filenameGrp", edit=True, text=ret[0])
	def filenameNew(self, nodeName):
		path = cmds.textFieldButtonGrp("filenameGrp", label="File Name",changeCommand=self.filenameEdit, width=300)
		cmds.textFieldButtonGrp(path, edit=True, text=cmds.getAttr(nodeName))
		cmds.textFieldButtonGrp(path, edit=True, buttonLabel="...",buttonCommand=self.LoadFilenameButtonPush)
		ramp = cmds.createNode('ramp')
		cmds.rampColorPort( node=ramp )
	def filenameReplace(self, nodeName):
		cmds.textFieldButtonGrp("filenameGrp", edit=True,text=cmds.getAttr(nodeName) )
	def rampNew(self, nodeName):
		#path = cmds.textFieldButtonGrp("filenameGrp", label="File Name",changeCommand=self.filenameEdit, width=300)
		#cmds.textFieldButtonGrp(path, edit=True, text=cmds.getAttr(nodeName))
		#cmds.textFieldButtonGrp(path, edit=True, buttonLabel="...",buttonCommand=self.LoadFilenameButtonPush)
		ramp = cmds.createNode('ramp')
		cmds.rampColorPort( node=ramp )
		p_type = self.nodeAttr("type")
		p_interpolation = self.nodeAttr("interpolation")
		#cmds.connectAttr( '%s.type'%ramp,p_type )
		#cmds.connectAttr( '%s.interpolation'%ramp,p_interpolation )
		#cmds.select(nodeName)
		cmds.setAttr('%s.interpolation'%ramp,0)
		cmds.select(nodeName)
	def rampReplace(self, nodeName):
		pass
	def setup(self):
		self.addSwatch()
		self.beginScrollLayout()

		self.addControl("type", label="Type", annotation="")
		self.addControl("interpolation", label="Interpo", annotation="")


		#self.addControl("position", label="Position N", annotation="")
		#self.addControl("color", label="Color N", annotation="")
		self.addCustom('', self.rampNew,self.rampReplace)

		self.addControl("uvCoord", label="UV Coord", annotation="")

		#self.addCustom('file', self.filenameNew, self.filenameReplace)

		pm.mel.AEdependNodeTemplate(self.nodeName)
		self.addExtraControls()

		self.endScrollLayout()
