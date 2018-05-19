#include "gui.h"

FTGui::FTGui() :
    rtmon(),
    iface_disp()
{
    //Get path to glade file
    boost::filesystem::path path_glade;
    builder_main = Gtk::Builder::create();
    path_glade = boost::dll::program_location().parent_path().parent_path().append("share/fueltest");
    file_glade.str("");
    file_glade << path_glade.string() << "/main.glade";

    connected = false;

    //GTK Widgets starts as nullptr
    window_main = nullptr;
    button_connect = nullptr;
    button_tare = nullptr;
    button_calibrate = nullptr;
    button_record = nullptr;
}

FTGui::~FTGui()
{
    if(ft.is_connected() && rtmon.joinable()){
        ft.stop_transmission(); //Sometimes is taking forever!
        rtmon.join();
    }
}

Gtk::Window* FTGui::ft_main()
{
    //Open glade file
    try {
        builder_main->add_from_file(file_glade.str());
    } catch(...){
        throw;
    }

    //Get pointer to widgets of the main window
    builder_main->get_widget("window_main", window_main);
    if(!window_main){
        throw std::runtime_error("Unable to access window_main widget");
    }
    
    builder_main->get_widget("button_connect", button_connect);
    if(!button_connect){
        throw std::runtime_error("Unable to access button_connect widget");
    }

    builder_main->get_widget("button_tare", button_tare);
    if(!button_connect){
        throw std::runtime_error("Unable to access button_tare widget");
    }

    builder_main->get_widget("button_calibrate", button_calibrate);
    if(!button_connect){
        throw std::runtime_error("Unable to access button_calibrate widget");
    }

    builder_main->get_widget("button_record", button_record);
    if(!button_connect){
        throw std::runtime_error("Unable to access button_record widget");
    }

    builder_main->get_widget("label_status", label_status);
    if(!label_status){
        throw std::runtime_error("Unable to access label_status widget");
    }
    builder_main->get_widget("label_weight", label_weight);
    if(!label_status){
        throw std::runtime_error("Unable to access label_weight widget");
    }

    //Signals of the main window
    button_connect->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_connect_clicked));
    button_tare->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_tare_clicked));
    button_calibrate->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_calibrate_clicked));
    button_record->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_record_clicked));
    iface_disp.connect(sigc::mem_fun(*this, &FTGui::on_notification_from_monitor));
    error_disp.connect(sigc::mem_fun(*this, &FTGui::on_timeout_received));

    //Disconnected show as red
    label_status->override_color(Gdk::RGBA("#cc0000"));

    //Return main window to be shown by main
    return window_main;
}

void FTGui::on_button_connect_clicked()
{
    try {
        ft.auto_connect(115200);
        ft.init_transmission();
        button_connect->set_sensitive(false);
        button_tare->set_sensitive(true);
        button_calibrate->set_sensitive(true);
        button_record->set_sensitive(true);
        label_status->override_color(Gdk::RGBA("#00cc00"));
        label_status->set_text("Conectado");
        rtmon = std::thread(&FuelTest::monitor, &ft, this, false);
        connected=true;
    } catch(const std::exception& ex){
        error(ex.what(), window_main);
    }
}

void FTGui::on_button_tare_clicked()
{
    try {
        ft.stop_transmission();
        rtmon.join();
        ft.tare();
        ft.init_transmission();
        rtmon = std::thread(&FuelTest::monitor, &ft, this, false);
    } catch(std::exception &ex){
        error(ex.what(), window_main);
    }
}

void FTGui::on_button_calibrate_clicked()
{
    //@TODO: Show not implemented message
}

void FTGui::on_button_record_clicked()
{
    //@TODO
}

void FTGui::notify_update()
{
    iface_disp.emit();
}

void FTGui::on_notification_from_monitor()
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << ft.get_last_weight();
    label_weight->set_text(ss.str());
}

void FTGui::error(std::string errmsg, Gtk::Window *parent)
{
    connected = false;
    ft.disconnect();
    if(rtmon.joinable())
        rtmon.join();
    button_connect->set_sensitive(true);
    button_tare->set_sensitive(false);
    button_calibrate->set_sensitive(false);
    button_record->set_sensitive(false);
    label_status->override_color(Gdk::RGBA("#cc0000"));
    label_status->set_text("Desconectado");
    Gtk::MessageDialog dialog(*parent, "ERRO", false, Gtk::MESSAGE_ERROR);
    dialog.set_secondary_text(errmsg);
    dialog.run();
}

void FTGui::notify_timeout()
{
    error_disp.emit();
}

void FTGui::on_timeout_received()
{
    if(connected)
        error("Timeout", window_main);
}