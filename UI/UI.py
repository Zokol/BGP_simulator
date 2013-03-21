"""""""""""""""""""""""""""""""""""""""""""""
@name: UI-Class
@author: Heikki Juva, Aleksi Torhamo

@description:
Contains all the UI-components used in Xadir.
"""""""""""""""""""""""""""""""""""""""""""""

import sys, time
import pygame
from resources import *

class UIRoot:
	def __init__(self):
		self.x = self.y = 0

class UIObject(object):
	"""Base class of everything with coordinates.

	Instances have the following attributes:
	- pos     = x, y          = position on screen
	- rel_pos = rel_x, rel_y  = position relative to parent
	- size    = width, height = size
	"""
	def __init__(self, parent, rel_pos, size = None):
		if not parent:
			parent = UIRoot()
		self.parent = parent
		self.rel_pos = rel_pos
		if size:
			self.size = size

	def _get_pos(self): return (self.x, self.y)
	def _set_pos(self, pos): (self.x, self.y) = pos
	pos = property(_get_pos, _set_pos)

	def _get_rel_x(self): return self.x - self.parent.x
	def _set_rel_x(self, x): self.x = self.parent.x + x
	rel_x = property(_get_rel_x, _set_rel_x)

	def _get_rel_y(self): return self.y - self.parent.y
	def _set_rel_y(self, y): self.y = self.parent.y + y
	rel_y = property(_get_rel_y, _set_rel_y)

	def _get_rel_pos(self): return (self.rel_x, self.rel_y)
	def _set_rel_pos(self, pos): (self.rel_x, self.rel_y) = pos
	rel_pos = property(_get_rel_pos, _set_rel_pos)

	def _get_size(self): return (self.width, self.height)
	def _set_size(self, size): (self.width, self.height) = size
	size = property(_get_size, _set_size)

	def _get_rect(self): return pygame.Rect(self.x, self.y, self.width, self.height)
	rect = property(_get_rect)

	def contains(self, x, y):
		return x >= self.x and y >= self.y and x < self.x + self.width and y < self.y + self.height

	def translate(self, x, y):
		return x - self.x, y - self.y

class UIGridObject(UIObject):
	"""Base class of everything that's on a grid.

	Instances have the following attributes:
	- pos      = x, y           = position on screen
	- rel_pos  = rel_x, rel_y   = position relative to parent
	- grid_pos = grid_x, grid_y = position on grid
	- size     = width, height  = size
	"""
	def __init__(self, grid, grid_pos):
		UIObject.__init__(self, grid, (0, 0), grid.cell_size)
		self.grid = grid
		self.grid_pos = grid_pos

	def _get_grid_x(self): return self.rel_x / self.width
	def _set_grid_x(self, x): self.rel_x = x * self.width
	grid_x = property(_get_grid_x, _set_grid_x)

	def _get_grid_y(self): return self.rel_y / self.height
	def _set_grid_y(self, y): self.rel_y = y * self.height
	grid_y = property(_get_grid_y, _set_grid_y)

	def _get_grid_pos(self): return (self.grid_x, self.grid_y)
	def _set_grid_pos(self, pos): (self.grid_x, self.grid_y) = pos
	grid_pos = property(_get_grid_pos, _set_grid_pos)

class UIContainer(UIObject):
	def __init__(self, parent, rel_pos, size, surface, transparent=False):
		UIObject.__init__(self, parent, rel_pos, size)
		self.surface = surface
		self.children = []
		self.spritegroup = pygame.sprite.LayeredUpdates()
		self.transparent = transparent
		if self.transparent != True:
			self.add_panel()

	def click(self, x, y):
		for child in self.children:
			propagate = child.click(x, y)
			if not propagate:
				return True

	def update(self):
		for child in self.children:
			child.update()

	def clear(self):
		self.children = []
		self.spritegroup = pygame.sprite.LayeredUpdates()

	def draw(self):
		if self.transparent != True:
			self.surface.blit(self.border[0], self.border[1])
			self.surface.blit(self.background[0], self.background[1])
		self.spritegroup.update()
		self.spritegroup.draw(self.surface)
		#for child in self.children:
		#	child.draw()
			
	def add_panel(self):
		self.border = self.add_round_rect(pygame.Rect(self.x, self.y, self.width + (ICON_BORDER * 2), self.height + (ICON_BORDER * 2)), COLOR_BORDER, ICON_RADIUS)
		self.background = self.add_round_rect(pygame.Rect(self.x + ICON_BORDER, self.y + ICON_BORDER, self.width, self.height), COLOR_BG, ICON_RADIUS)
		
	def add_round_rect(self, rect, color, radius=10):

		"""
		AAfilledRoundedRect(surface,rect,color,radius=0.4)

		surface : destination
		rect    : rectangle
		color   : rgb or rgba
		radius  : 0 <= radius <= 1
		"""

		rect         = Rect(rect)
		color        = Color(*color)
		alpha        = color.a
		color.a      = 0
		pos          = rect.topleft
		rect.topleft = 0,0
		rectangle    = pygame.Surface(rect.size,SRCALPHA)

		circle       = pygame.Surface([min(rect.size)*3]*2,SRCALPHA)
		pygame.draw.ellipse(circle,(0,0,0),circle.get_rect(),0)
		circle       = pygame.transform.smoothscale(circle,[int(radius)]*2)

		radius              = rectangle.blit(circle,(0,0))
		radius.bottomright  = rect.bottomright
		rectangle.blit(circle,radius)
		radius.topright     = rect.topright
		rectangle.blit(circle,radius)
		radius.bottomleft   = rect.bottomleft
		rectangle.blit(circle,radius)

		rectangle.fill((0,0,0),rect.inflate(-radius.w,0))
		rectangle.fill((0,0,0),rect.inflate(0,-radius.h))

		rectangle.fill(color,special_flags=BLEND_RGBA_MAX)
		rectangle.fill((255,255,255,alpha),special_flags=BLEND_RGBA_MIN)

		return [rectangle, pos]

