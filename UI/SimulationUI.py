import os, sys, time
import pygame
from pygame.locals import *
from UI import *
from selectdialog import *

if not pygame.font:
	print "Warning: Fonts not enabled"
if not pygame.mixer:
	print "Warning: Audio not enabled"


class NameList(TextList):
	def format_item(self, item):
		return item.name


###
# Name: RouterModel
#
# Description:
# Class to represent visual model of an simulated router
# RouterModel uses InterfaceModel-class to handle FIFOs
#
# Parameters: 
# - ports, number of network interfaces in router
# - router, router-object that is shown in model
###
class RouterModel(UIObject):
	def __init__(self, parent, surface, pos, ports = 4, router = None):
		self.pos = pos
		self.parent = parent
		self.surface = surface
		self.router = router
		self.selected_port = None
		self.router_logicblock = UIContainer(parent, (self.pos[0] + 150, self.pos[1]), (200, 300), self.surface, False)
		self.ports = ports

		x_left = 0
		x_right = 400
		y = 50
		self.port_blocks = []

		for i in range(int(ports / 2)):
			self.port_blocks.append(FuncButton(parent, self.pos[0] + x_left, self.pos[1] + y, 100, 50, [["P" + str(i * 2) + " AS: " + "None", None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
			self.port_blocks.append(FuncButton(parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["P" + str((i * 2) + 1) + " AS: " + "None", None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
			y += 150
		if (ports % 2):
			self.port_blocks.append(FuncButton(parent, self.pos[0] + x_left, self.pos[1] + y, 100, 50, [["P" + str(i + 1) + " AS: " + "None", None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		for b in self.port_blocks:
			self.router_logicblock.spritegroup.add(b)

	def draw(self):
		self.router_logicblock.draw()

	def select_router(self, router):
		self.router = router

#XXX
##
# Mock-up classes to hold the information until sockets are in place
##
#XXX

class Packet:
	def __init__(self, header, payload):
		self.header = header
		self.payload = payload

class Router:
	def __init__(self, name, interfaces):
		self.name = name
		self.interfaces = interfaces

class Interface:
	def __init__(self, client):
		self.client = client

class Console:
	def __init__(self):
		self.log = []
		self.prompt = '> '

	def send(self, cmd):
		self.log.append(self.prompt + cmd)

class RoutingTable:
	def __init__(self):
		self.table = []

class SimulationUI:
	def __init__(self, screen):
		pygame.display.set_caption('Protocol Processing - Keepalive UI')

		self.screen = screen
		self.width, self.height = self.screen.get_size()

		self.background = pygame.Surface((self.width, self.height))
		self.background.fill((200, 200, 200))

		self.fps = 30
		self.clock = pygame.time.Clock()

		self.selectdialogs = []

		self.selected_router = None
		self.routers = []
		self.console = Console()
		self.routing_table_main = RoutingTable()
		self.routing_table_all = RoutingTable()
		self.init_routerobject = Router("Router: No local AS", [Interface(None)]*4)
		
		##UI Elements
		self.buttons = []
		self.routerlist_con = UIContainer(None, (5,5), (210, 310), self.screen, False)
		self.routerlist_dialog = NameList(self.routerlist_con, (15,15), (183, 250), self.routers, selected = self.select_router)
		self.console_dialog = NameList(None, (15,350), (300, 300), self.console.log)
		
		self.routing_table_main_dialog = NameList(None, (450,350), (300, 300), self.routing_table_main.table)
		self.routing_table_all_dialog = NameList(None, (800,350), (300, 300), self.routing_table_all.table)

		self.sprites = pygame.sprite.LayeredDirty(_time_threshold = 1000.0)
		self.sprites.set_clip()
		self.sprites.add(self.routerlist_dialog)
		self.sprites.add(self.console_dialog)
		self.sprites.add(self.routing_table_main_dialog)
		self.sprites.add(self.routing_table_all_dialog)
		self.routers.append(self.init_routerobject)
		
		btn = FuncButton(self.routerlist_con, self.routerlist_con.x + 10, self.routerlist_con.y + self.routerlist_con.height - 50, 180, 30, [["New router", None]], None, ICON_FONTSIZE, self.screen, 1, (self.add_router, None), True, False, True)
		self.buttons.append(btn)
		self.routerlist_con.spritegroup.add(btn)

		self.routermodel = RouterModel(None, self.screen, (300, 10), 4, None)

	def add_router(self, router):
		print "New router added"
		print self.routers
		if router == None:
			self.routers.append(self.init_routerobject)
		else:
			self.routers.append(router)

	def select_router(self, namelist, event):
		print "select"
		sel = namelist.get_selected()
		if sel is not None:
			router = namelist.items[sel]
			self.routermodel.select_router(router)
		else:
			# Unselect
			self.routermodel.select_router(None)

	def draw_selectdialogs(self):
		self.sprites.update()
		self.sprites._spritelist.sort(key = lambda sprite: sprite._layer)
		self.sprites.draw(self.screen)
		pygame.display.flip()

	def draw(self):
		if self.routermodel.router != None:
			self.routermodel.draw()
		pygame.display.flip()

	def click(self, event):
		for b in self.buttons:
			if b.contains(*event.pos):
				f = b.function[0]
				f(b.function[1])

	def loop(self):
		self.done = False
		self.screen.blit(self.background, (0,0))
		self.routerlist_con.draw()
		while not self.done:
			for event in pygame.event.get():
				if event.type == pygame.MOUSEBUTTONDOWN:
					if event.button == 1:
						self.click(event)
				for sprite in self.sprites:
					if hasattr(sprite, 'event'):
						sprite.event(event)
				if event.type == pygame.QUIT:
					self.done = True
			self.draw()
			self.draw_selectdialogs()

if __name__ == "__main__":
	screen = init_pygame((1200, 700))

	win = SimulationUI(screen)
	win.loop()

