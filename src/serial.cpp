#include "serial.h"

#ifdef _WIN32

#else
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#endif

Serial::Serial() : io(),
                    port(io)
{

}

void Serial::begin(std::string portname, size_t baud)
{
    try {
        port.open(portname);
        port.set_option(boost::asio::serial_port_base::baud_rate(baud));
    } catch(const boost::system::system_error &ex){
        throw ex;
    }
}

void Serial::write_buffer_blocking(const uint8_t *buffer, size_t size)
{
    if(!port.is_open())
        throw std::runtime_error("Port is closed");
    try{
        boost::asio::write(port, boost::asio::buffer(buffer, size));
    } catch(const boost::system::system_error &ex){
        throw ex;
    }
}

/*
 * https://stackoverflow.com/questions/13126776/asioread-with-timeout?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
 */
void Serial::read_buffer_timeout(uint8_t *buffer, size_t size, size_t timeout_ms)
{
    if(!port.is_open())
        throw std::runtime_error("Port is closed");
    
    boost::optional<boost::system::error_code> timer_result;
    boost::asio::deadline_timer timer(port.get_io_service());
    timer.expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    timer.async_wait([&timer_result] (const boost::system::error_code& error)
                        {
                            timer_result.reset(error);
                        });

    boost::optional<boost::system::error_code> read_result;
    boost::asio::async_read(port, boost::asio::buffer(buffer, size),
                            [&read_result] (const boost::system::error_code& error, size_t)
                            {
                                read_result.reset(error);
                            });

    port.get_io_service().reset();
    while (port.get_io_service().run_one())
    { 
        if(read_result)
            timer.cancel();
        else if(timer_result)
            port.cancel();
    }

    if (*read_result)
        throw boost::system::system_error(*read_result);
}

void Serial::flush()
{
    if(!port.is_open())
        throw std::runtime_error("Port is closed");

#ifdef _WIN32

#else
    int native_port = port.native_handle();
    if(::tcflush(native_port, TCIOFLUSH))
        throw std::runtime_error(std::string(strerror(errno)));
#endif
}

void Serial::close()
{
    if(port.is_open())
        port.close();
}

bool Serial::is_open()
{
    return port.is_open();
}


std::vector<std::string> Serial::list_ports()
{
#ifdef _WIN32

#else //Unix
    struct dirent *ent;
    struct stat sb;
    DIR *dir;

    std::vector<std::string> ports;

    if(!::stat("/dev/serial/by-id", &sb) && S_ISDIR(sb.st_mode))
        dir = ::opendir("/dev/serial/by-id");
    else
        return ports;

    while((ent = ::readdir(dir)))
        if(::strlen(ent->d_name)>2)
            ports.push_back("/dev/serial/by-id/"+std::string(ent->d_name));

    ::closedir(dir);
    return ports;
#endif
}