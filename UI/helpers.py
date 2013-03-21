
def get_distance_2(pos1, pos2):
	"""Get squared euclidean distance"""
	return (pos2[0] - pos1[0])**2 + (pos2[1] - pos1[1])**2

def div_ceil(dividend, divisor):
	"""Integer division, but with result rounded up instead"""
	return (dividend - 1) / divisor + 1

def sign(value):
	return cmp(value, 0)

# XXX: Figure out how to do scaling better (output is new_max only if input is old_max)
def scale_floor(value, old_max, new_max):
	"""Scaling function where the result is new_max only when the input is old_max"""
	assert value >= 0
	assert value <= old_max
	if old_max == 0:
		return 0
	return new_max * value / old_max

def scale_ceil(value, old_max, new_max):
	"""Scaling function where the result is 0 only when the input is 0"""
	assert value >= 0
	assert value <= old_max
	if old_max == 0:
		return 0
	return div_ceil(new_max * value, old_max)

scale = scale_ceil

def scale_pos(value, old_max, new_max):
	return tuple(scale(value[i], old_max[i], new_max[i]) for i in range(2))

# Misleading names, since they are inclusize
clamp_below = lambda v, maxv: min(v, maxv)
clamp_above = lambda v, minv: max(v, minv)

def clamp(v, minv, maxv):
	"""Clamp the value v between minv and maxv, inclusive"""
	assert minv <= maxv, 'Invalid clamping constraint'
	return min(max(v, minv), maxv)
	# minv <= v <= maxv

def clamp_pos((x, y), (width, height)):
	"""Clamp the position (x, y) inside an element of size (width, height)"""
	# The position does not have a size, so it can reach the maximum size
	assert 0 <= width and 0 <= height, 'Invalid clamping constraint'
	return (clamp(x, 0, width), clamp(y, 0, height))
	# 0 <= x <= width and 0 <= y <= height

def clamp_elem((x, y), (width, height), (area_width, area_height)):
	"""Clamp the element of position (x, y) and of size (width, height) inside an element of size (area_width, area_height)"""
	assert 0 <= width <= area_width and 0 <= height <= area_height, 'Invalid clamping constraint'
	return clamp_pos((x, y), (area_width - width, area_height - height))
	# 0 <= x + width <= area_width and 0 <= y + height <= area_height

