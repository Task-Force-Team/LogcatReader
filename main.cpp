#include "LogcatReader.h"
#include <gtkmm.h>

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.taskforce.logcatreader");
    LogcatReader window;
    return app->run(window);
}
