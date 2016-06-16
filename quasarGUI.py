#!/usr/bin/python
'''
quasarGUI.py

@author:     Damian Abalo Miron <damian.abalo@cern.ch>

@copyright:  2015 CERN

@license:
Copyright (c) 2015, CERN, Universidad de Oviedo.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    damian.abalo@cern.ch
'''

import time
import curses
import curses.wrapper
import os
import sys
import subprocess
import inspect
sys.path.insert(0, './FrameworkInternals')
from generateCmake import generateCmake
from automated_build import automatedBuild
from distclean import distClean
from install_framework import createProject
from install_framework import upgradeProject
from manage_files import mfCheckConsistency
from manage_files import mfCreateRelease
from manage_files import mfInstall
from manage_files import mfSetupSvnIgnore
from manage_files import mfCheckSvnIgnore
from manage_files import mfDesignVsDevice
from deviceGenerators import generateRoot
from deviceGenerators import generateBaseClass
from deviceGenerators import generateDeviceClass
from deviceGenerators import generateAllDevices
from addressSpaceGenerators import generateASClass
from addressSpaceGenerators import generateInformationModel
from addressSpaceGenerators import generateSourceVariables
from configurationGenerators import generateConfiguration
from configurationGenerators import generateConfigurator
from configurationGenerators import generateConfigValidator
from designTools import validateDesign
from designTools import formatDesign
from designTools import upgradeDesign
from designTools import createDiagram
from generateHonkyTonk import generateHonkyTonk
from runDoxygen import runDoxygen
from externalToolCheck import checkExternalDependencies
from quasarCommands import getCommandFromFunction

exitLoop = False
myScreen = None
mainMenu = None
currentSubMenu = None

g_sMenuCoordinateY = 11
g_sMenuCoordinateX = 25
g_sTitleCoordinateY = 2
g_sTitleCoordinateX = 0
class Option:	
	def __init__(self, id, description, function, closeCurses, userInput = None):
		self.id = id
		self.userInput = userInput
		self.description = description
		self.function = function
		self.closeCurses = closeCurses
	def callFunction(self):
		global exitLoop
		param = None
		if self.userInput != None:
			param = queryUser(self.userInput)
		if self.closeCurses:
			curses.endwin()		
		if self.function != backToMainMenu:
			print 'Calling: [python quasar.py '+ getCommandFromFunction(self.function) +']'
		if self.userInput == None or not param:			
			self.function()
		else:
			self.function(param)
		if self.closeCurses:
			exitLoop = True
			return

class SubMenu:
	def __init__(self, id):
		self.id = id	
		self.currentOption = (None, None)#(option, position)
		self.options = [] #if self.options len is 0, it is exit command
	
	def addOption(self, option):
		self.options.append(option)
		if self.currentOption == (None, None):
			self.currentOption = (option, 0)
	def nextOption(self):
		tempIndex = (self.currentOption[1] + 1)
		if(tempIndex == len(self.options)):
			tempIndex = 0
		self.currentOption = (self.options[tempIndex], tempIndex)
	def previousOption(self):
		tempIndex = (self.currentOption[1] - 1)
		if(tempIndex < 0):
			tempIndex = len(self.options) - 1
		self.currentOption = (self.options[tempIndex], tempIndex)
	def description(self):
		return self.currentOption[0].description
	def drawSubMenu(self):
		fillWithPoints()
		try:
			myScreen.addstr(g_sTitleCoordinateY, g_sTitleCoordinateX,  self.description(), curses.A_BOLD)					
			for i in range(0, len(self.options)):
				if self.options[i] == self.currentOption[0]:
					myScreen.addstr(g_sMenuCoordinateY + i * 2, g_sMenuCoordinateX, self.options[i].id, curses.A_REVERSE)
				else:
					myScreen.addstr(g_sMenuCoordinateY + i * 2, g_sMenuCoordinateX, self.options[i].id, curses.A_BOLD)
			myScreen.refresh()
		except curses.error: pass
	def callFunction(self):
		self.currentOption[0].callFunction()
	def isExit(self):
		return len(self.options) == 0
		
class MainMenu:
	def __init__(self):
		self.message = "                         OPC UA Generic Server Framework                        "
		self.currentSubmenu = (None, None)#(submenu, position)
		self.submenus = []
		
	def addSubmenu(self, submenu):
		self.submenus.append(submenu)
		if self.currentSubmenu == (None, None):
			self.currentSubmenu = (submenu, 0)
	def nextSubmenu(self):
		tempIndex = (self.currentSubmenu[1] + 1)
		if(tempIndex == len(self.submenus)):
			tempIndex = 0
		self.currentSubmenu = (self.submenus[tempIndex], tempIndex)
	def previousSubmenu(self):
		tempIndex = (self.currentSubmenu[1] - 1)
		if(tempIndex < 0):
			tempIndex = len(self.submenus) - 1
		self.currentSubmenu = (self.submenus[tempIndex], tempIndex)
	def drawMainMenu(self):
		fillWithPoints()
		try:
			myScreen.addstr(g_sTitleCoordinateY, g_sTitleCoordinateX,  self.message, curses.A_BOLD)
			for i in range(0, len(self.submenus)):
				if self.submenus[i] == self.currentSubmenu[0]:
					myScreen.addstr(g_sMenuCoordinateY + i * 2, g_sMenuCoordinateX, self.submenus[i].id, curses.A_REVERSE)
				else:
					myScreen.addstr(g_sMenuCoordinateY + i * 2, g_sMenuCoordinateX, self.submenus[i].id, curses.A_BOLD)
			myScreen.refresh()
		except curses.error: pass
	def restart(self):
		self.currentSubmenu = (self.submenus[0], 0)

