extends Node3D

# Adjustable properties for camera shake:
@export var enabled := true
@export var shake_amplitude := 0.5  # How strong the shake is.
@export var shake_frequency := 3.0  # How fast the shake moves.

@export var noise: FastNoiseLite
var original_origin: Vector3
var t := 0.0

func _process(delta):
	if !enabled:
		return;
		
	t += delta * shake_frequency
	var offset := Vector3(
		noise.get_noise_1d(t),
		noise.get_noise_1d(t + 100.0),
		0
	) * deg_to_rad(shake_amplitude)

	# Apply the noise offset to the original camera position.
	rotation = offset
