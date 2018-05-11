#include <gtkmm.h>
#include <boost/dll/runtime_symbol_info.hpp>

#include "fueltest.h"

class FTGui {
public:
    FTGui();
    Gtk::Window *ft_main();

private:
    void on_button_connect_clicked();
    void on_button_tare_clicked();
    void on_button_calibrate_clicked();
    void on_button_record_clicked();

    FuelTest ft;

    std::stringstream file_glade;
    Glib::RefPtr<Gtk::Builder> builder_main;

    Gtk::Window *window_main;
    Gtk::Button *button_connect;
    Gtk::Button *button_tare;
    Gtk::Button *button_calibrate;
    Gtk::Button *button_record;
    Gtk::Label *label_status;

};