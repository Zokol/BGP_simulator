import os, sys, time
import pygame
from pygame.locals import *
from UI import *
from selectdialog import *
import eztext
import socket

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
	def __init__(self, parent, surface, pos, router = None):
		self.pos = pos
		self.parent = parent
		self.surface = surface
		self.router = router
		self.selected_port = None
		self.ezfont = pygame.font.Font(FONT, int(15*FONTSCALE))
		self.router_logicblock = UIContainer(parent, (self.pos[0] + 150, self.pos[1]), (200, 300), self.surface, False)

		self.port_blocks = []
		self.update_ports()

	def draw(self):
		self.update_ports()
		self.router_logicblock.draw()

	def select_router(self, router):
		self.router = router

	def update_ports(self):
		x_left = 0
		x_right = 400
		y = 50
		for b in self.port_blocks:
			self.router_logicblock.spritegroup.remove(b)
		self.port_blocks = []
		self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_left, self.pos[1] + y, 100, 50, [["P0| AS: " + self.router.get_client_id(0), None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["P1| AS: " + self.router.get_client_id(1), None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		y += 150
		self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_left, self.pos[1] + y, 100, 50, [["P2| AS: " + self.router.get_client_id(2), None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["Local AS: " + self.router.as_id, None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		for b in self.port_blocks:
			self.router_logicblock.spritegroup.add(b)

#XXX
##
# Mock-up classes to hold the information until sockets are in place
##
#XXX

class Packet:
	def __init__(self, header, payload):
		self.header = header
		self.payload = payload

class Interface:
	def __init__(self, client = None, port = None):
		self.client = client
		self.client_port = port

class Route:
	def __init__(self, target_as = None, prefix = None, path = None):
		self.target_as = target_as
		self.prefix = prefix
		self.path = path
		
		@property
		def name(self):
			return self.target_as + " | " + self.prefix + " | " + self.path

class Router:
	def __init__(self, as_id = '0', prefix = '0.0.0.0/32', med = '0', localpref = '100', keepalivetime = '60', holddown_multiplier = '3', interfaces = None, routing_table = None, preferred_routes = None):
		self.as_id = as_id
		self.prefix = prefix
		self.med = med
		self.localpref = localpref
		self.keepalivetime = keepalivetime
		self.holddown_multiplier = holddown_multiplier
		if routing_table != None:
			self.routing_table = routing_table
		else:
			self.routing_table = []
		if preferred_routes != None:
			self.preferred_routes = preferred_routes
		else:
			self.preferred_routes = []
		if interfaces != None:
			self.interfaces = interfaces
		else:
			self.interfaces = []
			for i in range(4):
				self.interfaces.append(Interface())
	
	@property
	def name(self):
		return self.as_id

	def set_id(self, as_id):
		self.as_id = as_id
		return "AS name set to " + str(self.as_id)

	def set_prefix(self, prefix):
		self.prefix = prefix
		return "AS prefix set to " + str(self.prefix)

	def set_med(self, med):
		self.med = med
		return "AS MED set to " + str(self.med)

	def set_lp(self, lp):
		self.localpref = lp
		return "AS Local Pref set to " + str(self.localpref)

	def set_kt(self, kt):
		self.keepalivetime = kt
		return "AS Keepalive Timer set to " + str(self.keepalivetime)

	def set_hdm(self, hdm):
		self.holddown_multiplier = hdm
		return "AS Hold Down Multiplier set to " + str(self.holddown_multiplier)

	def get_client(self, port):
		if self.interfaces[port].client == None:
			return "N/C"
		else:
			return self.interfaces[port].client

	def get_client_id(self, port):
		if self.interfaces[port].client == None:
			return "N/C"
		else:
			return self.interfaces[port].client.as_id

class Console:
	def __init__(self):
		self.log = []
		self.prompt = '> '

	def send(self, cmd):
		self.log.append(self.prompt + cmd)

	def add_log(self, msg):
		self.log.append(msg)

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

		##Simulation objects
		self.selected_router = None
		self.routers = []
		self.console = Console()
		self.routing_table_main = RoutingTable()
		self.routing_table_all = RoutingTable()
		self.init_routerobject = Router()
		self.routers.append(self.init_routerobject)
		self.selected_router = self.routers[0]
		self.routermodel = RouterModel(None, self.screen, (300, 10), self.routers[0])

		##Socket
		self.host = "localhost"
		self.port = 50000
		self.size = 1024
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		try:
			self.socket.connect((self.host, self.port))
			self.socket.send('UI_INIT')
			resp = self.socket.recv(self.size)
			print resp
		except socket.error:
			print "No connection to server, please ensure that simulation server is running at ",self.host,":",self.port
			sys.exit(1)

		##UI Elements
		self.buttons = []
		self.routerlist_con = UIContainer(None, (5,5), (210, 310), self.screen, False)
		self.routerlist_dialog = NameList(self.routerlist_con, (15,15), (183, 250), self.routers, selected = self.select_router)
		self.console_dialog = TextList(None, (15,350), (300, 300), self.console.log)
		
		self.routing_table_main_dialog = TextList(None, (450,350), (300, 300), self.selected_router.preferred_routes)
		self.routing_table_all_dialog = TextList(None, (800,350), (300, 300), self.selected_router.routing_table)
		
		self.ezfont = pygame.font.Font(FONT, int(15*FONTSCALE))

		self.console_input = eztext.Input(None, (15, 650), (300, 15), maxlength=50, color=COLOR_FONT, prompt='cmd> ', font = self.ezfont, handle_enter = self.send_cmd)

		self.sprites = pygame.sprite.LayeredDirty(_time_threshold = 1000.0)
		self.sprites.set_clip()
		self.sprites.add(self.routerlist_dialog)
		self.sprites.add(self.console_dialog)
		self.sprites.add(self.routing_table_main_dialog)
		self.sprites.add(self.routing_table_all_dialog)
		self.sprites.add(self.console_input)
		
		btn = FuncButton(self.routerlist_con, self.routerlist_con.x + 10, self.routerlist_con.y + self.routerlist_con.height - 50, 180, 30, [["New router", None]], None, ICON_FONTSIZE, self.screen, 1, (self.add_router, None), True, False, True)
		self.buttons.append(btn)
		self.routerlist_con.spritegroup.add(btn)


	def add_router(self, router):
		if router == None:
			self.log("New router added")
			self.routers.append(Router())
		else:
			self.log("Router added: " + router.name)
			self.routers.append(router)

	def send_cmd(self):
		cmd = self.console_input.value
		self.console.send(cmd)
		self.console_input.value = ''
		
		if cmd == 'help':
			self.print_help()
		else:
			params = cmd.split(' ')
			print params
			if params[0] == "connect" and len(params) == 3:
				r1 = params[1].split(':')
				r2 = params[2].split(':')
				self.log(self.connect((self.get_router(r1[0]), int(r1[1])), (self.get_router(r2[0]), int(r2[1]))))
			elif params[0] == "disconnect" and len(params) == 2:
				tmp = params[1].split(':')
				self.log(self.disconnect(tmp[0], tmp[1]))
			elif params[0] == "shutdown" and len(params) == 2:
				self.log("No shutdown method created")
			elif params[0] == "revive" and len(params) == 2:
				self.log("No revive method created")
			elif params[0] == "start" and len(params) == 1:
				self.log(self.start_sim())
			elif params[0] == "stop" and len(params) == 1:
				self.log(self.stop_sim())
			elif params[0] == "set_id" and len(params) == 2:
				self.log(self.selected_router.set_id(str(params[1])))
			elif params[0] == "set_prefix" and len(params) == 2:
				self.log(self.selected_router.set_prefix(str(params[1])))
			elif params[0] == "set_med" and len(params) == 2:
				self.log(self.selected_router.set_med(str(params[1])))
			elif params[0] == "set_lp" and len(params) == 2:
				self.log(self.selected_router.set_lp(str(params[1])))
			elif params[0] == "set_kt" and len(params) == 2:
				self.log(self.selected_router.set_kt(str(params[1])))
			elif params[0] == "set_hdm" and len(params) == 2:
				self.log(self.selected_router.set_hdm(str(params[1])))
			elif params[0] == "debug" and len(params) == 2:
				self.log(self.send_socket_cmd(str(params[1])))
			elif params[0] == "test":
				print self.routers
				print self.selected_router
				print self.routers[int(params[1])]
				self.log(self.routers[int(params[1])].as_id)
				self.routers[int(params[1])].as_id = "testing"
				self.log(self.routers[int(params[1])].as_id)
			else:
				self.log(str(params[0]) + ": command not found or invalid parameters")
				self.print_help()

	def log(self, msg):
		self.console.add_log(msg)

	def print_help(self):
		self.log("Set AS-id: set_id 123")
		self.log("Set prefix: set_prefix 0.0.0.0/32")
		self.log("Set MED: set_med 10")
		self.log("Set Local Pref: set_lp 25")
		self.log("Set Keepalive Time: set_kt 60")
		self.log("Set Hold Down Multiplier: set_hdm 3")
		self.log("Connect routers: connect AS_id1:port_num AS_id2:port_num")
		self.log("Disconnect routers: disconnect AS_id port_num")
		self.log("Shutdown router: shutdown AS_id")
		self.log("Revive router: revive AS_id")
		self.log("Start simulation: start")
		self.log("Stop simulation: stop")
		self.log("Send command to socket server: debug CMD")

	def connect(self, router1, router2):
		port_a = router1[0].interfaces[router1[1]]
		port_a.client = router2[0]
		port_a.client_port = router2[1]
		port_b = router2[0].interfaces[router2[1]]
		port_b.client = router1[0]
		port_b.client_port = router1[1]

	def disconnect(self, router, port):
		client = router.interfaces[port]
		for i in client.interfaces:
			if i.client == router:
				i.client = None
		router.interfaces[port] = None

	def add_route(self, router, route):
		router.routing_table.add(route)

	def remove_route(self, router, route):
		router.routing_table.remove(route)

	def prefer_route(self, router, route):
		router.preferred_route.add(route)

	def prefer_route_id(self, router, route_id):
		router.preferred_route.add(router.routing_table[route_id])

	def deprefer_route(self, router, route):
		router.preferred_route.remove(route)

	def prefer_route_list(self, namelist, event):
		sel = namelist.get_selected()
		if sel is not None:
			route = namelist.items[sel]
			self.log("Preferring: " + route.name)
			self.prefer_route(self.selected_router, route)

	def deprefer_route_list(self, namelist, event):
		sel = namelist.get_selected()
		if sel is not None:
			route = namelist.items[sel]
			self.log("Depreferring: " + route.name)
			self.depefer_route(self.selected_router, route)

	def start_sim(self):
		sim_conf = "<SIM_CONFIG>"
		conf = []
		for r in self.routers:
			router_params = [r.as_id, r.prefix, r.med, r.localpref, r.keepalivetime, r.holddown_multiplier]
			for i in range(0,3):
				port = r.interfaces[i]
				if port.client != None:
					client_router_id = None
					for c_r in range(len(self.routers)):
						if self.routers[c_r].as_id == port.client.as_id:
							client_router_id = c_r
					if client_router_id == None:
						self.log("Error: Could not find given router")
					else:
						router_params.append(str(i) + '_' + str(port.client_port) + '_' + str(client_router_id))
			conf.append(router_params)
		string = []
		for c in conf:
			string.append(",".join(c))
		sim_conf += ";".join(string)
		sim_conf += "</SIM_CONFIG>"
		print sim_conf
		self.socket.send(sim_conf)
		resp = self.socket.recv(self.size)

	def stop_sim(self):
		self.socket.send("STOP")
		resp = self.socket.recv(self.size)

	def send_socket_cmd(self, cmd):
		self.socket.send(cmd)
		resp = self.socket.recv(self.size)

	def select_router(self, namelist, event):
		sel = namelist.get_selected()
		if sel is not None:
			router = namelist.items[sel]
			self.log("Selected: " + router.name)
			self.routermodel.select_router(router)
			self.selected_router = router
			print router, router.name, router.as_id, router.prefix, router.interfaces, router.routing_table, router.preferred_routes
		else:
			# Unselect
			self.routermodel.select_router(None)
			self.selected_router = None

	def get_router(self, name):
		for r in self.routers:
			if r.as_id == name:
				return r

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

