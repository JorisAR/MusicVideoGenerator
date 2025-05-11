extends Node2D

@export var bar_count : int = 16
@export var max_frequency : float = 18000.0
@export var bar_color : Color = Color(1.0,1.0,1.0)
var SIZE := Vector2(800, 250)
var OFFSET := Vector2(0, 0)

const HEIGHT_SCALE : float = 8.0
const MIN_DB : float = 60
const ANIMATION_SPEED : float = 0.1

var spectrum : AudioEffectSpectrumAnalyzerInstance
var min_values : Array[float] = []
var max_values : Array[float] = []

@export var reference_node : Node2D
@export var audio_bus := 1
@export var audio_effect_index := 0

func _apply_slope(frequency: float, height: float) -> float:
	const ref_freq := 20.0
	const gain_per_octave := pow(10, 4.5 / 20.0)
	var octaves_above := log(frequency / ref_freq) / log(2)
	var gain := pow(gain_per_octave, octaves_above)    
	return height * gain

func _draw():
	var w = SIZE.x / bar_count
	for i in range(bar_count):
		var min_height : float = min_values[i]
		var max_height : float = max_values[i]
		var height = lerp(min_height, max_height, ANIMATION_SPEED)
		
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
	var prev_hz = 0
	for i in range(bar_count):
		var hz = (1 + i) * max_frequency / bar_count
		var magnitude = spectrum.get_magnitude_for_frequency_range(prev_hz, hz).length()
		prev_hz = hz
		
		magnitude = _apply_slope(hz, magnitude)
		var energy = clampf((MIN_DB + linear_to_db(magnitude)) / MIN_DB, 0, 1)
		var height = energy * SIZE.y * HEIGHT_SCALE
		
		if height > max_values[i]:
			max_values[i] = height
		else:
			max_values[i] = lerp(max_values[i], height, ANIMATION_SPEED)
		if height <= 0.0:
			min_values[i] = lerp(min_values[i], 0.0, ANIMATION_SPEED)
	queue_redraw()

func _ready():
	SIZE = reference_node.scale
	OFFSET = reference_node.position - SIZE * 0.5
	spectrum = AudioServer.get_bus_effect_instance(audio_bus, audio_effect_index)
	
	min_values.resize(bar_count)
	max_values.resize(bar_count)
	min_values.fill(0.0)
	max_values.fill(0.0)
