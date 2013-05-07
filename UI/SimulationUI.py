"""
 # \file 	SimulationUI.py
 # \brief	User interface for BGP simulation
 # \details	Sets the initial configuration and gives feedback for user about the simulation
 # \author	Heikki Juva, 76718
 # \version	1.5
 # \date	24.4.2013
"""

import os, sys, time
import pygame
from pygame.locals import *
from UI import *
from selectdialog import *
from timeout import timeout
import eztext
import socket
import math

if not pygame.font:
	print "Warning: Fonts not enabled"
if not pygame.mixer:
	print "Warning: Audio not enabled"

class NameList(TextList):
	def format_item(self, item):
		return item.name


###
# \brief	RouterModel
#
# \details
# Class to represent visual model of an simulated router
# RouterModel uses InterfaceModel-class to handle FIFOs
#
# Parameters: 
# \li ports, number of network interfaces in router
# \li router, router-object that is shown in model
###
class RouterModel(UIObject):
	def __init__(self, parent, surface, pos, router = None):
		self.pos = pos
		self.parent = parent
		self.surface = surface
		self.router = router
		self.selected_port = None
		self.ezfont = pygame.font.Font(FONT, int(15*FONTSCALE))
		self.router_logicblock = UIContainer(parent, (self.pos[0] + 100, self.pos[1]), (200, 300), self.surface, False)

		self.port_blocks = []
		self.update_ports()

	def draw(self):
		self.update_ports()
		self.router_logicblock.draw()
		self.update_text()

	def select_router(self, router):
		self.router = router

	def update_ports(self):
		x_left = 0
		x_right = 303
		y = 37
		for b in self.port_blocks:
			self.router_logicblock.spritegroup.remove(b)
		self.port_blocks = []
		self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["P0 AS: " + self.router.get_client_id(0), None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		y += 88
		self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["P1 AS: " + self.router.get_client_id(1), None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		y += 88
		self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["P2 AS: " + self.router.get_client_id(2), None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		#y += 150
		#self.port_blocks.append(FuncButton(self.parent, self.pos[0] + x_right, self.pos[1] + y, 100, 50, [["Local AS: " + self.router.as_id, None]], None, ICON_FONTSIZE, self.surface, 1, None, True, False, True))
		for b in self.port_blocks:
			self.router_logicblock.spritegroup.add(b)

	def update_text(self):
		font = pygame.font.Font(FONT, int(25*FONTSCALE))
		text = font.render("AS id: " + self.router.as_id, True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = self.pos[0] + 200
		rect.centery = 50
		self.router_logicblock.surface.blit(text, rect)
		text = font.render("Prefix: " + self.router.prefix, True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = self.pos[0] + 200
		rect.centery = 80
		self.router_logicblock.surface.blit(text, rect)
		text = font.render("MED: " + self.router.med, True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = self.pos[0] + 200
		rect.centery = 110
		self.router_logicblock.surface.blit(text, rect)
		text = font.render("Local preference: " + self.router.localpref, True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = self.pos[0] + 200
		rect.centery = 140
		self.router_logicblock.surface.blit(text, rect)
		text = font.render("Keepalive time: " + self.router.keepalivetime, True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = self.pos[0] + 200
		rect.centery = 170
		self.router_logicblock.surface.blit(text, rect)
		text = font.render("Holddown multiplier: " + self.router.holddown_multiplier, True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = self.pos[0] + 200
		rect.centery = 200
		self.router_logicblock.surface.blit(text, rect)

"""
# \brief Simulation-objects
# \details Classes that mimic the simulation object-classes, simulation data is stored in these classes and data is synchronized via socket connection.
"""

class Packet:
	def __init__(self, header, payload):
		self.header = header
		self.payload = payload
	@property
	def name(self):
		return self.header + " | " + self.payload

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
		self.as_id = str(as_id)
		self.prefix = str(prefix)
		self.med = str(med)
		self.localpref = str(localpref)
		self.keepalivetime = str(keepalivetime)
		self.holddown_multiplier = str(holddown_multiplier)
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
		self.as_packets = []
	
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
		self.log.insert(0,self.prompt + cmd)

	def add_log(self, msg):
		self.log.insert(0,msg)

class RoutingTable:
	def __init__(self):
		self.table = []

	def update_table(self, data):
		self.table = []
		#raw_table = data.split(";")
		for row in data:
			print row
			route = row.split(',')
			if len(route) == 4:
				self.table.append(Route(route[1], route[2], route[3]))
		print self.table
		for t in self.table:
			print t.name

"""
# \brief SimulationUI
# \details 
# \li Defines the pygame-objects. 
# \li Initializes simulation-objects to handle the data.
# \li Creates socket to communicate with the simulation software.
"""

class SimulationUI:
	def __init__(self, screen):
		pygame.display.set_caption('Protocol Processing - Keepalive UI')

		self.screen = screen
		self.width, self.height = self.screen.get_size()

		self.background = pygame.Surface((self.width, self.height))
		self.background.fill((200, 200, 200))

		self.fps = 30
		self.clock = pygame.time.Clock()

		self.sim_running = False
		self.sim_stopped = False

		##Simulation objects
		self.next_routerID = 0
		self.selected_router = None
		self.connection_router1 = None
		self.connection_router2 = None
		self.all_routes = []
		self.main_routing_table = []
		self.packet_list = []
		self.routers = []
		self.console = Console()
		self.routing_table_main = RoutingTable()
		self.routing_table_all = RoutingTable()
		#self.init_routerobject = Router()
		#self.routers.append(self.init_routerobject)
		self.add_router(None)
		self.selected_router = self.routers[0]
		self.routermodel = RouterModel(None, self.screen, (335, 25), self.routers[0])

		##Socket
		self.host = "localhost"
		self.port = 50000
		self.size = 1024
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		try:
			self.socket.connect((self.host, self.port))
			#self.socket.send('UI_INIT')
			#resp = self.socket.recv(self.size)
			#print resp
		except socket.error:
			print "No connection to server, please ensure that simulation server is running at ",self.host,":",self.port
			sys.exit(1)

		##UI Elements
		self.buttons = []
		self.control_con = UIContainer(None, (5,365), (210, 330), self.screen, False)
		self.routerlist_con = UIContainer(None, (5,25), (210, 300), self.screen, False)
		self.packetlist_con = UIContainer(None, (230,25), (210, 300), self.screen, False)
		self.network_con = UIContainer(None, (750,25), (430, 300), self.screen, False)
		self.routerlist_dialog = NameList(self.routerlist_con, (15,25), (184, 220), self.routers, selected = self.select_router)
		self.console_dialog = TextList(None, (230,365), (300, 320), self.console.log)
		self.packetlist_dialog = NameList(None, (237,32), (193, 237), self.packet_list)
		self.ezfont = pygame.font.Font(FONT, int(15*FONTSCALE))
		self.console_input = eztext.Input(None, (230, 680), (300, 15), maxlength=50, color=COLOR_FONT, prompt='cmd> ', font = self.ezfont, handle_enter = self.send_cmd)
		
		self.routing_table_main_dialog = NameList(None, (545,365), (320, 330), self.main_routing_table)
		self.routing_table_all_dialog = NameList(None, (875,365), (320, 330), self.all_routes)

		self.texts = []
		font = pygame.font.Font(FONT, int(25*FONTSCALE))
		text = font.render("Routerlist", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 108
		rect.y = 3
		self.texts.append([text, rect])
		text = font.render("AS ID", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 108
		rect.y = 27
		self.texts.append([text, rect])
		text = font.render("Received packets", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 335 
		rect.y = 3
		self.texts.append([text, rect])
		text = font.render("Router conf", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 530
		rect.y = 3
		self.texts.append([text, rect])
		text = font.render("Ports", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 690
		rect.y = 3
		self.texts.append([text, rect])
		text = font.render("Network", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 970
		rect.y = 3
		self.texts.append([text, rect])

		font = pygame.font.Font(FONT, int(30*FONTSCALE))
		text = font.render("Controlpanel", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 110
		rect.y = 340
		self.texts.append([text, rect])
		text = font.render("Console", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 372
		rect.y = 340
		self.texts.append([text, rect])
		text = font.render("Main routing table", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 710
		rect.y = 340
		self.texts.append([text, rect])
		text = font.render("All routes", True, COLOR_FONT)
		rect = text.get_rect()
		rect.centerx = 1040
		rect.y = 340
		self.texts.append([text, rect])
		
		self.sprites = pygame.sprite.LayeredDirty(_time_threshold = 1000.0)
		self.sprites.set_clip()
		
		self.sprites.add(self.routerlist_dialog)
		self.sprites.add(self.console_dialog)
		self.sprites.add(self.packetlist_dialog)
		self.sprites.add(self.routing_table_main_dialog)
		self.sprites.add(self.routing_table_all_dialog)
		self.sprites.add(self.console_input)
		
		btn = FuncButton(self.routerlist_con, 15, self.routerlist_con.y + self.routerlist_con.height - 70, 180, 30, [["New router", None]], None, 25, self.screen, 1, (self.add_router, None), True, False, True)
		self.buttons.append(btn)
		self.routerlist_con.spritegroup.add(btn)
		btn = FuncButton(self.packetlist_con, 15, self.packetlist_con.y + 230, 180, 30, [["Clear packets", None]], None, 25, self.screen, 1, (self.clear_packets, self.selected_router), True, False, True)
		self.buttons.append(btn)
		self.packetlist_con.spritegroup.add(btn)
		btn = FuncButton(self.control_con, 10, 10, 190, 40, [["START", None]], None, 30, self.screen, 1, (self.start_sim, None), True, False, True)
		self.buttons.append(btn)
		self.control_con.spritegroup.add(btn)
		btn = FuncButton(self.control_con, 10, 70, 190, 40, [["STOP", None]], None, 30, self.screen, 1, (self.stop_sim, None), True, False, True)
		self.buttons.append(btn)
		self.control_con.spritegroup.add(btn)


	def add_router(self, router):
		if router == None:
			self.log("New router added")
			self.routers.append(Router(self.next_routerID))
			self.next_routerID += 1
		else:
			self.log("Router added: " + router.name)
			self.routers.append(router)

	def clear_packets(self, router):
		if self.sim_running:
			for i in range(len(self.routers)):
				if self.routers[i] == router:
					cmd = "<CMD>CLEAR_PACKETS," + str(i) + "</CMD>"
					print cmd
					self.socket.send(cmd)
					if not self.wait_for("ACK"):
						self.log("Server returned an error, please try again.")
						return 0
					self.log("Packets cleared")
					return 1

	def send_cmd(self):
		cmd = self.console_input.value
		self.console.send(cmd)
		self.console_input.value = ''
		
		if cmd == 'help':
			self.print_help()
		else:
			if self.sim_running:
				params = cmd.split(' ')
				print params
				if params[0] == "connect" and len(params) == 2:
					r1 = params[1].split(':')
					self.log(self.connect((self.get_router(r1[0]), int(r1[1])), (self.get_router(r2[0]), int(r2[1]))))
				elif params[0] == "disconnect" and len(params) == 2:
					tmp = params[1].split(':')
					self.log(self.disconnect(tmp[0], tmp[1]))
				elif params[0] == "send_packet" and len(params) == 4:
					self.send_packet(params[1], params[2], params[3])
				elif params[0] == "kill_router" and len(params) == 2:
					self.log("No revive method created")
				elif params[0] == "revive_router" and len(params) == 2:
					self.log("No revive method created")
				elif params[0] == "reset_router" and len(params) == 2:
					self.log("No revive method created")
				elif params[0] == "start" and len(params) == 1:
					self.log("Simulation is already running")
				elif params[0] == "stop" and len(params) == 1:
					self.log(self.stop_sim())
				elif params[0] == "debug":
					params = cmd.split(" ", 1)
					self.log(self.send_socket_cmd(str(params[1])))
				else:
					self.log(str(params[0]) + ": command not found or invalid parameters")
					self.print_help()

			elif not self.sim_stopped:
				params = cmd.split(' ')
				print params
				if params[0] == "connect" and len(params) == 3:
					r1 = params[1].split(':')
					r2 = params[2].split(':')
					if len(r1) == 2:
						self.log(self.connect((self.get_router(r1[0]), int(r1[1])), (self.get_router(r2[0]), int(r2[1]))))
					elif len(r1) == 1:
						self.log(self.connect((self.get_router(r1[0]), None), (self.get_router(r2[0]), None)))
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
					self.log("Run start-command first")
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
				elif params[0] == "debug":
					params = cmd.split(" ", 1)
					self.log(self.send_socket_cmd(str(params[1])))
				elif params[0] == "test":
					print self.routers
					print self.selected_router
					print self.routers[int(params[1])]
					self.log(self.routers[int(params[1])].as_id)
					self.routers[int(params[1])].as_id = "testing"
					self.log(self.routers[int(params[1])].as_id)
				elif params[0] == "update_network" and len(params) == 1:
					self.draw_network()
				else:
					self.log(str(params[0]) + ": command not found or invalid parameters")
					self.print_help()

	def log(self, msg):
		print msg
		self.console.add_log(msg)

	def print_help(self):
		if self.sim_running:
			self.print_sim_help()
		else:
			self.print_conf_help()

	def print_conf_help(self):
		self.log("Set AS-id: set_id [AS_ID]")
		self.log("Set prefix: set_prefix [PREFIX]")
		self.log("Set MED: set_med [MED]")
		self.log("Set Local Pref: set_lp [LocalPref]")
		self.log("Set Keepalive Time: set_kt [Keepalive]")
		self.log("Set Hold Down Multiplier: set_hdm [multiplier]")
		self.log("Connect routers: connect [AS1_ID]{:[PORT_ID]} [AS2_ID]{:[PORT_ID]}")
		self.log("Disconnect routers: disconnect [AS_ID]:[port_num]")
		self.log("Start simulation: start")
		self.log("Send command to socket server: debug [CMD]")

	def print_sim_help(self):
		self.log("Reset router config: reset_router [AS_ID]")
		self.log("Disable router: kill_router [AS_ID]")
		self.log("Revive router: revive_router [AS_ID]")
		self.log("Send packet: send_packet [DEST_PREFIX] [AS_ID] [PAYLOAD]")
		self.log("Disconnect route: disconnect [AS_ID]:[PORT_ID]")
		self.log("Re-enable route: connect [AS_ID]:[PORT_ID]")
		self.log("Stop simulation: stop")
		self.log("Send command to socket server: debug [CMD]")

	def connect(self, router1, router2):
		if router1[1] == None and router2[1] == None:
			router1_port = self.next_free_port(router1[0])
			print router1_port
			if router1_port == -1:
				return "No free ports, specify port to be reconnected"
			router2_port = self.next_free_port(router2[0])
			if router2_port == -1:
				return "No free ports, specify port to be reconnected"
			try:
				port_a = router1[0].interfaces[router1_port]
				port_a.client = router2[0]
				port_a.client_port = router2_port
				port_b = router2[0].interfaces[router2_port]
				port_b.client = router1[0]
				port_b.client_port = router1_port
			except IndexError:
				return "Error: Invalid router or port selected"
		else:
			try:
				port_a = router1[0].interfaces[router1[1]]
				port_a.client = router2[0]
				port_a.client_port = router2[1]
				port_b = router2[0].interfaces[router2[1]]
				port_b.client = router1[0]
				port_b.client_port = router1[1]
			except IndexError:
				return "Error: Invalid router or port selected"

	def disconnect(self, router, port):
		client = router.interfaces[port]
		for i in client.interfaces:
			if i.client == router:
				i.client = None
		router.interfaces[port] = None

	def next_free_port(self, router):
		for i in range(len(router.interfaces) - 1):
			print router.interfaces[i].client
			if router.interfaces[i].client == None:
				return i
		return -1

	def send_packet(self, prefix, target_as, payload):
		cmd = "<CMD>SEND_PACKET," + prefix + "," + target_as + "," + payload + "</CMD>"
		print cmd
		self.socket.send(cmd)
		if not self.wait_for("ACK"):
			self.log("Server returned an error, please try again.")
			return 0

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

	def update_routing_tables(self):
		for i in range(len(self.routers)):
			if self.routers[i] == self.selected_router:
				router_index = i
		table_str = ""
		done = False
		"""
		print router_index, type(router_index)
		cmd = "<CMD>READ_TABLE," + str(router_index) + "</CMD>"
		self.socket.send(cmd)
		if not self.wait_for("ACK"):
			self.log("Server returned an error, please try again.")
			return 0
		while not done:
			resp = self.socket.recv(self.size)
			print resp
			#self.log("Received: ", resp)
			if resp.find("<TABLE>") != -1:
				table_str += resp[resp.find("<TABLE>"):]
				while True:
					resp = self.socket.recv(self.size)
					if resp.find("</TABLE>") == -1:
						table_str += resp
					else:
						table_str += resp[:resp.find("</TABLE>")]
						break
		table = table_str.split(";")
		self.selected_router.preferred_routes = table
		"""
		#Main Routing table reading returns nothing
		
		cmd = "<CMD>READ_TABLE," + str(router_index) + "</CMD>"
		#print cmd
		self.socket.send(cmd)
		table = self.socket.recv(self.size)
		table = table[(table.find("<TABLE>")+7):table.find("</TABLE>")]
		table = table.split(";")
		#print table
		del self.main_routing_table[:]
		self.main_routing_table.append(Route("TARGET_PREFIX", "TARGET", "ROUTE"))
		for row in table:
			route = row.split(',')
			if len(route) == 4:
				self.main_routing_table.append(Route(route[1], route[2], route[3]))
		
		cmd = "<CMD>READ_RAW_TABLE," + str(router_index) + "</CMD>"
		#print cmd
		self.socket.send(cmd)
		###XXX Simulation does not send ACK, fix this later
		#if not self.wait_for("ACK"):
		#	self.log("Server returned an error, please try again.")
		#	return 0
		"""
		while not done:
			resp = self.socket.recv(self.size)
			print resp
			#self.log("Received: ", resp)
			if resp.find("<TABLE>") != -1:
				table_str += resp[resp.find("<TABLE>"):]
				while True:
					print table_str
					resp = self.socket.recv(self.size)
					if resp.find("</TABLE>") == -1:
						table_str += resp
					else:
						table_str += resp[:resp.find("</TABLE>")]
						break
		table = table_str.split(";")
		"""
		table = self.socket.recv(self.size)
		table = table[(table.find("<TABLE>")+7):table.find("</TABLE>")]
		table = table.split(";")
		del self.all_routes[:]
		self.all_routes.append(Route("TARGET_PREFIX", "TARGET", "ROUTE"))
		for row in table:
			route = row.split(',')
			if len(route) == 4:
				self.all_routes.append(Route(route[1], route[2], route[3]))
	
	def update_packetlist(self):
		for i in range(len(self.routers)):
				if self.routers[i] == self.selected_router:
					router_index = i
		table_str = ""
		done = False
		self.socket.send("<CMD>READ_PACKET," + str(router_index) + "</CMD>")
		#if not self.wait_for("ACK"):
		#	self.log("Server returned an error, please try again.")
		#	return 0
		"""
		while not done:
			resp = self.socket.recv(self.size)
			print resp
			#self.log("Received: ", resp)
			if resp.find("<TABLE>") != -1:
				table_str += resp[resp.find("<TABLE>"):]
				while True:
					resp = self.socket.recv(self.size)
					if resp.find("</TABLE>") == -1:
						table_str += resp
					else:
						table_str += resp[:resp.find("</TABLE>")]
						break
		"""
		table = self.socket.recv(self.size)
		table = table[(table.find("<TABLE>")+7):table.find("</TABLE>")]
		table = table.split(";")
		del self.packet_list[:]
		self.packet_list.append(Packet("SOURCE | TARGET", "PAYLOAD"))
		for row in table:
			print row
			packet = row.split(',')
			if len(packet) == 16:
				self.packet_list.append(Packet(packet[13] + "|" + packet[14], packet[15]))
		#	route = row.split(',')
		#	if len(route) == 2:
		#		self.all_routes.append(Route(route[1], route[2], route[3]))
	
	def start_sim(self, none = None):
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
		if not self.wait_for("ACK"):
			self.log("Server returned an error, please try again.")
			return 0
		print "<CMD>START</CMD>"
		self.socket.send("<CMD>START</CMD>")
		if not self.wait_for("ACK"):
			self.log("Server returned an error, please try again.")
			return 0
		self.sim_running = True
		self.draw_controlpanel()

	def stop_sim(self, none = None):
		self.socket.send("<CMD>STOP</CMD>")
		###XXX Simulation does not send ACK, fix this when it does
		"""
		if not self.wait_for("ACK"):
			self.log("Server returned an error, please try again.")
			return 0
		"""
		self.sim_running = False
		self.sim_stopped = True
		self.draw_controlpanel()

	#Debug-function, raw send and receive. Notice that receive length is fixed and this function does not wayt for any responce
	def send_socket_cmd(self, cmd):
		self.socket.send(cmd)
		resp = self.socket.recv(self.size)
		return "Received: " + resp

	#Default function to use when setting config while simulation is running, waits for proper responce from server before continuing
	def send_config(self, cmd):
		self.socket.send(cmd)
		resp = self.socket.recv(self.size)
		if not self.wait_for("ACK"):
			self.log("Server returned an error, please try again.")
			return 0

	#Function to use for waiting server responce
	@timeout(5) # Timeout after 5 seconds.
	def wait_for(self, cmd):
		done = False
		while not done:
			resp = self.socket.recv(self.size)
			self.log(resp)
			if resp.find(cmd) != -1:
				done = True
			elif resp.find("NACK") != -1:
				return False
		return True
		self.log(cmd + " OK")

	def select_router(self, namelist, event):
		sel = namelist.get_selected()
		if sel is not None:
			router = namelist.items[sel]
			self.log("Selected: " + router.name)
			self.routermodel.select_router(router)
			self.selected_router = router
			#self.all_routes = self.selected_router.routing_table
			#self.main_routing_table = self.selected_router.preferred_routes
			#print router, router.name, router.as_id, router.prefix, router.interfaces, router.routing_table, router.preferred_routes
		else:
			# Unselect
			self.routermodel.select_router(None)
			self.selected_router = None

	def get_router(self, name):
		for r in self.routers:
			if r.as_id == name:
				return r

	def draw_network(self):
		x = 968
		y = 175
		max_x = 300
		x_step = 40
		y_step = 40
		max_radius = 15
		a = 190
		b = 124
		r_fontsize = 20
		#angles = []
		router_points = []
		#r_x = x
		#r_y = y
		self.network_con.draw()
		d = ((math.pi*2)/len(self.routers))
		i = 0
		#for i in range(len(self.routers)):
		#	angles.append(a*i)
		if len(self.routers) > 1:
			r = math.tan((d/2))*b
			if r <= (max_radius + 1):
				radius = int((r-1))
				r_fontsize = int((radius*1.3))
			else:
				radius = max_radius
			#print r
			#print radius
		else:
			radius = max_radius
		for r in self.routers:
			router_points.append([r.as_id, int(x + a*math.cos(d*i)), int(y + b*math.sin(d*i))])
			i += 1
			#Find out next coords. Implement Elliptic-curve/line intersection here
			"""
			if (r_x - x) < max_x:
				r_x += (x_step + (radius*2)) 
			else:
				r_x = x
				r_y += (y_step + (radius*2))
			"""
		for r_id in range(len(self.routers)):
			r = self.routers[r_id]
			coord_a = (router_points[r_id][1], router_points[r_id][2])
			for p in r.interfaces:
				if p.client != None:
					for i in router_points:
						if i[0] == p.client.as_id:	
							coord_b = (i[1], i[2])
							pygame.draw.aaline(self.screen, (0,0,0), coord_a, coord_b, 2)
							break
		for c in router_points:	
			pygame.draw.circle(self.screen, (255,255,255), (c[1], c[2]), radius)
			pygame.draw.circle(self.screen, (0,0,100), (c[1], c[2]), radius-2)
			font = pygame.font.Font(FONT, int(r_fontsize*FONTSCALE))
			text = font.render(c[0], True, (255,255,255))
			rect = text.get_rect()
			rect.centerx = c[1]
			rect.centery = c[2]
			self.screen.blit(text, rect)
		#rect = pygame.Rect((777,32), (380, 250))
		#ellipse = pygame.draw.ellipse(self.screen, (0,0,0), rect, 5)
		return router_points, radius

	def draw_controlpanel(self):
		self.control_con.draw()
		color_run = (0,125,0)
		color_wait = (20,20,20)
		color_stop = (125,0,0)
		if self.sim_running:
			rect = pygame.Rect(self.control_con.x + 10, self.control_con.y + 120, 195, 205)
			pygame.draw.rect(self.screen, color_run, rect, 0)
			font = pygame.font.Font(FONT, 50)
			text = font.render("Running", True, (100,100,100))
			text = pygame.transform.rotate(text, 30)
			rect = text.get_rect()
			rect.centerx = 110
			rect.centery = 580
			self.screen.blit(text, rect)
			font = pygame.font.Font(FONT, 49)
			text = font.render("Running", True, (255,255,255))
			text = pygame.transform.rotate(text, 30)
			rect = text.get_rect()
			rect.centerx = 110
			rect.centery = 580
			self.screen.blit(text, rect)
		elif self.sim_stopped:
			rect = pygame.Rect(self.control_con.x + 10, self.control_con.y + 120, 195, 205)
			pygame.draw.rect(self.screen, color_stop, rect, 0)
			font = pygame.font.Font(FONT, 50)
			text = font.render("Stopped", True, (100,100,100))
			text = pygame.transform.rotate(text, 30)
			rect = text.get_rect()
			rect.centerx = 110
			rect.centery = 580
			self.screen.blit(text, rect)
			font = pygame.font.Font(FONT, 49)
			text = font.render("Stopped", True, (255,255,255))
			text = pygame.transform.rotate(text, 30)
			rect = text.get_rect()
			rect.centerx = 110
			rect.centery = 580
			self.screen.blit(text, rect)
		else:
			rect = pygame.Rect(self.control_con.x + 10, self.control_con.y + 120, 195, 205)
			pygame.draw.rect(self.screen, color_wait, rect, 0)
			font = pygame.font.Font(FONT, 50)
			text = font.render("Waiting", True, (100,100,100))
			text = pygame.transform.rotate(text, 30)
			rect = text.get_rect()
			rect.centerx = 110
			rect.centery = 580
			self.screen.blit(text, rect)
			font = pygame.font.Font(FONT, 49)
			text = font.render("Waiting", True, (255,255,255))
			text = pygame.transform.rotate(text, 30)
			rect = text.get_rect()
			rect.centerx = 110
			rect.centery = 580
			self.screen.blit(text, rect)

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
		for p in range(len(self.topology_points)):
			mouse_x, mouse_y = pygame.mouse.get_pos()
			router_point = self.topology_points[p]
			print mouse_x, mouse_y
			print router_point
			d = math.sqrt((mouse_x - router_point[1])**2 + (mouse_y - router_point[2])**2)
			if d <= self.topology_radius:
				if self.connection_router1 == None:
					print "selected router ", p
					self.connection_router1 = self.routers[p]
				else:
					print "selected router ", p
					self.connection_router2 = self.routers[p]
					self.log(self.connect((self.connection_router1, None), (self.connection_router2, None)))
					self.connection_router1 = None
					self.connection_router2 = None

	"""
	# \brief Simulation UI loop
	# \details Main loop that updates the UI
	# \li Clears the screen and draws the containers that divide window in smaller partitions 
	# \li Updates network topology-view
	# \li Starts the loop
	# Inside the loop: 
	# \li Update network topology.
	# \li Update routing table-lists. 
	# \li Check if there are evts for console of buttons.
	# \li Draw the router-model to show parameters for selected router.
	# \li Update the router selection dialog.
	"""
	def loop(self):
		self.done = False
		self.screen.blit(self.background, (0,0))
		#self.control_con.draw()
		self.draw_controlpanel()
		self.routerlist_con.draw()
		self.packetlist_con.draw()
		self.draw_network()
		for t in self.texts:
			self.screen.blit(t[0], t[1])
		while not self.done:
			if not self.sim_stopped:
				self.topology_points, self.topology_radius = self.draw_network()
				if self.sim_running:
					self.update_routing_tables()
					self.update_packetlist()
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
			else:
				for event in pygame.event.get():
					if event.type == pygame.QUIT:
						self.done = True

"""
# \brief Init and loop
# \details If UI is run, set the window to 1200*700, run initialization for Simulation UI and run the loop.
"""

if __name__ == "__main__":
	screen = init_pygame((1200, 700))

	win = SimulationUI(screen)
	win.loop()

