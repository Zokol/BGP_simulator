#!/usr/bin/env python

"""
A simple echo server
"""
import socket

host = ''
port = 50000
backlog = 5
size = 1024
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)
print "Server started"
while 1:
	client, address = s.accept()
	data = client.recv(size)
	if data:
		#client.send(data)
		print "Received", data
		try:
			a = data.split('<')
			b = []
			for i in a:
				x = i.split('>')
				for j in x:
					b.append(j)
			print b
			start_tag = b[0]
			end_tag = b[2]
			payload = b[1]
			if start_tag == "SIM_CONFIG" and end_tag == "SIM_CONFIG":
				client.send("ACK")
		except IndexError:
			if data == "START":
				print "Simulation started"
				client.send("ACK")
			elif data == "STOP":
				print "Simulation stopped"
				client.send("ACK")
			else:
				a = data.split(' ')
				if a[0] == "READ_RAW_TABLE":
					client.send("<TABLE>0,192.168.0.1,2,3;1,100.203.0.3,40,0;2,100.239.10.20,10,25</TABLE>")
				if a[0] == "READ_TABLE":
					client.send("<TABLE>0,192.168.0.1,2,3;1,100.203.0.3,40,0</TABLE>")
			print "Invalid command, please check the client configuration."
	client.close()
