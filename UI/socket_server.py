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
		print data
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
				client.send("Config OK - Starting simulation")
		except IndexError:
			print "Invalid command, please check the client configuration."
	client.close()
