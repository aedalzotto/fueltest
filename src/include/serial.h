#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <vector>
#include <string>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

class Serial {
public:
    Serial();
    void begin(std::string portname, size_t baud);
    void write_buffer_blocking(const uint8_t *buffer, size_t size);

    template<typename T> 
    void write_blocking(const T data)
    {
        write_buffer_blocking(&data, sizeof(data));
    }

    void read_buffer_timeout(uint8_t *buffer, size_t size, size_t timeout_ms);

    template<typename T> 
    T read_timeout(size_t timeout_ms=100)
    {
        T data;
        try {
            read_buffer_timeout((uint8_t*)&data, sizeof(data), timeout_ms);
        } catch(const boost::system::system_error &ex){
            throw ex;
        }
        return data;
    }

    void flush();
    void close();

    bool is_open();

    static std::vector<std::string> list_ports();

private:
    boost::asio::io_service io;
    boost::asio::serial_port port;
};

#endif /* _SERIAL_H_ */