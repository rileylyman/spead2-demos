import spead2
import spead2.recv

#create a thread pool to allow C threads to receive packets on a stream

threadPool = spead2.ThreadPool()

#At this point, we could create a spead2.MemoryPool in order to block off
#a section of memory for the incoming heaps to be stored to. This allows 
#for higher performance, but is not needed in a small application

#create a receiving stream associated with the threadPool

stream = spead2.recv.Stream(threadPool)
del threadPool

#now we can listen on a port for udp traffic
stream.add_udp_reader(8888)

#now the stream will begin to listen on port 8888 and store spead2.recv.Heap
#objects. It is not recommended to inspect these directly. Instead,
#we will create an item group, and use its update method in order to parse
#item values from the received heaps.

itemGroup = spead2.ItemGroup()

#the spead2.recv.Stream.get() method waits until the next heap arrives,
#or until the end-of-heap marker arrives, in which case it raises
#spead2.Stopped. Iterating through a stream will repeatedly call the get
#method on that stream, until it raises spead2.Stopped

for recvdHeap in stream:
	#we now create an item out of the received heap
	itemGroup.update(recvdHeap)

#we now stop the stream

stream.stop()

#now lets iterate through the received items and print their names
#and values. We can do this by calling itemGroup.values()

for (key, item) in itemGroup.items():
	print("\nName: " + str(item.name) + "\nValue:" + str(item.value) + "\n")

		 