def fillWithPoints():
	rows, columns = myScreen.getmaxyx()
	for y in range(0, rows):
		for x in range(0, columns):
			try:
				myScreen.addch(y,x, '.')
			except curses.error:
				pass		

def createMenus():
	global mainMenu
	mainMenu = MainMenu()
	buildMenu = SubMenu("      Build/Clean project      ")
	buildMenu.addOption(Option("        Automated Build        ", inspect.getdoc(automatedBuild), automatedBuild, True, "                Please insert toolchain cmake configuration file                "))
	buildMenu.addOption(Option("         Clean project         ", inspect.getdoc(distClean), distClean, True))
	buildMenu.addOption(Option("            Return             ", "                             Return to the main menu                            ", backToMainMenu, False))
	deviceMenu = SubMenu("    Generate device classes    ")
	deviceMenu.addOption(Option("        Generate Device        ", inspect.getdoc(generateDeviceClass), generateDeviceClass, True, "                          Please insert device name                              "))
	deviceMenu.addOption(Option("      Generate all Devices     ", inspect.getdoc(generateAllDevices), generateAllDevices, True))
	deviceMenu.addOption(Option("            Return             ", "                             Return to the main menu                            ", backToMainMenu, False))
	designMenu = SubMenu("       Design operations       ")
	designMenu.addOption(Option("        Upgrade Design         ", inspect.getdoc(upgradeDesign), upgradeDesign, True))
	designMenu.addOption(Option("         Format Design         ", inspect.getdoc(formatDesign), formatDesign, True))
	designMenu.addOption(Option("        Validate Design        ", inspect.getdoc(validateDesign), validateDesign, True))
	designMenu.addOption(Option("            Return             ", "                             Return to the main menu                            ", backToMainMenu, False))
	projectMenu = SubMenu("      Project operations       ")
	projectMenu.addOption(Option("       Install framework       ", inspect.getdoc(createProject), createProject, True, "                        Please specify install folder                           "))
	projectMenu.addOption(Option("       Update framework        ", inspect.getdoc(upgradeProject), upgradeProject, True, "                        Please specify upgrade folder                           "))
	projectMenu.addOption(Option("       Check consistency       ", inspect.getdoc(mfCheckConsistency), mfCheckConsistency, True))
	projectMenu.addOption(Option("         Set SVN ignore        ", inspect.getdoc(mfSetupSvnIgnore), mfSetupSvnIgnore, True))
	projectMenu.addOption(Option("        Design vs Device       ", inspect.getdoc(mfDesignVsDevice), mfDesignVsDevice, True))
	projectMenu.addOption(Option("            Return             ", "                             Return to the main menu                            ", backToMainMenu, False))	
	additionalMenu = SubMenu("     Additional operations     ")
	additionalMenu.addOption(Option("       Generate Diagram        ", inspect.getdoc(createDiagram), createDiagram, True, "                  Please specify detail level (0 by detault)                    "))
	additionalMenu.addOption(Option("          Run Doxygen          ", "Runs doxygen in the documentation folder, making the tool generate documentation for the server automatically", runDoxygen, True))
	additionalMenu.addOption(Option("      External tool Check      ", inspect.getdoc(checkExternalDependencies), checkExternalDependencies, True))
	additionalMenu.addOption(Option("            Return             ", "                             Return to the main menu                            ", backToMainMenu, False))
	exitMenu = SubMenu("             Exit              ")
	mainMenu.addSubmenu(buildMenu)
	mainMenu.addSubmenu(deviceMenu)
	mainMenu.addSubmenu(designMenu)
	mainMenu.addSubmenu(projectMenu)
	mainMenu.addSubmenu(additionalMenu)
	mainMenu.addSubmenu(exitMenu)

def draw():
	if currentSubMenu == None:
		mainMenu.drawMainMenu()
	else:
		currentSubMenu.drawSubMenu()
	
def queryUser(message):
	rows, columns = myScreen.getmaxyx()
	for y in range(0, rows):
		for x in range(0, columns):
			try:
				myScreen.addch(y,x, '.')
			except curses.error:
				pass
	myScreen.addstr(3, 0,  message, curses.A_BOLD)
	myScreen.refresh()
	curses.echo()
	# Get a 15-character string, with the cursor on the top line
	s = myScreen.getstr(15,32)
	curses.noecho()
	return s;

def backToMainMenu():
	global mainMenu
	global currentSubMenu
	currentSubMenu = None
	mainMenu.restart()

def processInput():
	key = myScreen.getkey()
	global mainMenu
	global currentSubMenu
	global exitLoop
	
	if(key == "KEY_DOWN"):
		if currentSubMenu == None:
			mainMenu.nextSubmenu()
		else:
			currentSubMenu.nextOption()
	elif(key == "KEY_UP"):
		if currentSubMenu == None:
			mainMenu.previousSubmenu()
		else:
			currentSubMenu.previousOption()
	elif(key == "\n"):#the user hited return
		if currentSubMenu == None:
			if mainMenu.currentSubmenu[0].isExit():
				exitLoop = True
			else:
				currentSubMenu = mainMenu.currentSubmenu[0]
		else:
			currentSubMenu.callFunction()
			
def cursesApplicationMain(screen):
	global myScreen
	myScreen = screen
	createMenus()
	while exitLoop == False:
		draw()		
		processInput()
		
	return

curses.wrapper(cursesApplicationMain)