# XXX: Deprecated
class UIComponent(UIObject):
	def __init__(self, x, y, width, height):
		UIObject.__init__(self, None, (x, y), (width, height))

class Tile(pygame.sprite.Sprite):
	def __init__(self, image, rect=None, layer=0):
		pygame.sprite.Sprite.__init__(self)
		self.image = image
		self.rect = image.get_rect()
		self._layer = layer
		if rect is not None:
			self.rect = rect

class FuncButton(UIObject, pygame.sprite.Sprite):
	def __init__(self, parent, x, y, width, height, text, image, fontsize, surface, layer, function, visible, selected, static, simple=False):
		UIObject.__init__(self, parent, (x, y), (width, height))
		pygame.sprite.Sprite.__init__(self)
		self.visible = visible
		self.function = function #Function is tuple that consists the function and also the parameter in form of (f, param)
		self.surface = surface
		self.fontsize = fontsize
		self.bg_color = COLOR_BG
		self.border_color = COLOR_BORDER
		self.selected_color = COLOR_SELECTED
		self.images = image
		self.selected = selected
		self.static = static
		
		if simple:
			self.border = [pygame.Surface((self.width + (ICON_BORDER * 2), self.height + (ICON_BORDER * 2))), pygame.Rect(0, 0, self.width + (ICON_BORDER * 2), self.height + (ICON_BORDER * 2))]
			self.border[0].fill(COLOR_BORDER)
			self.background = [pygame.Surface((self.width, self.height)), pygame.Rect(0, 0, self.width, self.height)]
			self.background[0].fill(COLOR_BG)
		else:
			self.border = self.add_round_rect(pygame.Rect(0, 0, self.width + (ICON_BORDER * 2), self.height + (ICON_BORDER * 2)), COLOR_BORDER, ICON_RADIUS)
			self.background = self.add_round_rect(pygame.Rect(ICON_BORDER, ICON_BORDER, self.width, self.height), COLOR_BG, ICON_RADIUS)
		
		self.image = self.border[0].copy()
		self._layer = layer

		self.texts = []
		if text != None:
			for t in text:
				self.add_text(t[0], t[1], self.fontsize)

	def update(self):
		self.image = self.border[0].copy()
		if self.visible:
			#self.image.blit(self.border[0], self.border[1])
			self.image.blit(self.background[0], self.background[1])
			if self.images != None:
				for i in self.images:
					try:
						rect = i[0].get_rect()
						if i[1] == None:
							rect.centerx = (self.width + (ICON_BORDER * 2))/2
							rect.centery =  (self.height + (ICON_BORDER * 2))/2
						else:
							rect.x = i[1][0]
							rect.y = i[1][1]
						self.image.blit(i[0], rect)
					except AttributeError:
						rect = i[0].rect
						if i[1] == None:
							rect.centerx = (self.width + (ICON_BORDER * 2))/2
							rect.centery =  (self.height + (ICON_BORDER * 2))/2
						else:
							rect.x = i[1][0]
							rect.y = i[1][1]
						self.image.blit(i[0].image, i[0].rect)
			for t in self.texts:
				if t[1] == None:
					self.image.blit(t[0], ((self.width + (ICON_BORDER * 2))/2, (self.height + (ICON_BORDER * 2))/2))
				else:
					self.image.blit(t[0], t[1])
		else: self.image = pygame.Surface((0,0))
				
	def add_image(self, image, coords):
		if coords == None:
			rect = image.get_rect()
			coords = ((self.width - rect.width)/2, (self.height - rect.height)/2)
		self.images.append([image, coords])

	def add_text(self, string, coords, fontsize):
		font = pygame.font.Font(FONT, int(fontsize*FONTSCALE))
		text = font.render(string, True, COLOR_FONT)
		rect = text.get_rect()
		if coords != None:
			rect.x = coords[0]
			rect.y = coords[1]
		else:
			rect.centerx = (self.width + (ICON_BORDER * 2))/2
			rect.centery = (self.height + (ICON_BORDER * 2))/2
		self.texts.append([text, rect])
		
	def show(self):
		self.visible = True
	
	def hide(self):
		self.visible = False
		
	def select(self):
		self.selected = True
		self.border = self.add_round_rect(pygame.Rect(self.x, self.y, self.width + (ICON_BORDER * 2), self.height + (ICON_BORDER * 2)), COLOR_SELECTED, ICON_RADIUS)
		
	def unselect(self):
		self.selected = False
		self.border = self.add_round_rect(pygame.Rect(self.x, self.y, self.width + (ICON_BORDER * 2), self.height + (ICON_BORDER * 2)), COLOR_BORDER, ICON_RADIUS)
		
	def toggle(self):
		if self.selected:
			self.select()
		else:
			self.unselect()
		
	def add_round_rect(self, rect, color, radius=9):

		"""
		AAfilledRoundedRect(surface,rect,color,radius=0.4)

		surface : destination
		rect    : rectangle
		color   : rgb or rgba
		radius  : 0 <= radius <= 1
		"""

		rect         = Rect(rect)
		color        = Color(*color)
		alpha        = color.a
		color.a      = 0
		pos          = rect.topleft
		rect.topleft = 0,0
		rectangle    = pygame.Surface(rect.size,SRCALPHA)

		circle       = pygame.Surface([min(rect.size)*3]*2,SRCALPHA)
		pygame.draw.ellipse(circle,(0,0,0),circle.get_rect(),0)
		circle       = pygame.transform.smoothscale(circle,[int(radius)]*2)

		radius              = rectangle.blit(circle,(0,0))
		radius.bottomright  = rect.bottomright
		rectangle.blit(circle,radius)
		radius.topright     = rect.topright
		rectangle.blit(circle,radius)
		radius.bottomleft   = rect.bottomleft
		rectangle.blit(circle,radius)

		rectangle.fill((0,0,0),rect.inflate(-radius.w,0))
		rectangle.fill((0,0,0),rect.inflate(0,-radius.h))

		rectangle.fill(color,special_flags=BLEND_RGBA_MAX)
		rectangle.fill((255,255,255,alpha),special_flags=BLEND_RGBA_MIN)

		return [rectangle, pos]
	
	def toggle_visibility(self):
		print "Visibility toggled", self, self.visible
		if self.static == False:
			if self.visible: self.visible = False
			else: self.visible = True
			
