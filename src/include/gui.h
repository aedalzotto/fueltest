#include <gtkmm.h>
#include <boost/dll/runtime_symbol_info.hpp>

#include "fueltest.h"

class FTGui {
public:
    FTGui();
    Gtk::Window *ft_main();

private:

    FuelTest ft;

    Glib::RefPtr<Gtk::Builder> builder_main;
    boost::filesystem::path path_glade;

    Gtk::Window *window_main;
    
};