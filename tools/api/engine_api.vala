/*
 * Copyright (c) 2012-2017 Daniele Bartolini and individual contributors.
 * License: https://github.com/dbartolini/crown/blob/master/LICENSE
 */

namespace Crown
{
	/// Functions to encode Vala types to Lua.
	namespace Lua
	{
		public string bool(bool b)
		{
			return b == true ? "true" : "false";
		}

		public string vector2(Vector2 v)
		{
			return "Vector2(%f, %f)".printf(v.x, v.y);
		}

		public string vector3(Vector3 v)
		{
			return "Vector3(%f, %f, %f)".printf(v.x, v.y, v.z);
		}

		public string quaternion(Quaternion q)
		{
			return "Quaternion.from_elements(%f, %f, %f, %f)".printf(q.x, q.y, q.z, q.w);
		}

	}

	namespace DataCompilerApi
	{
		public string compile(Guid id, string data_dir, string platform)
		{
			return "{\"type\":\"compile\",\"id\":\"%s\",\"data_dir\":\"%s\",\"platform\":\"%s\"}".printf(id.to_string()
				, data_dir.replace("\\", "\\\\").replace("\"", "\\\"")
				, platform
				);
		}

	}

	namespace DeviceApi
	{
		public string command(string[] args)
		{
			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < args.length; ++i)
			{
				string arg = args[i].replace("\\", "\\\\").replace("\"", "\\\"");
				sb.append("\"%s\",".printf(arg));
			}

			return "{\"type\":\"command\",\"args\":[%s]}".printf(sb.str);
		}

		public string reload(string type, string name)
		{
			return command({ "reload", type, name });
		}

		public string pause()
		{
			return command({ "pause" });
		}

