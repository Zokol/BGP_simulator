import os, sys, time
import pygame
from pygame.locals import *
from UI import *

if not pygame.font:
	print "Warning: Fonts not enabled"
if not pygame.mixer:
	print "Warning: Audio not enabled"


###
# Name: RouterList
#
# Description:
# Class for router listing
#
# Parameters:
# - routermodel, RouterModel-object that shows the selected router in UI
# - routers[], array of routers available at init
###
class RouterList:
	def __init__(self, parent, pos, surface, routermodel, routers = []):
		self.routers = routers
		self.parent = parent
		self.pos = pos
		self.surface = surface

		self.routermodel = routermodel

		self.container = UIContainer(self.parent, self.pos, (200, 280), self.surface, False)
		if len(self.routers) > 0:
			self.update_list()

		self.buttons = []
		self.router_buttons = []
		btn = FuncButton(self.container, self.container.x + 10, self.container.y + self.container.height - 30, 150, 20, [["New router", None]], None, ICON_FONTSIZE, self.surface, 1, (self.add_router, None), True, False, True)
		self.buttons.append(btn)
		self.container.spritegroup.add(btn)

	def draw(self):
		self.container.draw()

	def add_router(self, router):
		if router == None:
			self.routers.append(Router("testrouter", [Interface(Fifo(8), Fifo(8))]*4))
		else:
			self.routers.append(router)
		self.update_list()

	def update_list(self):
		self.router_buttons = []
		x = 25
		y = 25
		for i in range(len(self.routers)):
			r = self.routers[i]
			btn_txt = r.name
			btn = FuncButton(self.container, x, y, 150, 20, [[btn_txt, None]], None, ICON_FONTSIZE, self.surface, 1, (self.select_router, i), True, False, True)
			self.router_buttons.append(btn)
			self.container.spritegroup.add(btn)
			y += 30

	def select_router(self, index):
		self.routermodel.select_router(self.routers[index])

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
	def __init__(self, parent, surface, pos, ports = 4, router = None, in_fifomodel = None, out_fifomodel = None):
		self.pos = pos
		self.parent = parent
		self.surface = surface
		self.router = router
		self.selected_port = None
		self.in_fifomodel = in_fifomodel
		self.out_fifomodel = out_fifomodel
		self.router_logicblock = UIContainer(parent, (self.pos[0] + 150, self.pos[1]), (200, 300), self.surface, False)
		self.ports = ports

		x_left = 0
		x_right = 400
		y = 50
		self.port_blocks = []

		for i in range(int(ports / 2)):
			self.port_blocks.append(FuncButton(parent, self.pos[0] + x_left, self.pos[1] + y, 100, 50, [["Port" + str(i * 2), None]], None, ICON_FONTSIZE, self.surface, 1, (self.select_port, i), True, False, True))
			self.port_blocks.append(FuncButton(parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["Port" + str((i * 2) + 1), None]], None, ICON_FONTSIZE, self.surface, 1, (self.select_port, i), True, False, True))
			y += 150
		if (ports % 2):
			self.port_blocks.append(FuncButton(parent, self.pos[0] + x_left, self.pos[1] + y, 100, 50, [["Port" + str(i + 1), None]], None, ICON_FONTSIZE, self.surface, 1, (self.select_port, i), True, False, True))
		for b in self.port_blocks:
			self.router_logicblock.spritegroup.add(b)

	def draw(self):
		self.router_logicblock.draw()

	def select_router(self, router):
		print "Selected ", router.name
		self.router = router
		if len(self.router.interfaces) > self.ports:
			print "Error - Not enough ports!"

	def select_port(self, index):
		if self.in_fifomodel != None and self.out_fifomodel != None:
			port = self.router.interfaces[index]
			self.selected_port = port
			self.in_fifomodel.update(port.in_fifo)
			self.out_fifomodel.update(port.in_fifo)

