extends Node

func _input(event) -> void:
	# Use is_action_just_pressed to prevent multiple screenshots on one press.
	if event.is_action_pressed("take_screenshot"):
		take_screenshot()

func take_screenshot() -> void:
	print("Taking screenshot...")
	# Capture the current viewport texture and grab its image data.
	var viewport_texture = get_viewport().get_texture()
	var screenshot: Image = viewport_texture.get_image()
	
	# Ensure the user screenshots directory exists.
	var screenshots_dir = "user://screenshots"
	DirAccess.make_dir_recursive_absolute(screenshots_dir) 
	
	var timestamp = Time.get_datetime_string_from_system()
	timestamp = timestamp.replace(":", "-")
	timestamp = timestamp.replace(" ", "T")
	
	var file_path = "%s/screenshot_%s.png" % [screenshots_dir, timestamp]
	
	var err = screenshot.save_png(file_path)
	if err == OK:
		print("Screenshot saved successfully at ", ProjectSettings.globalize_path(file_path))
	else:
		print("Error saving screenshot: ", err)
