#include "fueltest.h"
#include "gui.h"

#include <iostream>

FuelTest::FuelTest() : serial()
{

}

void FuelTest::auto_connect(uint32_t baud)
{
    std::vector<std::string> ports = Serial::list_ports();
    for(auto &it : ports){
        try {
            serial.begin(it, 115200);
        } catch(...){
            continue;
        }
        static const uint8_t buf[3] = {comm::HDR, comm::CON, comm::ACK};
        serial.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        serial.write_buffer_blocking(buf, 2);
        for(int i = 0; i < 3; i++){
            try {
                if(serial.read_timeout<char>(2000) != buf[i]){ 
                    serial.close();
                    break;
                }
            } catch(const std::exception &ex){
                serial.close();
                break;
            }
        }
        if(serial.is_open())
            return;
    }

    throw std::runtime_error("Impossível conectar.");
}

void FuelTest::disconnect()
{
    serial.close();
}

void FuelTest::monitor(FTGui *caller, bool dump)
{
    std::ofstream dumpfile;
    if(dump){
        dumpfile.open(filename);
        dumpfile << std::fixed;
    }
    
    point_count = 0;
    while(true){
        try {
            if(serial.read_timeout<char>() != comm::HDR)
                continue;
            
            char input = serial.read_timeout<char>();
            if(input == comm::STP){
                if(serial.read_timeout<char>() == comm::ACK){
                    if(dump)
                        dumpfile.close();
                    return;
                }
                else
                    continue;
                
            } else if(input != comm::STA)
                continue;
            
            val_mtx.lock();
            point_count++;
            serial.read_buffer_timeout((uint8_t*)&last_point_time, 8, 500);
            serial.read_buffer_timeout((uint8_t*)&last_weight, 4, 500);
            if(dump)
                dumpfile << last_point_time << "," << last_weight << std::endl;
            val_mtx.unlock();
            caller->notify_update();
        } catch(...){
            val_mtx.unlock();
            if(dump)
                dumpfile.close();
            caller->notify_timeout();
            return;
        }
    }
}

float FuelTest::get_last_weight()
{
    val_mtx.lock();
    float ret = last_weight;
    val_mtx.unlock();
    return ret;
}

void FuelTest::init_transmission()
{
    serial.flush();
    static const uint8_t buf[3] = {comm::HDR, comm::INI, comm::ACK};
    serial.write_buffer_blocking(buf, 2);
    for(int i = 0; i < 3; i++){
        try {
            if(serial.read_timeout<char>() != buf[i])
                throw std::runtime_error("Communication error");
        } catch(...){
            throw;
        }
    }
}

void FuelTest::stop_transmission()
{
    static const uint8_t buf[3] = {comm::HDR, comm::STP, comm::ACK};
    serial.write_buffer_blocking(buf, 2);
}

bool FuelTest::is_connected()
{
    return serial.is_open();
}

void FuelTest::tare()
{
    serial.flush();
    static const uint8_t buf[3] = {comm::HDR, comm::TAR, comm::ACK};
    serial.write_buffer_blocking(buf, 2);
    for(int i = 0; i < 3; i++){
        try {
            if(serial.read_timeout<uint8_t>(1000) != buf[i])
                throw std::runtime_error("Communication error");
        } catch(...){
            throw;
        }
    }
}

void FuelTest::set_dump_filename(std::string name)
{
    filename = name;
}

unsigned long FuelTest::get_point_count()
{
    unsigned long ret;
    val_mtx.lock();
    ret = point_count;
    val_mtx.unlock();
    return ret;
}

void FuelTest::calibrate(float weight)
{
    serial.flush();
    std::cout << weight << std::endl;
    static const uint8_t buf[3] = {comm::HDR, comm::CAL, comm::ACK};
    serial.write_buffer_blocking(buf, 2);
    serial.write_buffer_blocking((uint8_t*)&weight, sizeof(weight));
    for(int i = 0; i < 3; i++){
        try {
            if(serial.read_timeout<uint8_t>(1000) != buf[i])
                throw std::runtime_error("Communication error");
        } catch(...){
            throw;
        }
    }
    std::cout << (float)serial.read_timeout<float>(2000) << std::endl;
}