###
# Name: FifoModel
#
# Description:
# Class to represent network interface FIFOs
#
# Parameters:
# - fifo_length, integer lenght of fifo-buffer determines the UI-block size
# - output buffer, boolean value that determines if buffer is input or output buffer. If buffer is on input-mode, it has packet-field add feed packets to the buffer
###
class FifoModel:
	def __init__(self, parent, pos, surface, fifo_length = 8, input_buffer = False):
		self.length = fifo_length
		self.mode = input_buffer
		self.parent = parent
		self.pos = pos
		self.surface = surface
		self.fifo_length = fifo_length

		self.packets = [None] * self.length

		self.container = UIContainer(self.parent, self.pos, (200, 280), self.surface, False)

	def draw(self):
		self.container.draw()

	def update(self, fifo):
		self.packet_btn = []
		x = 25
		y = 25
		for i in range(self.fifo_length):
			p = fifo.packets[i]
			if p != None:
				btn_txt = p.header
			else:
				btn_txt = "Empty"
			btn = FuncButton(self.container, x, y, 150, 20, [[btn_txt, None]], None, ICON_FONTSIZE, self.surface, 1, (self.show_packet, i), True, False, True)
			self.packet_btn.append(btn)
			self.container.spritegroup.add(btn)
			y += 30

	def show_packet(index):
		if self.packets[index] != None:
			p = self.packets[index]
			print p.header, p.payload


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
	def __init__(self, in_fifo, out_fifo):
		self.in_fifo = in_fifo
		self.out_fifo = out_fifo

class Fifo:
	def __init__(self, length):
		self.packets = [Packet("None", "None")]*length

	def write(self, packet):
		for p in self.buf:
			if p == None:
				p = packet
				break
		else:
			print "Error - FIFO Full!"

	def read(self):
		packet = self.buf[0]
		for i in range(0, len(self.buf) - 1):
			self.buf[i] = self.buf[i + 1]
		return packet

class SimulationUI:
	def __init__(self, screen):
		pygame.display.set_caption('Protocol Processing - Keepalive UI')

		self.screen = screen
		self.width, self.height = self.screen.get_size()

		self.background = pygame.Surface((self.width, self.height))
		self.background.fill((200, 200, 200))

		self.fps = 30
		self.clock = pygame.time.Clock()

		routers = []
		self.init_routerobject = Router("testrouter", [Interface(Fifo(8), Fifo(8))]*4)
		self.in_fifomodel = FifoModel(None, (10, 400), self.screen, 8, True)
		self.out_fifomodel = FifoModel(None, (300, 400), self.screen, 8, False)
		self.routermodel = RouterModel(None, self.screen, (300, 10), 4, None, self.in_fifomodel, self.out_fifomodel)
		self.routerlist = RouterList(None, (10, 10), self.screen, self.routermodel)
		self.routerlist.add_router(self.init_routerobject)

	def draw(self):
		self.screen.blit(self.background, (0,0))
		self.routerlist.draw()
		if self.routermodel.router != None:
			self.routermodel.draw()
			if self.routermodel.selected_port != None:
				self.in_fifomodel.draw()
				self.out_fifomodel.draw()
		pygame.display.flip()

	def click(self, event):
		for b in self.routerlist.router_buttons:
			if b.contains(*event.pos):
				f = b.function[0]
				f(b.function[1])
		for b in self.routerlist.buttons:
			if b.contains(*event.pos):
				f = b.function[0]
				f(b.function[1])
		for b in self.routermodel.port_blocks:
			if b.contains(*event.pos):
				f = b.function[0]
				f(b.function[1])

	def loop(self):
		self.done = False
		while not self.done:
			for event in pygame.event.get():
				if event.type == pygame.MOUSEBUTTONDOWN:
					if event.button == 1:
						self.click(event)
				if event.type == pygame.QUIT:
					self.done = True
			self.draw()

if __name__ == "__main__":
	screen = init_pygame((1200, 700))

	win = SimulationUI(screen)
	win.loop()

