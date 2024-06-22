#include "LogcatReader.h"

LogcatReader::LogcatReader()
    : vbox(Gtk::ORIENTATION_VERTICAL, 10),
      logTypes(),
      startButton("Start Logging"),
      exitButton("Exit"),
      searchButton("Search"),
      running(false),
      logFile(nullptr),
      buttonBox(Gtk::ORIENTATION_HORIZONTAL, 10),
      searchBox(Gtk::ORIENTATION_HORIZONTAL, 5)
{
    // Set up log types
    logTypes.append("Errors");
    logTypes.append("Warnings");
    logTypes.append("Info");
    //logTypes.append("Kernel logs");
    //logTypes.append("Driver log");
    logTypes.set_active(0);

    // Set up log buffer
    logBuffer = Gtk::TextBuffer::create();
    logView.set_buffer(logBuffer);

    // Set up tags for highlighting
    timeTag = logBuffer->create_tag("time");
    timeTag->property_foreground() = "sky blue";
    errorTag = logBuffer->create_tag("error");
    errorTag->property_foreground() = "red";
    warningTag = logBuffer->create_tag("warning");
    warningTag->property_foreground() = "yellow";
    infoTag = logBuffer->create_tag("info");
    infoTag->property_foreground() = "dark green";
    typeTag = logBuffer->create_tag("type");
    typeTag->property_foreground() = "purple";
    searchTag = logBuffer->create_tag("search");
    searchTag->property_background() = "yellow";

    // Add widgets to vbox
    vbox.pack_start(logTypes, false, false, 0);
    vbox.pack_start(buttonBox, false, false, 0);
    buttonBox.pack_start(startButton, false, false, 0);
    buttonBox.pack_start(exitButton, false, false, 0);
    vbox.pack_start(searchBox, false, false, 0);
    searchBox.pack_start(searchEntry, true, true, 0);
    searchBox.pack_start(searchButton, false, false, 0);
    vbox.pack_start(logFrame, true, true, 0);
    logFrame.add(scrolledWindow);
    scrolledWindow.add(logView);

    // Set up signals
    startButton.signal_clicked().connect(sigc::mem_fun(*this, &LogcatReader::onStartClicked));
    exitButton.signal_clicked().connect(sigc::mem_fun(*this, &LogcatReader::onExitClicked));
    searchButton.signal_clicked().connect(sigc::mem_fun(*this, &LogcatReader::onSearchClicked));
    logTypes.signal_changed().connect(sigc::mem_fun(*this, &LogcatReader::onLogTypeChanged));
    dispatcher.connect(sigc::mem_fun(*this, &LogcatReader::onDispatcher));

    // Improve GUI appearance
    set_border_width(10);
    vbox.set_spacing(10);
    logTypes.set_size_request(200, -1);
    startButton.set_size_request(100, -1);
    exitButton.set_size_request(100, -1);
    searchButton.set_size_request(100, -1);
    scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    logFrame.set_label("Log Output");
    logFrame.set_label_align(0.5, 0.5);
    logFrame.set_shadow_type(Gtk::SHADOW_ETCHED_IN);

    add(vbox);
    show_all_children();
}

LogcatReader::~LogcatReader() {
    if (running) {
        running = false;
        logThread.join();
        if (logFile) {
            pclose(logFile);
        }
    }
}

void LogcatReader::onStartClicked() {
    std::string selectedOption = logTypes.get_active_text();
    std::string filter = getLogType(selectedOption);
    if (!filter.empty()) {
        std::string command = "adb logcat -v time ";
        if (selectedOption == "Kernel logs") {
            command += " -b kernel";
        } else if (selectedOption == "Driver log") {
            command += " -b all";
        }
        command += " *:S " + filter;
        logFile = popen(command.c_str(), "r");
        if (!logFile) {
            std::cerr << "Failed to open log file." << std::endl;
            return;
        }
        running = true;
        logThread = std::thread(&LogcatReader::readLogFile, this);
    }
}

