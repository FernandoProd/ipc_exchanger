#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <memory>
#include <vector>
#include "protocol.hpp"
#include <iomanip>

// function for formated time to ISO-8601
// all code about formated time was stolen from LLM
static std::string format_iso8601(int64_t ms) {
    using namespace std::chrono;
    auto tp = system_clock::time_point(milliseconds(ms));
    std::time_t tt = system_clock::to_time_t(tp);
    auto millis = ms % 1000;
    std::tm tm = *std::gmtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << millis << 'Z';
    return oss.str();
}

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
        // get the current time
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        // forming package with packMessage
        // return std::vector<uint8_t> 
        send_buffer_ = ipc::protocol::packMessage(ms);

        // async writing with catching shared_ptr
        auto self = shared_from_this();  // важно: продлеваем жизнь объекта
        boost::asio::async_write(socket_, boost::asio::buffer(send_buffer_),
            [self, ms](boost::system::error_code ec, size_t /*bytes*/) {
                if (!ec) {
                    // gettting current iso time for out 
                    std::cout << "Sent timestamp: " << format_iso8601(ms) << std::endl;
                    // auto now_iso = std::chrono::system_clock::now();
                    // std::time_t tt = std::chrono::system_clock::to_time_t(now_iso);
                    // std::string iso_str = std::ctime(&tt);
                    // iso_str.pop_back(); // убираем '\n'
                    // std::cout << "Sent timestamp: " << iso_str << std::endl;

                    // planning next send
                    self->schedule_send();
                } else {
                    std::cerr << "Write error: " << ec.message() << std::endl;
                    // Closing socket and ignore error
                    boost::system::error_code ignored;
                    // canceling our planning send
                    self->timer_.cancel(ignored);
                    self->socket_.close(ignored);

                    // Waiting for 2 second and reconnecting 
                    self->timer_.expires_after(std::chrono::seconds(2));
                    self->timer_.async_wait([self](boost::system::error_code ec) {
                        if (!ec) {
                            self->do_connect();
                        }
                    });
                }
            });
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
                self->do_connect();
            }
        });
    }

};


int main() {
    boost::asio::io_context io;
    auto sender = std::make_shared<Sender>(io);
    sender->start();
    io.run();
    return 0;
}