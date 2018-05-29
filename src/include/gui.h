#ifndef _FT_GUI_H_
#define _FT_GUI_H_

#include <gtkmm.h>
#include <boost/dll/runtime_symbol_info.hpp>
#include <iomanip>

#include "fueltest.h"

class FTGui {
public:
    FTGui();
    ~FTGui();
    Gtk::Window *ft_main();
    void notify_update();
    void notify_timeout();

private:
    void on_button_connect_clicked();
    void on_button_tare_clicked();
    void on_button_calibrate_clicked();
    void on_button_record_clicked();
    void on_notification_from_monitor();
    void on_timeout_received();
    void on_button_finnish_clicked();

    void error(std::string errmsg, Gtk::Window *parent);
    void run_sampling_window();

    FuelTest ft;
    std::thread rtmon;
    Glib::Dispatcher iface_disp;
    Glib::Dispatcher error_disp;

    bool connected;
    bool dumping;

    std::stringstream file_glade;
    Glib::RefPtr<Gtk::Builder> builder_main;

    Gtk::Window *window_main;
    Gtk::Button *button_connect;
    Gtk::Button *button_tare;
    Gtk::Button *button_calibrate;
    Gtk::Button *button_record;
    Gtk::Label *label_status;
    Gtk::Label *label_weight;

    Gtk::Window *window_sampling;
    Gtk::Label *label_sampling;
    Gtk::Button *button_finnish;
};

#endif /* _FT_GUI_H_ */