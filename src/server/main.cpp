#include <gmredis/version.h>

#include <asio.hpp>
#include <print>
#include <memory>

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(
            asio::buffer(data_, max_length),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::println("Received {} bytes from client", length);
                    do_write(length);
                } else {
                    std::println("Read error: {}", ec.message());
                }
            });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        asio::async_write(
            socket_,
            asio::buffer(data_, length),
            [this, self](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                } else {
                    std::println("Write error: {}", ec.message());
                }
            });
    }

    tcp::socket socket_;
    static constexpr std::size_t max_length = 1024;
    char data_[max_length];
};

class Server {
public:
    Server(asio::io_context& io_context, unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::println("New client connected from {}:{}",
                        socket.remote_endpoint().address().to_string(),
                        socket.remote_endpoint().port());
                    std::make_shared<Session>(std::move(socket))->start();
                } else {
                    std::println("Accept error: {}", ec.message());
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

int main() {
    std::println("GMRedis Server");
    std::println("{}", gmredis::get_version_info());
    std::println("Starting server on port 6379...");

    try {
        asio::io_context io_context;
        Server server(io_context, 6379);

        std::println("Server listening on 0.0.0.0:6379");
        std::println("Press Ctrl+C to stop");

        io_context.run();
    } catch (const std::exception& e) {
        std::println(stderr, "Error: {}", e.what());
        return 1;
    }

    return 0;
}
