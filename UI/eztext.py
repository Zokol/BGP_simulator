# input lib
from pygame.locals import *
import pygame, string
from resources import *
from tiles import *
from UI import *
from config import FONT, FONTSCALE

DEFAULT_ALLOWED = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!"#$%&\'()*+,-./:;<=>?@[\]^_`{|}~ '

class Input(StateTrackingSprite, UIObject):
	def __init__(self, parent, rel_pos, size, font = None, color = (0, 0, 0), restricted = DEFAULT_ALLOWED, maxlength = None, prompt = '', handle_enter = None, interval = 10):
		StateTrackingSprite.__init__(self)
		UIObject.__init__(self, parent, rel_pos, size)

		self.image = pygame.Surface(self.size)
		self.font = font or pygame.font.Font(FONT, int(32*FONTSCALE))
		self.color = color
		self.restricted = restricted
		self.maxlength = maxlength
		self.prompt = prompt
		self.value = ''
		self._handle_enter = handle_enter
		self.interval = interval
		self.count = 0

	rect = property(lambda self: pygame.Rect(self.pos, self.image.get_size()))

	def update(self):
		self.count = (self.count + 1) % (2 * self.interval)
		StateTrackingSprite.update(self)

	def get_state(self):
		return self.font, self.color, self.prompt, self.value, (self.count / self.interval) % 2 == 0 and get_inputfocus() == self

	def redraw(self):
		font, color, prompt, value, cursor = self.state
		text = font.render(prompt + value, True, color)
		self.image.fill((255, 255, 255))
		self.image.blit(text, (0, 0))
		if cursor:
			self.image.fill((0, 0, 0), (text.get_width() + 2, 2, 2, self.height - 4))

	def handle_enter(self):
		if self._handle_enter:
			self._handle_enter()

	def event(self, event):
		if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
			if self.contains(*event.pos):
				grab_inputfocus(self)
				self.count = 0
			else:
				ungrab_inputfocus(self)
		if event.type == KEYDOWN and get_inputfocus() == self:
			if event.key == K_BACKSPACE:
				self.value = self.value[:-1]
			elif event.key == K_RETURN:
				self.handle_enter()
			elif event.unicode in self.restricted:
				self.value += event.unicode
			self.count = 0
		if self.maxlength is not None and len(self.value) > self.maxlength:
			self.value = self.value[:self.maxlength]

