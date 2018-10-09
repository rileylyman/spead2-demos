#include <iostream>
#include <utility>
#include <boost/asio.hpp>
#include <spead2/common_endian.h>
#include <spead2/common_thread_pool.h>
#include <spead2/common_defines.h>
#include <spead2/common_flavour.h>
#include <spead2/send_heap.h>
#include <spead2/send_udp.h>
#include <spead2/send_stream.h>

int main() {
    /*A group of C threads that all streams need in order to be constructed. In this case we just use one.*/
    spead2::thread_pool thread_pool;

    /*Resolve the udp query to establish the connection to another server.*/
    boost::asio::ip::udp::resolver resolver(thread_pool.get_io_service());
    boost::asio::ip::udp::resolver query("10.11.0.153", "8888");
    auto it = resolver.resolve(query);

    /*Construct a udp stream. We pass in the thread_pool's io_service, the resolved udp query, and a new stream_config instance.*/
    spead2::send::udp_stream stream(thread_pool.get_io_service(), *it, spead2::send::stream_config(9000, 0));

    /*Construct a heap that we will add item and item descriptors to, which will then be sent off across udp.*/
    spead2::send::heap heap;

    std::int32_t some_integer = 0xDEADBEEF;
    spead2::descriptor desc;
    desc.id = 0x1000;
    desc.name = "first integer";
    desc.description = "some dead beef";
    desc.format.emplace_back('i', 32);

    /*We add our item and descriptor to the heap. Notice that their ids are the same.*/
    heap.add_item(0x1000, &some_integer, sizeof(some_integer), true);
    heap.add_descriptor(desc);

    send(heap);

    /*Create a heap that lets the receiver know we are done, and send that.*/
    spead2::send::heap end;
    end.add_end();

    send(end);

    /*Blocks the program until all enqueued heaps have been flushed.*/
    stream.flush(); 

}

void send(spead2::send::heap & heap) { 
    stream.async_send_heap(heap, 
        [] ( const boost::system::error_code &error, spead2::item_pointer_tbytes_transferred)         { 
            if (error) { std::cerr << ec.message() << '\n'; }
        }   
}
