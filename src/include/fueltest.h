#ifndef _FT_MAIN_H
#define _FT_MAIN_H

#include <string>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>


#include <iostream>

#include "serial.h"

class FTGui;

class FuelTest {
public:
    FuelTest();
    void auto_connect(uint32_t baud);
    void disconnect();
    void tare();
    void calibrate(float weight);
    void stop_transmission();
    void init_transmission();
    void set_dump_filename(std::string name);

    void monitor(FTGui *caller, bool dump);

    bool is_connected();
    float get_last_weight();
    unsigned long get_last_rtime();
    unsigned long get_point_count();

private:
    
    Serial serial;
    std::mutex val_mtx;

    std::string filename;

    float last_weight;
    uint64_t last_point_time;;
    uint64_t point_count;

    enum comm {
        HDR = 0x41,

        CON = 0x63,
        TAR = 0x74,
        CAL = 0x99,
        INI = 0x69,
        STP = 0x73,

        STA = 0x70,
        ACK = 0x61,
        NCK = 0x6E
    };
};


#endif /* _FT_MAIN_H */