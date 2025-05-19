extends Node3D

@export var move_speed : float = 8.0
@export var look_sensitivity : float = 0.1
@export var cinematic : bool = false

# Smoothing factors – higher values make the camera follow input faster.
@export var cinematic_speed_smoothing: float = 10.0
@export var cinematic_rotation_smoothing: float = 5.0

const MIN_MOVE_SPEED = 0.5
const MAX_MOVE_SPEED = 512

var rotating : bool = false

# Variables used only in cinematic mode
var velocity: Vector3 = Vector3.ZERO
var target_rotation: Vector3 = Vector3.ZERO

func _ready() -> void:
	set_mouse(true)
	# Initialize target rotation with the current rotation.
	target_rotation = rotation

func _process(delta: float) -> void:
	var _wish_dir_raw = Input.get_vector("move_left", "move_right", "move_forward", "move_backward")
	var up = Vector3.ZERO
	if Input.is_action_pressed("move_up"):
		up = Vector3.UP
	if Input.is_action_pressed("move_down"):
		up = Vector3.DOWN
	var forward = transform.basis.z.normalized()
	var right = transform.basis.x.normalized()
	var move_dir = forward * _wish_dir_raw.y + right * _wish_dir_raw.x + up

	if cinematic:
		# Calculate the desired velocity from the current input.
		var desired_velocity: Vector3 = Vector3.ZERO
		if move_dir.length() > 0:
			desired_velocity = move_dir.normalized() * move_speed

		# Smoothly interpolate toward the desired velocity.
		velocity = velocity.lerp(desired_velocity, cinematic_speed_smoothing * delta)
		position += velocity * delta

		# Update rotation by easing from the current rotation toward the target rotation.
		rotation = rotation.lerp(target_rotation, cinematic_rotation_smoothing * delta)
	else:
		# Immediate movement with no smoothing.
		if move_dir != Vector3.ZERO:
			position += move_dir.normalized() * move_speed * delta / Engine.time_scale

func _input(event) -> void:
	if event.is_action_pressed("scroll_up"):
		move_speed *= 2
	if event.is_action_pressed("scroll_down"):
		move_speed *= 0.5
	move_speed = clamp(move_speed, MIN_MOVE_SPEED, MAX_MOVE_SPEED)

	if event is InputEventMouseMotion and rotating:
		var rotation_change = Vector3(-event.relative.y * 0.0025, -event.relative.x * 0.0025, 0)
		if cinematic:
			# Instead of immediately applying the change, update our target.
			target_rotation += rotation_change
			target_rotation.x = clamp(target_rotation.x, -PI/2.2, PI/2.2)
		else:
			rotation += rotation_change
			rotation.x = clamp(rotation.x, -PI/2.2, PI/2.2)

func set_mouse(value: bool) -> void:
	rotating = value
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED if rotating else Input.MOUSE_MODE_VISIBLE
