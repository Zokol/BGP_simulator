import pygame
from config import *

class StaticSprite(pygame.sprite.DirtySprite):
	def __init__(self, image, rect=None, layer=0):
		pygame.sprite.DirtySprite.__init__(self)
		self.image = image
		self.rect = image.get_rect()
		self._layer = layer
		if rect is not None:
			self.rect = rect

class TextSprite(StaticSprite): # hehehehehe
	def __init__(self, text, area_rect, layer):
		font = pygame.font.Font(FONT, int(150*FONTSCALE))
		image = font.render(text, True, (0, 0, 0))
		rect = image.get_rect()
		rect.center = area_rect.center

		StaticSprite.__init__(self, image, rect, layer)

class AnimatedSprite(pygame.sprite.DirtySprite):
	def __init__(self, images, rect, layer, interval = 1):
		pygame.sprite.DirtySprite.__init__(self)
		self._layer = layer

		self.images = images
		self.pos = 0

		self.image = self.images[self.pos]
		self.rect = rect

		self.interval = interval
		self.count = 0

	def reset(self):
		self.count = 0
		if self.pos != 0:
			self.pos = 0
			self.image = self.images[self.pos]
			self.dirty = 1

	def update(self):
		if not self.visible:
			return

		# Do not bother to do anything if there's no animation
		if len(self.images) <= 1:
			return

		self.count += 1
		if self.count >= self.interval:
			self.count = 0
			self.pos += 1
			if self.pos >= len(self.images):
				self.pos = 0

			self.image = self.images[self.pos]
			self.dirty = 1

class StateTrackingSprite(pygame.sprite.DirtySprite):
	def __init__(self):
		pygame.sprite.DirtySprite.__init__(self)
		self.state = None

	def get_state(self):
		raise NotImplemented, 'This method must be implemented by base classes'

	def redraw(self):
		raise NotImplemented, 'This method must be implemented by base classes'

	def update(self):
		if not self.visible:
			return

		state = self.get_state()
		if state == self.state:
			return
		self.state = state

		self.redraw()
		self.dirty = 1

Tile = StaticSprite
Textile = TextSprite
AnimatedTile = AnimatedSprite

