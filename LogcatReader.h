#ifndef LOGCATREADER_H
#define LOGCATREADER_H

#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <cstdio>
#include <queue>
#include <mutex>

class LogcatReader : public Gtk::Window {
public:
    LogcatReader();
    virtual ~LogcatReader();

private:
    // Widgets
    Gtk::Box vbox;
    Gtk::ComboBoxText logTypes;
    Gtk::Button startButton, exitButton, searchButton;
    Gtk::Entry searchEntry;
    Gtk::TextView logView;
    Gtk::ScrolledWindow scrolledWindow;
    Gtk::Frame logFrame;
    Gtk::Box buttonBox, searchBox;

    // File stream for log
    FILE* logFile;
    std::thread logThread;
    std::atomic<bool> running;
    Glib::Dispatcher dispatcher;
    Glib::RefPtr<Gtk::TextBuffer> logBuffer;
    Glib::RefPtr<Gtk::TextTag> timeTag, errorTag, warningTag, infoTag, typeTag, searchTag;

    // Log queue and mutex for thread-safe access
    std::queue<std::string> logQueue;
    std::mutex logMutex;

    // Signal handlers
    void onStartClicked();
    void onExitClicked();
    void onLogTypeChanged();
    void onSearchClicked();

    // Other methods
    void readLogFile();
    void onDispatcher();

    // Helper function to get log type filter
    std::string getLogType(const std::string& selectedOption);
};

#endif // LOGCATREADER_H
