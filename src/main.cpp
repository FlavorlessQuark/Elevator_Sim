#include "System.hpp"
#include <iostream>
#include <functional>
#include <chrono>
#include <sstream>
#include <unistd.h>
#include <sys/poll.h>
#include <termios.h>
#include <fcntl.h>


int count = 3;
int floors = 10;


struct RawTerminal {
    struct termios orig_termios;

    RawTerminal() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        
        raw.c_lflag &= ~(ICANON | ECHO);
        
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    ~RawTerminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        std::cout << "\033[?25h" << std::flush;
    }
};

void getInput(std::string& mutable_input) {
    char buf[256];
    int bytesRead = read(STDIN_FILENO, buf, sizeof(buf));
    
    if (bytesRead > 0) {
        mutable_input.append(buf, bytesRead);
    }
}

void render(std::vector<Elevator>& elevators) {
    std::cout << "\033[2J\033[H";
    for (size_t i = 0; i < elevators.size(); ++i) {
        const Elevator& elevator = elevators[i];
        std::cout << "Elevator " << i + 1 << ": Floor " << elevators[i].getState().currentFloor
                    << " | State: " << static_cast<int>(elevators[i].getState().state) << std::endl;
        }

}

bool extractNewCommand(const std::string& buffer, std::string& outCommand) {
    static size_t lastProcessedIndex = std::string::npos;
    
    size_t lastNewline = buffer.find_last_of("\r\n");
    
    if (lastNewline == std::string::npos || lastNewline == lastProcessedIndex) {
        return false;
    }
    
    size_t startPos = 0;
    if (lastNewline > 0) {
        size_t prevNewline = buffer.find_last_of("\r\n", lastNewline - 1);
        if (prevNewline != std::string::npos) {
            startPos = prevNewline + 1;
        }
    }
    
    outCommand = buffer.substr(startPos, lastNewline - startPos);
    
    lastProcessedIndex = lastNewline;
    
    return true; 
}

int main(int argc, char* argv[]){
    RawTerminal terminal;
    System system(floors, count);
    std::string input;
    std::cout << "\033[2J\033[H\033[?25l";
    while (true) {
        getInput(input);
        std::string command;
        if (extractNewCommand(input, command)) {
            system.processInput(command); 
        }
        system.update(input);
        system.display(input);
        usleep(20000);
    }
    return 0;
}