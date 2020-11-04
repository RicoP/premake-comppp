local component = require "comppp"

workspace "MyWorkspace"
	location ".build/projects"
	targetdir ".build/bin/%{cfg.buildcfg}"
	configurations { "Debug", "Release" }
	language "C++"
	component.targetdir "source/components"

project "MyProject"
	kind "ConsoleApp"
	component.new "vector3"
	component.add {
		x="float",
		y="float",
		z={"float", "0"}
	}
	component.new "Player"
	component.add {
		position="vector3",
		state="int",
	}
	component.new "World"
	component.add {
		position="Player[4]"
	}
	files {"source/**"}
	component.generate()
