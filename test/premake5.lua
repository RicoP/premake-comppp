local component = require "comppp"

workspace "MyWorkspace"
	location ".build/projects"
	targetdir ".build/bin/%{cfg.buildcfg}"
	configurations { "Debug", "Release" }
	language "C++"
	--component.targetdir "source/components"

project "MyProject"
	kind "ConsoleApp"
	files {"main.cpp"}
	component.new "vector3"
	component.add { x="float", y="float", z="float" }
