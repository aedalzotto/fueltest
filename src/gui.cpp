#include "gui.h"

FTGui::FTGui()
{
    //Get path to glade file
    boost::filesystem::path path_glade;
    builder_main = Gtk::Builder::create();
    path_glade = boost::dll::program_location().parent_path().parent_path().append("share/fueltest");
    file_glade.str("");
    file_glade << path_glade.string() << "/main.glade";


    //GTK Widgets starts as nullptr
    window_main = nullptr;
    button_connect = nullptr;
    button_tare = nullptr;
    button_calibrate = nullptr;
    button_record = nullptr;
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

    //Signals of the main window
    button_connect->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_connect_clicked));
    button_tare->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_tare_clicked));
    button_calibrate->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_calibrate_clicked));
    button_record->signal_clicked().connect(
                sigc::mem_fun(*this, &FTGui::on_button_record_clicked));

    label_status->override_color(Gdk::RGBA("#cc0000"));

    //Return main window to be shown by main
    return window_main;
}

void FTGui::on_button_connect_clicked()
{
    try {
        ft.auto_connect();
        button_connect->set_sensitive(false);
        button_tare->set_sensitive(true);
        button_calibrate->set_sensitive(true);
        button_record->set_sensitive(true);
        label_status->override_color(Gdk::RGBA("#00cc00"));
        label_status->set_text("Conectado");
        //Start the connection thread to update label_weight
    } catch(const std::exception& ex){
        Gtk::MessageDialog dialog(*window_main, "ERRO", false, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text(ex.what());
        dialog.run();
    }
}

void FTGui::on_button_tare_clicked()
{

}

void FTGui::on_button_calibrate_clicked()
{
    //Show not implemented message
}

void FTGui::on_button_record_clicked()
{

}