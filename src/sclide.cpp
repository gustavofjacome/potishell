#include "sclide.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/ioctl.h>

namespace Terminal {

    // rebuild the fast track color lines based on the current sclideColor and shadeDegradingColor
    void sclide::rebuildFastTrackColor() {
        auto clampChannel = [](int value) {
            if (value < 0) {
                return 0;
            }
            if (value > 255) {
                return 255;
            }
            return value;
        };

        auto mixChannel = [&](int colorChannel, int backgroundChannel, int step) {
            return clampChannel(backgroundChannel + ((colorChannel - backgroundChannel) * step) / 4);
        };

        fastTrackColorLine = "\033[38;2;" + std::to_string(clampChannel(sclideColor.r)) + ";" +
                             std::to_string(clampChannel(sclideColor.g)) + ";" +
                             std::to_string(clampChannel(sclideColor.b)) + "m";
        fastTrackShadeFirst = fastTrackColorLine;
        fastTrackShadeSecond = "\033[38;2;" + std::to_string(mixChannel(sclideColor.r, shadeDegradingColor.r, 3)) + ";" +
                               std::to_string(mixChannel(sclideColor.g, shadeDegradingColor.g, 3)) + ";" +
                               std::to_string(mixChannel(sclideColor.b, shadeDegradingColor.b, 3)) + "m";
        fastTrackShadeThird = "\033[38;2;" + std::to_string(mixChannel(sclideColor.r, shadeDegradingColor.r, 2)) + ";" +
                              std::to_string(mixChannel(sclideColor.g, shadeDegradingColor.g, 2)) + ";" +
                              std::to_string(mixChannel(sclideColor.b, shadeDegradingColor.b, 2)) + "m";
        fastTrackShadeFourth = "\033[38;2;" + std::to_string(mixChannel(sclideColor.r, shadeDegradingColor.r, 1)) + ";" +
                               std::to_string(mixChannel(sclideColor.g, shadeDegradingColor.g, 1)) + ";" +
                               std::to_string(mixChannel(sclideColor.b, shadeDegradingColor.b, 1)) + "m";
    }

    // important to fast track the line printing so i make pre made lines for each shade and full line and space
    void sclide::makePreMadeLines() {
        for(int i = 0; i < widthOfTerminal; i++) {
            fullLinePreMade += line;
            firstShadePreMade += shades[0];
            secondShadePreMade += shades[1];
            thirdShadePreMade += shades[2];
            fourthShadePreMade += shades[3];
            spacePreMade += ' ';
        }
    }

    // to reset all lines for new data
    void sclide::resetPreMadeLines() {
        fullLinePreMade.clear();
        firstShadePreMade.clear();
        secondShadePreMade.clear();
        thirdShadePreMade.clear();
        fourthShadePreMade.clear();
        spacePreMade.clear();
    }

