import pygame
from pygame.locals import *
from config import *
import time
import sys

class Resources:
	def __init__(self, screen):
		self.screen = screen

	def __getattr__(self, name):
		"""Autoload resources as they are accessed"""
		load_name = 'load_' + name
		assert hasattr(self, load_name)
		loader = getattr(self, load_name)
		loader()
		assert hasattr(self, name)
		return getattr(self, name)

	def load_terrain(self):
		self.terrain = load_named_tiles('tilemap_terrain', TILE_SIZE, (255, 0, 255), SCALE)
		self.terrain = {'G': [self.terrain['G']], 'D': [self.terrain['D']], 'F': [self.terrain['G']], 'W': [self.terrain['W[1]'], self.terrain['W[2]']]}

	def load_borders(self):
		self.borders = load_named_tiles('tilemap_borders', BORDER_SIZE, (255, 0, 255), SCALE)

	def load_overlay(self):
		self.overlay = load_named_tiles('tilemap_overlay', OVERLAY_SIZE, (255, 0, 255), SCALE)

	def load_races(self):
		raceimages = load_tiles('races.png', CHAR_SIZE, (255, 0, 255), SCALE)
		racenames = file(os.path.join(GFXDIR, 'races.txt')).read().split('\n')

		self.races = {}
		for name, images in zip(racenames, raceimages):
			self.races[name] = {270: images[0], 180: images[1], 0: images[2], 90: images[3]}

	def load_hairs(self):
		hairimages = load_tiles('hairs.png', HAIR_SIZE, (255, 0, 255), SCALE)
		hairnames = file(os.path.join(GFXDIR, 'hairs.txt')).read().split('\n')

		self.hairs = {}
		for name, images in zip(hairnames, hairimages):
			self.hairs[name] = {270: images[0], 0: images[1], 90: images[2], 180: pygame.transform.flip(images[1], True, False)}

	def load_armors(self):
		armorimages = load_tiles('Collections/Heavy_Armor_Collection.png', ARMOR_SIZE, (255, 255, 255), SCALE)
		armornames = file(os.path.join(GFXDIR, 'Collections/Heavy_Armor_Collection.txt')).read().split('\n')[:-1]

		self.armors = {}
		for namerow, names in enumerate(armornames):
			for namecol, name in enumerate(names.split(' ')):
				sizes = {}
				for sizeidx, size in enumerate('dwarf human gnome elf'.split()):
					images = armorimages[namerow*4 + sizeidx][namecol*3:(namecol+1)*3]
					sizes[size] = {270: images[0], 180: images[1], 90: images[2], 0: pygame.transform.flip(images[1], True, False)}
				self.armors[name] = sizes

	def load_selections(self):
		self.selections = {}
		for name, color in [('red', (255, 0, 0)), ('green', (10, 212, 0))]:
			surf = pygame.Surface(TILE_SIZE)
			surf.fill(color)
			surf.set_alpha(120)
			self.selections[name] = surf

def blitteds(dst, srcs, copy = True):
	"""Blit multiple images on top of dst resizing it if needed"""
	left = top = right = bottom = 0
	dw, dh = dst.get_size()

	# Calculate the needed extra size
	for src, (x, y) in srcs:
		sw, sh = src.get_size()
		left = max(left, -x)
		top = max(top, -y)
		right = max(right, x + sw - dw)
		bottom = max(bottom, y + sh - dh)

	# Create a new surface if the old size is not enough
	if top or left or bottom or right:
		tmp = pygame.Surface((dw + left + right, dh + top + bottom))
		tmp.set_colorkey(dst.get_colorkey())
		tmp.fill(dst.get_colorkey())
		tmp.blit(dst, (left, top))
		dst = tmp
	elif copy:
		dst = dst.copy()

	# Blit the images to the appropriate positions
	for src, (x, y) in srcs:
		dst.blit(src, (x + left, y + top))

	return dst, (left, top)

def init_pygame(mode = (1200, 720)):
	if '--no-sound' in sys.argv:
		pygame.mixer = None
	if pygame.mixer:
		pygame.mixer.pre_init(48000)
	pygame.init()
	screen = pygame.display.set_mode(mode)
	if pygame.mixer:
		pygame.mixer.set_reserved(1)
	return screen

def change_sound(channel, new_sound, fade_ms, loops = -1):
	if not pygame.mixer:
		return
	channel = pygame.mixer.Channel(channel)
	if channel.get_sound():
		channel.set_endevent(USEREVENT)
		channel.fadeout(fade_ms)
		while pygame.event.wait().type != USEREVENT:
			pass
		channel.set_endevent()
	channel.play(new_sound, loops = loops, fade_ms = fade_ms)

def load_image(name, colorkey=None, scale=1):
	path = os.path.join(GFXDIR, name)
	try:
		image = pygame.image.load(path)
	except pygame.error, message:
		print 'Cannot load image:', name
		raise SystemExit, message
	image = image.convert()
	if colorkey is not None:
		if colorkey is -1:
			colorkey = image.get_at((0,0))
		image.set_colorkey(colorkey, RLEACCEL)
	if scale is not 1:
		image_rect = image.get_rect()
		image = pygame.transform.scale(image, (image_rect.right*scale, image_rect.bottom*scale))
	return image

def load_tiles(name, (width, height), colorkey=None, scale=1):
	#print "Scaling: %d" % (scale)
	image = load_image(name, colorkey, scale)
	return parse_tiles(image, (width, height))

def parse_tiles(tileimage, (width, height)):
	rect = tileimage.get_rect()
	if rect.width % width or rect.height % height:
		print 'Tile image should be divisible to (%d,%d)' % (width, height)
	cols = rect.width / width
	rows = rect.height / height
	#print "parse_tiles - got %dx%d tileset with %d cols and %d rows" % (rect.width, rect.height, cols, rows)
	images = []
	for y in range(rows):
		row = []
		for x in range(cols):
			image = tileimage.subsurface((x*width, y*height, width, height))
			row.append(image)
		images.append(row)
	return images

def load_named_tiles(name, (width, height), colorkey=None, scale=1):
	images = load_tiles(name + '.png', (width, height), colorkey, scale)
	tiles = {}
	with file(os.path.join(GFXDIR, name + '.txt'), 'rb') as f:
		for y, line in enumerate(f):
			for x, name in enumerate(line.split()):
				tiles[name] = images[y][x]
	return tiles

def load_map(name):
	path = os.path.join(MAPDIR, name)
	with file(path, 'rb') as f:
		aliases = {}
		width = height = 0
		spawns = {}
		for line in f:
			cmd = line.strip().split()
			if not cmd:
				break
			if cmd[0] == 'SIZE':
				width, height = int(cmd[1]), int(cmd[2])
			elif cmd[0] == 'ALIAS':
				aliases[cmd[1]] = cmd[2]
			elif cmd[0] == 'SPAWN':
				spawns.setdefault(int(cmd[1]), []).append((int(cmd[2]), int(cmd[3])))
			else:
				raise ValueError, 'Unknown map directive'
		result = []
		for y in range(height):
			line = f.next()
			tiles = [aliases.get(tile, tile) for tile in line.split()]
			assert len(tiles) == width
			result.append(tiles)
	return result, (width, height), spawns

def load_sound(name):
	class NoneSound:
		def play(self): pass
	if not pygame.mixer:
		return NoneSound()
	fullname = os.path.join(SNDDIR, name)
	try:
		sound = pygame.mixer.Sound(fullname)
	except pygame.error, message:
		print 'Cannot load sound:', wav
		raise SystemExit, message
	return sound

