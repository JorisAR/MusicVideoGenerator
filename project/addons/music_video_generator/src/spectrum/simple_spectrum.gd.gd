extends Node2D


@export var bar_color : Color = Color(1.0,1.0,1.0)
var SIZE := Vector2(800, 250)
var OFFSET := Vector2(0, 0)

@export var reference_node : Node2D
@export var music_manager : MusicManager

func _draw():
	var spectrum : PackedFloat32Array = music_manager.spectrum_data;
	var bar_count : int = music_manager.spectrum_size;
	var w = SIZE.x / bar_count
	for i in range(bar_count):
		var height : float = spectrum[i] * SIZE.y
		var color_reflection := bar_color
		color_reflection.a = 0.0125
		draw_rect(
			Rect2(w * i + OFFSET.x, SIZE.y - height + OFFSET.y, w - 2, height),
			bar_color
		)
		draw_rect(
			Rect2(w * i + OFFSET.x, SIZE.y + OFFSET.y, w - 2, height),
			color_reflection
		)
		
func _process(_delta):
	queue_redraw()

func _ready():
	SIZE = reference_node.scale
	OFFSET = reference_node.position - SIZE * 0.5
