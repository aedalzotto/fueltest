#include "serial.h"

#ifdef _WIN32
#include <windows.h>
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
        port.set_option(boost::asio::serial_port_base::character_size(8));
		port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
		port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
		port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
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
    HANDLE native_port = port.native_handle();
	if(!PurgeComm(native_port, PURGE_RXCLEAR | PURGE_TXCLEAR)){
        LPSTR messageBuffer = nullptr;
		DWORD errorMessageID = GetLastError();
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);
        throw std::runtime_error(message);
    }
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
    std::vector<std::string> ports;
#ifdef _WIN32
    /* This was made by Gabriel Kressin */
	HKEY Key;
    DWORD Sz = 50;
    char Name[50],Value[50];

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &Key))
        return ports;

    for(int i=0; RegEnumValue(Key, i, Name, &Sz, NULL, NULL, NULL, NULL) == 0 ; i++)
    {
        if(RegQueryValueEx(Key, Name, 0, NULL, (LPBYTE)Value, &Sz) == 0)
            ports.push_back(Value);
        Sz = 50;
    }

    RegCloseKey(Key);
    
#else //Unix
    struct dirent *ent;
    struct stat sb;
    DIR *dir;

    

    if(!::stat("/dev/serial/by-id", &sb) && S_ISDIR(sb.st_mode))
        dir = ::opendir("/dev/serial/by-id");
    else
        return ports;

    while((ent = ::readdir(dir)))
        if(::strlen(ent->d_name)>2)
            ports.push_back("/dev/serial/by-id/"+std::string(ent->d_name));

    ::closedir(dir);
#endif
    return ports;
}