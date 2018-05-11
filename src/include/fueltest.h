#include <string>
#include <stdexcept>

class FuelTest {
public:
    FuelTest();
    void auto_connect();
    void tare();
    void stop_transmission();
    void transmit(unsigned int frequency);
    void dump_transmission(std::string filename);

    bool is_connected();
    bool is_transmission_active();
    float get_last_weight();
    unsigned long get_last_rtime();

private:
    void add_checksum(uint8_t &buffer, uint8_t sz, uint8_t &ck_a, uint8_t &ck_b);
    bool verify_checksum(uint8_t &buffer, uint8_t size);

    bool connected;
    bool transmitting;
    float last_weight;
    unsigned long first_point_time;
    unsigned long last_point_time;
    uint8_t out_buffer[8];
    uint8_t in_buffer[12];
};