    sclide::sclide(int startingRow) : startingRow(startingRow) {
        struct winsize w = {};
        // fallback pra quando stdout nao é um terminal (ex: pipe), ai o ioctl falha ou retorna 0
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0 || w.ws_col == 0 || w.ws_row == 0) {
            w.ws_col = 80;
            w.ws_row = 24;
        }
        widthOfTerminal = w.ws_col;
        heightOfTerminal = w.ws_row;
        makePreMadeLines();
        rebuildFastTrackColor();
    }

    int sclide::setSclideColor(int r, int g, int b) {
        sclideColor.r = r;
        sclideColor.g = g;
        sclideColor.b = b;
        rebuildFastTrackColor();
        return 0;
    }

    int sclide::setSclideColor(std::string hexCode) {
        // Convert hex code to rgb
        rgb color;
        color.r = std::stoi(hexCode.substr(1, 2), nullptr, 16);
        color.g = std::stoi(hexCode.substr(3, 2), nullptr, 16);
        color.b = std::stoi(hexCode.substr(5, 2), nullptr, 16);
        sclideColor = color;
        rebuildFastTrackColor();
        return 0;
    }

    int sclide::setShadeDegradingColor(int r, int g, int b) {
        shadeDegradingColor.r = r;
        shadeDegradingColor.g = g;
        shadeDegradingColor.b = b;
        rebuildFastTrackColor();
        return 0;
    }

    int sclide::setShadeDegradingColor(std::string hexCode) {
        // Convert hex code to rgb
        rgb color;
        color.r = std::stoi(hexCode.substr(1, 2), nullptr, 16);
        color.g = std::stoi(hexCode.substr(3, 2), nullptr, 16);
        color.b = std::stoi(hexCode.substr(5, 2), nullptr, 16);
        shadeDegradingColor = color;
        rebuildFastTrackColor();
        return 0;
    }

    int sclide::resetSclideColor() {
        sclideColor = {255, 255, 255};
        rebuildFastTrackColor();
        return 0;
    }

    int sclide::setSpeed(double newSpeed) {
        speed = newSpeed;
        return 0;
    }

    int sclide::changeShades(std::vector<std::string> newShades) {
        for(int i = 0; i < 4; i++) {
            shades[i] = newShades[i];
        }
        resetPreMadeLines();
        makePreMadeLines();
        return 0;
    }

    int sclide::clean(){
        // using a buffer string
        std::string buffer;
        std::string hideCursor = "\033[?25l"; // Hide cursor

        buffer = hideCursor + "\033[" + std::to_string(startingRow) + ";1H" + fastTrackColorLine + fullLinePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // printing the line with first shade now
        // printing hard coded color for the first shade

        buffer = "\033[" + std::to_string(startingRow) + ";1H" + fastTrackShadeFirst+firstShadePreMade + fastTrackColorLine+fullLinePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // printing the line with second shade now
        buffer = "\033[" + std::to_string(startingRow) + ";1H" + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade + fastTrackColorLine+fullLinePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // printing the line with third shade now
        buffer = "\033[" + std::to_string(startingRow) + ";1H" + fastTrackShadeThird+thirdShadePreMade + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade + fastTrackColorLine+fullLinePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // printing the line with fourth shade now
        buffer = "\033[" + std::to_string(startingRow) + ";1H" + fastTrackShadeFourth+fourthShadePreMade + fastTrackShadeThird+thirdShadePreMade + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade + fastTrackColorLine+fullLinePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // making final buffer without coordainte so i can use direct coords
        buffer = spacePreMade + fastTrackShadeFourth+fourthShadePreMade + fastTrackShadeThird+thirdShadePreMade + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade + fastTrackColorLine+fullLinePreMade;

        // now using loop to simulate remaining one with space too so it clean the text behind
        for (int i = 0; i < heightOfTerminal - startingRow - 4; ++i) {
            int row = startingRow + i;
            std::string command = "\033[" + std::to_string(row) + ";1H" + buffer;
            write(STDOUT_FILENO, command.c_str(), command.size());
            std::this_thread::sleep_for(std::chrono::duration<double>(speed));
        }

        // Manual final animation: the trail keeps sliding down until it falls off the
        // bottom of the terminal. Each frame the trail is one row shorter because the
        // bottom row (the line first, then shade4, shade3...) already left the screen.
        int stoppedRow = heightOfTerminal - 4;

        // the line already fell off, only the shades remain
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade + fastTrackShadeFourth+fourthShadePreMade + fastTrackShadeThird+thirdShadePreMade + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // shade4 fell off too
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade + fastTrackShadeThird+thirdShadePreMade + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // shade3 fell off
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // shade2 fell off
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade + fastTrackShadeFirst+firstShadePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // shade1 fell off, only space remains and the screen is clean
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        std::cout << "\033[" << startingRow << ";1H"; // Move cursor to the starting row
        std::cout << "\033[0m"; // Reset color
        std::cout << "\033[?25h"; // Show cursor
        return 0;
    }

    namespace {
        // verde matrix (#00FF41) e 2x mais rapido que o padrao (0.1s -> 0.05s)
        sclide criarSclide() {
            // startingRow 1 porque o escape ANSI é 1-based (0 não existe)
            sclide s(1);
            s.setSclideColor(0, 255, 65);
            s.setSpeed(0.05);
            return s;
        }

        sclide mySclide = criarSclide();
    }

    void limpar() {
        mySclide.clean();
    }
}