class CascadeButton(UIComponent):
	def __init__(self, parent, surface, x, y, width, height, texts, images, name = None, function = None):
		self.parent = parent
		self.surface = surface
		#self.buttons = pygame.sprite.LayeredUpdates()
		self.child_buttons = []
		self.function = function
		self.parent_button = FuncButton(self.parent, x, y, width, height, texts, images, ICON_FONTSIZE, self.surface, 0, self.function, True, False, False)
		#self.buttons.add(self.parent_button)
		self.parent.spritegroup.add(self.parent_button)
		self.name = name
		self.childs_visible = False
		
	def add_button(self, coords, size, text, image, function, visible=False, static=False, align=None):
		if align == "centerx":
			x = coords[0] + (self.parent_button.width / 2)
			y = coords[1]
		elif align == "centery":
			x = coords[0]
			y = coords[1] + (self.parent_button.height / 2)
		elif align == "center":
			x = coords[0] + (self.parent_button.width / 2)
			y = coords[1] + (self.parent_button.height / 2)
		else:
			x = coords[0]
			y = coords[1]
		tmp = FuncButton(self.parent_button, x, y, size[0], size[1], text, image, ICON_FONTSIZE, self.surface, 1, function, visible, False, static)
		self.child_buttons.append(tmp)
		#self.buttons.add(tmp)
		self.parent.spritegroup.add(tmp)

	def button_click(self):
		print "Clicked button"
		
	def enable_buttons(self):
		for b in self.child_buttons:
			if b.static == False:
				b.toggle_visibility()
		self.childs_visible = True
			
	def hide_buttons(self):
		for b in self.child_buttons:
			if b.static == False:
				b.visible = False
		self.childs_visible = False
	"""
	def draw(self):
		print self.buttons.layers
		self.buttons.update()
		self.buttons.draw(self.surface)
	"""

class Button(UIComponent):
	def __init__(self, parent, surface, x, y, width, height, texts, images):
		self.parent = parent
		self.surface = surface
		#self.buttons = pygame.sprite.LayeredUpdates()
		#self.child_buttons = []
		self.button = FuncButton(self.parent, x, y, width, height, texts, images, ICON_FONTSIZE, self.surface, 0, self.enable_buttons, True, False, False)
		#self.buttons.add(self.button)

	def button_click(self):
		print "Clicked button"
		
	def enable_buttons(self):
		for b in self.child_buttons:
			if b.static == False:
				b.toggle_visibility()
			
	"""
	def draw(self):
		self.buttons.update()
		self.buttons.draw(self.surface)
	"""
