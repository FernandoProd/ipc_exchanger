#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <memory>
#include <vector>
#include "protocol.hpp"


// int main() {
//     using namespace ipc::protocol; // need for us for using our ipc::protocol:functions without prefix ipc::protocol in this area
//     int64_t testTime = 1234567890123;
//     auto frame = packMessage(testTime); // auto allow us to put the type of return type of function automaticly
//     std::cout << "Frame size: " << frame.size() << std::endl;
//     auto unpacked = unpackMessage(frame);
//     if (unpacked && *unpacked == testTime) {  // * is link of int64
//         std::cout << "Protocol works correctly!" << std::endl;
//     } else {
//         std::cout << "Error in protocol" << std::endl;
//     }

//     // int64_t testtime = 1234567890001;
//     // if (testtime == testTime) {  // * is link of int64
//     //     std::cout << "Protocol works correctly!" << std::endl;
//     // } else {
//     //     std::cout << "Error in protocol" << std::endl;
//     // }
//     return 0;
// }

using boost::asio::ip::tcp;

// async client for connect to server and send some data
class Sender : public std::enable_shared_from_this<Sender> {
public:
    explicit Sender(boost::asio::io_context& io) // ??? explicit, how it works
        : io_(io), socket_(io), timer_(io) {}

    void start() {
        do_connect();
    }


private:
    boost::asio::io_context& io_;          // link on event-loop
    tcp::socket socket_;                   // TCP client
    boost::asio::steady_timer timer_;      // timer for periodic sending
    std::vector<uint8_t> send_buffer_;     // buffer for sending

    // setting TCP-connection with server with auto retries if it'll have fails
    void do_connect() {
        // create endpoint
        std::string ip = "127.0.0.1";
        unsigned short port = 12345;  // unsigned short WTF?? It is standard type of port 
        boost::system::error_code ec;
        auto endpoint = tcp::endpoint(boost::asio::ip::make_address(ip, ec), port);
        if (ec) {
            std::cerr << "Invalid address: " << ec.message() << std::endl;
            return;
        }
        
        //create callback with lambda function
        auto on_connect = [self = shared_from_this()](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "Connected!\n";
            self->schedule_send();
        } else {
            std::cerr << "Error: " << ec.message() << "\n";
            self->timer_.expires_after(std::chrono::seconds(2));
            self->timer_.async_wait([self](boost::system::error_code) {
                self->do_connect();
            });
        }
    };


        socket_.async_connect(endpoint, on_connect);

    }

    void do_write() {

    }

    // setting timer for periodic data sending 
    void schedule_send() {
        // Setting timer for a one second
        timer_.expires_after(std::chrono::seconds(1));

        // async waiting for timer trigger
        timer_.async_wait([self = shared_from_this()](boost::system::error_code ec) {
            // If timer was cancel, it will be ignore
            if (ec == boost::asio::error::operation_aborted) {
                return;
            }
            // if other errors did not happened, we'll call data sending
            if (!ec) {
                self->do_write();
            } else {
                std::cerr << "Timer error in schedule_send: " << ec.message() << std::endl;
            }
        });
    }

};