		public string unpause()
		{
			return command({ "unpause" });
		}

	}

	namespace LevelEditorApi
	{
		public string reset()
		{
			return "LevelEditor:reset()";
		}

		public string set_mouse_state(int x, int y, bool left, bool middle, bool right)
		{
			return "LevelEditor:set_mouse_state(%d,%d,%s,%s,%s)".printf(x
				, y
				, Lua.bool(left)
				, Lua.bool(middle)
				, Lua.bool(right)
				);
		}

		public string mouse_move(int x, int y, int dx, int dy)
		{
			return "LevelEditor:mouse_move(%d,%d,%d,%d)".printf(x, y, dx, dy);
		}

		public string mouse_wheel(double delta)
		{
			return "LevelEditor:mouse_wheel(%f)".printf(delta);
		}

		public string mouse_down(int x, int y)
		{
			return "LevelEditor:mouse_down(%d,%d)".printf(x, y);
		}

		public string mouse_up(int x, int y)
		{
			return "LevelEditor:mouse_up(%d,%d)".printf(x, y);
		}

		public string key_down(string key)
		{
			return "LevelEditor:key_down(\"%s\")".printf(key);
		}

		public string key_up(string key)
		{
			return "LevelEditor:key_up(\"%s\")".printf(key);
		}

		private const string[] _tools =
		{
			"place_tool",
			"move_tool",
			"rotate_tool",
			"scale_tool"
		};

		public string set_tool_type(ToolType type)
		{
			return "LevelEditor:set_tool(LevelEditor.%s)".printf(_tools[(int)type]);
		}

		public string set_snap_mode(SnapMode sm)
		{
			return """LevelEditor:set_snap_mode("%s")""".printf(sm == SnapMode.RELATIVE ? "relative" : "absolute");
		}

		public string set_reference_system(ReferenceSystem rs)
		{
			return """LevelEditor:set_reference_system("%s")""".printf(rs == ReferenceSystem.LOCAL ? "local" : "world");
		}

		public string enable_show_grid(bool enabled)
		{
			return "LevelEditor:enable_show_grid(%s)".printf(Lua.bool(enabled));
		}

		public string enable_snap_to_grid(bool enabled)
		{
			return "LevelEditor:enable_snap_to_grid(%s)".printf(Lua.bool(enabled));
		}

		public string enable_debug_render_world(bool enabled)
		{
			return "LevelEditor:enable_debug_render_world(%s)".printf(Lua.bool(enabled));
		}

		public string enable_debug_physics_world(bool enabled)
		{
			return "LevelEditor:enable_debug_physics_world(%s)".printf(Lua.bool(enabled));
		}

		public string set_grid_size(double size)
		{
			return "LevelEditor:set_grid_size(%f)".printf(size);
		}

		public string set_rotation_snap(double deg)
		{
			return "LevelEditor:set_rotation_snap(%f)".printf(MathUtils.rad(deg));
		}

		public string spawn_unit(Guid id, string name, Vector3 pos, Quaternion rot, Vector3 scl)
		{
			return "LevelEditor:spawn_unit(\"%s\", \"%s\", %s, %s, %s)".printf(id.to_string()
				, name
				, Lua.vector3(pos)
				, Lua.quaternion(rot)
				, Lua.vector3(scl)
				);
		}

		public string spawn_empty_unit(Guid id)
		{
			return "LevelEditor:spawn_empty_unit(\"%s\")".printf(id.to_string());
		}

		public string spawn_sound(Guid id, string name, Vector3 pos, Quaternion rot, double range, double volume, bool loop)
		{
			return "LevelEditor:spawn_sound(\"%s\", \"%s\", %s, %s, %f, %f, %s)".printf(id.to_string()
				, name
				, Lua.vector3(pos)
				, Lua.quaternion(rot)
				, range
				, volume
				, Lua.bool(loop)
				);
		}

		public string add_tranform_component(Guid id, Guid component_id, Vector3 pos, Quaternion rot, Vector3 scl)
		{
			return "LevelEditor:add_transform_component(\"%s\", \"%s\", %s, %s, %s)".printf(id.to_string()
				, component_id.to_string()
				, Lua.vector3(pos)
				, Lua.quaternion(rot)
				, Lua.vector3(scl)
				);
		}

		public string add_mesh_component(Guid id, Guid component_id, string mesh_resource, string geometry_name, string material_resource, bool visible)
		{
			return "LevelEditor:add_mesh_component(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %s)".printf(id.to_string()
				, component_id.to_string()
				, mesh_resource
				, geometry_name
				, material_resource
				, Lua.bool(visible)
				);
		}

		public string add_sprite_component(Guid id, Guid component_id, string sprite_resource, string material_resource, bool visible)
		{
			return "LevelEditor:add_sprite_component(\"%s\", \"%s\", \"%s\", \"%s\", %s)".printf(id.to_string()
				, component_id.to_string()
				, sprite_resource
				, material_resource
				, Lua.bool(visible)
				);
		}

		public string add_camera_component(Guid id, Guid component_id, string projection, double fov, double far_range, double near_range)
		{
			return "LevelEditor:add_camera_component(\"%s\", \"%s\", \"%s\", %f, %f, %f)".printf(id.to_string()
				, component_id.to_string()
				, projection
				, fov
				, far_range
				, near_range
				);
		}

		public string add_light_component(Guid id, Guid component_id, string type, double range, double intensity, double spot_angle, Vector3 color)
		{
			return "LevelEditor:add_light_component(\"%s\", \"%s\", \"%s\", %f, %f, %f, %s)".printf(id.to_string()
				, component_id.to_string()
				, type
				, range
				, intensity
				, spot_angle
				, Lua.vector3(color)
				);
		}

		public string move_object(Guid id, Vector3 pos, Quaternion rot, Vector3 scl)
		{
			return @"LevelEditor:move_object(\"%s\", %s, %s, %s)".printf(id.to_string()
				, Lua.vector3(pos)
				, Lua.quaternion(rot)
				, Lua.vector3(scl)
				);
		}

		public string set_light(Guid id, string type, double range, double intensity, double spot_angle, Vector3 color)
		{
			return @"LevelEditor._objects[\"%s\"]:set_light(\"%s\", %f, %f, %f, %s)".printf(id.to_string()
				, type
				, range
				, intensity
				, spot_angle
				, Lua.quaternion({color.x, color.y, color.z, 1.0})
				);
		}

		public string set_sound_range(Guid id, double range)
		{
			return @"LevelEditor._objects[\"%s\"]:set_range(%f)".printf(id.to_string()
				, range
				);
		}

		public string set_placeable(string type, string name)
		{
			return "LevelEditor:set_placeable(\"%s\", \"%s\")".printf(type, name);
		}

		public string selection_set(Guid[] ids)
		{
			StringBuilder sb = new StringBuilder();
			sb.append("LevelEditor._selection:set({");
			for (int i = 0; i < ids.length; ++i)
				sb.append("\"%s\",".printf(ids[i].to_string()));
			sb.append("})");
			return sb.str;
		}

		public string destroy(Guid id)
		{
			return @"LevelEditor:destroy(\"%s\")".printf(id.to_string());
		}

		public string set_color(string name, Vector3 color)
		{
			Quaternion c = Quaternion(color.x, color.y, color.z, 1.0);
			return @"Colors.%s = function() return %s end".printf(name, Lua.quaternion(c));
		}

	}

	namespace UnitPreviewApi
	{
		public string set_preview_resource(string placeable_type, string name)
		{
			return "UnitPreview:set_preview_resource(\"%s\", \"%s\")".printf(placeable_type, name);
		}

	}

}
