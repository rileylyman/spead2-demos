import spead2
import spead2.send
import numpy as np

#A thread pool defaults to one C thread per pool. Couple
#this with a stream to send packets.

threadPool = spead2.ThreadPool()

#StreamConfig class handles configuration parameter across
#all stream classes. 
#max_packet_size=1472
#rate=0.0 (fastest byte/sec send rate)
#burst_size=65535, max_heaps=4

streamConfig = spead2.send.StreamConfig(rate=1e7)

#A Udp stream sends heap packets over udp, parameters are
#threadPool, ipAddress, port, streamConfig

stream = spead2.send.UdpStream(threadPool, "127.0.0.1", 8888, streamConfig)

#Documentation says to delete thread_pools after they are assigned...
#really not sure why, will have to look into it
del threadPool

#An item group holds a number of spead2.Items, and can be passed into
#a heap generator in order to be sent in heap form through our UdpStream

itemGroup = spead2.send.ItemGroup()

#Adding items to the item group. An element of an ItemGroup object consists
#of an ItemDescriptor, which can be instatiated with the parameters we pass
#into itemGroup.add_item. It also consists of an Item object, which simply
#contains a value and a version number, which is incremented and sent
#through the stream every time the value changes. We can implicitly 
#instatiate an ItemDescription, and then an item, by doing the following

item = itemGroup.add_item(0x1000, "cat", "string containing the word cat", (), dtype=np.uint32)
item.value = 0xDEADBEEF

item = itemGroup.add_item(0x1001, "matrix", "a matrix of integers", (4,4), dtype=np.int32)
item.value = np.zeros((4,4), np.int32)

#takes a string and returns a list where each char is cast to its ASCII representation
def stringify(s):
	buff = []
	for char in s:
		buff.append(ord(char))
	return buff


item = itemGroup.add_item(0x1002, "string", "a string", (14,), dtype=np.uint8)
item.value = stringify("this is a string")

#We now associate this ItemGroup with a HeapGenerator, in order to send
#it through the stream
#a HeapGenerator is instatiated with an ItemGroup, 
#and optional parameters descriptor_frequency (int)
#and flavour (spead2.Flavour)

heapGenerator =  spead2.send.HeapGenerator(itemGroup)

#now we can generate heaps through the heap generator, and
#send them through udp. Note: in a larger application, calling
#heapGenerator.get_heap() will update the heap based on how the
#item group changes

stream.send_heap(heapGenerator.get_heap())

#we let the receiver know we are done by sending a heap which contains
#only an end-of-heap marker

stream.send_heap(heapGenerator.get_end())

#done!

