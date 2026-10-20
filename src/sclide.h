#pragma once

#include <string>
#include <vector>

namespace Terminal {
// eu nao fiz isso so to reaproveitando para usar na shell
class sclide {

private:
    // base sclide line and shades for degrading effect
    std::string line="─";
    std::string shades[4] = {"█", "▓", "▒", "░"};

    std::string fullLinePreMade;
    std::string firstShadePreMade;
    std::string secondShadePreMade;
    std::string thirdShadePreMade;
    std::string fourthShadePreMade;
    std::string spacePreMade;

private:
    // important to have a struct for rgb color representation
    struct rgb {
        int r;
        int g;
        int b;
    };

private:
    // sclide starting row, width and height of terminal, sclide color and shade degrading color
    int startingRow;
    int widthOfTerminal;
    int heightOfTerminal;
    rgb shadeDegradingColor = {0, 0, 0};
    rgb sclideColor = {255, 255, 255};
    std::string fastTrackColorLine = "\033[38;2;255;255;255m";
    std::string fastTrackShadeFirst = "\033[38;2;255;255;255m"; // full
    std::string fastTrackShadeSecond = "\033[38;2;193;193;193m"; // full - howMuchToDegrade = 63.7 == 63 for safe guard
    std::string fastTrackShadeThird = "\033[38;2;127;127;127m"; // full - howMuchToDegrade * 2 = 127.4 == 127 for safe guard
    std::string fastTrackShadeFourth = "\033[38;2;63;63;63m"; // full - howMuchToDegrade * 3 = 191.1 == 191 for safe guard
    double speed = 0.1; // Speed in seconds

private:
    void rebuildFastTrackColor();
    void makePreMadeLines();
    void resetPreMadeLines();

public:
    sclide(int startingRow);

    int setSclideColor(int r, int g, int b);
    int setSclideColor(std::string hexCode);

    int setShadeDegradingColor(int r, int g, int b);
    int setShadeDegradingColor(std::string hexCode);

    int resetSclideColor();
    int setSpeed(double newSpeed);
    int changeShades(std::vector<std::string> newShades);

    int clean();
};

// limpa a tela com a animacao do sclide (usado pelos comandos limpar/clear/cls)
void limpar();

}