void LogcatReader::onExitClicked() {
    if (running) {
        running = false;
        if (logFile) {
            pclose(logFile);
            logFile = nullptr;
        }
        std::string command = "pkill -SIGINT adb";
        system(command.c_str());
        logThread.join();
    }
    hide(); // Close the window
}

void LogcatReader::onLogTypeChanged() {
    // No action needed here
}

void LogcatReader::onSearchClicked() {
    std::string searchText = searchEntry.get_text();
    if (searchText.empty()) {
        return;
    }

    logBuffer->remove_tag(searchTag, logBuffer->begin(), logBuffer->end());

    Gtk::TextBuffer::iterator start, end;
    start = logBuffer->begin();
    while (true) {
        start = logBuffer->get_iter_at_offset(start.get_offset());
        end = logBuffer->get_iter_at_offset(start.get_offset());
        if (!start.forward_search(searchText, Gtk::TEXT_SEARCH_CASE_INSENSITIVE, start, end)) {
            break;
        }
        logBuffer->apply_tag(searchTag, start, end);
        start = end;
    }
}

void LogcatReader::readLogFile() {
    char buffer[1024];
    while (running) {
        while (fgets(buffer, sizeof(buffer), logFile)) {
            std::string line(buffer);
            {
                std::lock_guard<std::mutex> lock(logMutex);
                logQueue.push(line);
            }
            dispatcher.emit();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void LogcatReader::onDispatcher() {
    std::queue<std::string> localQueue;
    {
        std::lock_guard<std::mutex> lock(logMutex);
        localQueue.swap(logQueue);
    }
    std::string selectedOption = logTypes.get_active_text();
    std::string logType = getLogType(selectedOption);

    while (!localQueue.empty()) {
        std::string line = localQueue.front();
        localQueue.pop();

        // Filter logs based on the selected log type
        if ((logType == "*:E" && line.find(" E/") != std::string::npos) ||
            (logType == "*:W" && line.find(" W/") != std::string::npos) ||
            (logType == "*:I" && line.find(" I/") != std::string::npos) ||
            (logType == "kernel:V" && line.find(" kernel/") != std::string::npos) ||
            (logType == "driver:V" && line.find(" driver/") != std::string::npos)) {

            auto iter = logBuffer->end();
            logBuffer->insert(iter, line);
            iter = logBuffer->end();
            iter.backward_chars(line.length());

            // Apply tags based on content
            if (line.find(" E/") != std::string::npos) {
                logBuffer->apply_tag(errorTag, iter, logBuffer->end());
            } else if (line.find(" W/") != std::string::npos) {
                logBuffer->apply_tag(warningTag, iter, logBuffer->end());
            } else if (line.find(" I/") != std::string::npos) {
                logBuffer->apply_tag(infoTag, iter, logBuffer->end());
            }

            // Highlight time and type
            auto timePos = line.find(' ');
            if (timePos != std::string::npos) {
                auto timeEnd = line.find(' ', timePos + 1);
                if (timeEnd != std::string::npos) {
                    auto timeEndIter = iter;
                    timeEndIter.forward_chars(timeEnd);
                    logBuffer->apply_tag(timeTag, iter, timeEndIter);

                    auto typePos = line.find(' ', timeEnd + 1);
                    if (typePos != std::string::npos) {
                        auto typeStartIter = iter;
                        typeStartIter.forward_chars(typePos);
                        auto typeEndIter = iter;
                        typeEndIter.forward_chars(line.find(' ', typePos + 1));
                        logBuffer->apply_tag(typeTag, typeStartIter, typeEndIter);
                    }
                }
            }
        }
    }
    auto end_iter = logBuffer->end();
    logView.scroll_to(end_iter);
}

std::string LogcatReader::getLogType(const std::string& selectedOption) {
    if (selectedOption == "Errors") {
        return "*:E";
    } else if (selectedOption == "Warnings") {
        return "*:W";
    } else if (selectedOption == "Info") {
        return "*:I";
    } else if (selectedOption == "Kernel logs") {
        return "kernel:V";
    } else if (selectedOption == "Driver log") {
        return "driver:V";
    }
    return "";
}
