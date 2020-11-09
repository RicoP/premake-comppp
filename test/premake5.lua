local component = require "comppp"

workspace "MyWorkspace"
	location ".build/projects"
	targetdir ".build/bin/%{cfg.buildcfg}"
	configurations { "Debug", "Release" }
	language "C++"
	component.targetdir "source/components"

project "MyProject"
	kind "ConsoleApp"
	component.new "vector2"
	component.add {
		x="float",
		y="float",
	}
	component.new "vector3"
	component.add {
		x="float",
		y="float",
		z={"float", "0"}
	}
	component.new "quaternion"
	component.add {
		x="float",
		y="float",
		z="float",
		w="float"
	}
	component.new "matrix4"
	component.add {
		m00="float", m10="float", m20="float", m30="float",
		m01="float", m11="float", m21="float", m31="float",
		m02="float", m12="float", m22="float", m32="float",
		m03="float", m13="float", m23="float", m33="float"
	}
	component.new "Player"
	component.add {
		position="vector3",
		state="Active|Jumping",
	}
	component.new "World"
	component.add {
		title="string(64)",
		player="Player[4]",
		scores="float[4]"
	}
	component.new "ObjectID"
	component.add {
		id="int"
	}
	component.new "ShaderAssetRef"
	component.add {
		id="int"
	}
	component.new "SoundAssetRef"
	component.add {
		id="int"
	}
	component.new "MeshAssetRef"
	component.add {
		id="int"
	}
	component.new "TextureAssetRef"
	component.add {
		id="int"
	}

	-- From Gameengine
	component.new "Transform"
	component.add {
	  position= {"vector3", "vector(0,0,0)" },
	  scale= {"vector3", "vector(1,1,1)" },
	  rotation= {"quaternion", "qidentity()" }, -- remove this
	  mvt= {"matrix4", "identity4()"},
	}
	component.new "Camera"
	component.add {
	  projection= {"matrix4", "identity4()"},
	  lookat= {"matrix4", "identity4()"},
	  fov= {"float", "70" },
	  near_plane= {"float", "0.1f" },
	  far_plane= {"float", "1000" },
	}
	component.new "Skybox"
	component.add {
	  object= {"ObjectID" },
	}
	component.new "Collider"
	component.add {
	  pivot= {"vector3", "vector(0,0,0)" },
	  scale= {"vector3", "vector(1,1,1)" },
	  active= {"bool" },
	  intersections= {"ObjectID[16]" },
	}
	component.new "SceneObject"
	component.add {
	  ID= {"ObjectID", "idgen()" },
	  mesh= {"MeshAssetRef"},
	  texture= {"TextureAssetRef"},
	  transform= {"Transform"},
	  collider= {"Collider"},
	}
	component.new "Hero"
	component.add {
	  inputv= {"vector3"},
	  velocity= {"vector3"},
	  cam_offset= {"vector3", "vector(0,0,1)" },
	  cam_speed= {"vector2", "vector(0.05f,0.05f)" },
	  heroWalkSpeed= {"float", "0.3f"},
	  heroRunSpeed= {"float", "0.9f"},
	  renderObject= {"ObjectID" },
	  transform= {"Transform"},
	  jump_sound= {"SoundAssetRef"},
	  jump= {"bool"},
	}
	component.new "Scene"
	component.add {
	  skybox= {"Skybox" },
	  --TODO: make the game and editor cameras explicit
	  cameras= {"Camera[16]" }, -- 0 = editor camera, 1 = main game camera, 2 = stealth camera, 3+ = side cameras
	  activeCamera= {"int", "1" },
	  hero= {"Hero" },
	  objects= {"SceneObject[1024]" },
	}


	files {"source/**"}
	component.generate()
