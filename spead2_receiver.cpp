#include <iostream>
#include <utility>
#include <chrono>
#include <cstdint>
#include <boost/asio.hpp>
#include <spead2/common_thread_pool.h>
#include <spead2/recv_udp.h>
#include <spead2/recv_heap.h>
#include <spead2/recv_live_heap.h>
#include <spead2/recv_ring_stream.h>

int main() {
    /*All streams must be apart of a thread pool, which is just a collection of C threads for doing work.*/
    spead2::thread_pool thread_pool;
    
    /*This is a simpler class of stream. Builds all live_heaps for you and places them in a ring buffer. 
      we can then retrieve these frozen_heaps with stream.pop(). Shown here is the explicit definition of the
      default ring buffer used, but we can omit that to make it implicit.*/
    spead2::recv::ring_stream<ringbuffer<live_heap>> stream(thread_pool);

    /*Create a udp endpoint, and add it as the reader to the stream. Emplace reader calls the constructor of the 
      specified template type, and we pass in the to the constructor through the emplace_reader method.*/
    boost::asio::ip::udp::endpoint endpoint(bost::asio::ip::address_v4::any(), 8888);
    stream.emplace_reader<spead2::recv::udp_reader>(endpoint, spead2::recv::udp_reader::default_max_size, 8 * 1024 * 1024);

    while (true) {
        try {
            spead2::recv::heap frozen_heap = stream.pop();
            print_heap(frozen_heap); 
        }
    }   catch (spead2::ringbuffer_stopped &_) { break; }
}

void print_heap(const std::recv::heap & frozen_heap) {
    
    for (const spead2::recv::item & item : frozen_heap.get_items()) {
        std::cout << "\nID: 0x" << std::hex << item.id << std::dex << std::endl;
        std::cout << "Length (bytes): " << item.length << std::endl; 
    }
    for (const spead2::descriptor & desc : frozen_heap.get_descriptors()) {
        std::cout << "\nDescriptor Name: " << desc.name << std::endl;
        std::cout << "Description: " << desc.description << std::endl;
    }
}
