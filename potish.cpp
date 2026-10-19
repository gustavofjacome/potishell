#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <limits.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/ioctl.h>
#include <chrono>
#include <thread>



extern char **environ; //variaveis de ambiente permitiu que eu conseguisse compilar com o g++ pelo o shell ela é passada como parametro no meu execve

// defini os cabeçalhos das funções aqui em cima apenas para permitir deixar a main como primeira função
std::vector<std::string> geraVetorAgumentos(std::string comando);
std::vector<char*> converterParaArgv(std::vector<std::string>& args);
void verificacoesErros(const std::string& absolute_path, const std::string& programa);
bool executarComandosInternos(const std::vector<std::string>& args);
void executarComandosExterno(std::vector<std::string>& args);
void process_command(std::string command);
void potishLoop(bool interruptor, std::string nomeShell);

namespace Cor {
    const std::string RESET   = "\033[0m";
    const std::string VERMELHO= "\033[1;31m";
    const std::string VERDE   = "\033[1;32m";
    const std::string AMARELO = "\033[1;33m";
    const std::string AZUL    = "\033[1;34m";
    const std::string CIANO   = "\033[1;36m";

    inline std::string vermelho(const std::string& texto) { 
        return VERMELHO + texto + RESET; 
    }
    inline std::string verde(const std::string& texto)    { 
        return VERDE + texto + RESET; 
    }
    inline std::string amarelo(const std::string& texto)  { 
        return AMARELO + texto + RESET; 
    }
    inline std::string azul(const std::string& texto)     { 
        return AZUL + texto + RESET; 
    }
    inline std::string ciano(const std::string& texto)    { 
        return CIANO + texto + RESET; 
    }
}

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

    // it gonna use formula which is simply: (slcide/red - bg/red) / 4
    int howMuchToDegrade = (sclideColor.r - shadeDegradingColor.r) / 4; // how much shades to degrade

private:
    // rebuild the fast track color lines based on the current sclideColor and shadeDegradingColor
    void rebuildFastTrackColor() {
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

        howMuchToDegrade = std::max({
            std::abs(sclideColor.r - shadeDegradingColor.r),
            std::abs(sclideColor.g - shadeDegradingColor.g),
            std::abs(sclideColor.b - shadeDegradingColor.b)
        }) / 4;

        if (howMuchToDegrade < 1) {
            howMuchToDegrade = 1;
        }

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

private:
    // important to fast track the line printing so i make pre made lines for each shade and full line and space
    void makePreMadeLines() {
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
    void resetPreMadeLines() {
        fullLinePreMade.clear();
        firstShadePreMade.clear();
        secondShadePreMade.clear();
        thirdShadePreMade.clear();
        fourthShadePreMade.clear();
        spacePreMade.clear();
    }

public:
    sclide(int startingRow) : startingRow(startingRow) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        widthOfTerminal = w.ws_col;
        heightOfTerminal = w.ws_row;
        makePreMadeLines();
        rebuildFastTrackColor();
    }

    int setSclideColor(int r, int g, int b) {
        sclideColor.r = r;
        sclideColor.g = g;
        sclideColor.b = b;
        rebuildFastTrackColor();
        return 0;
    }
    int setSclideColor(std::string hexCode) {
        // Convert hex code to rgb
        rgb color;
        color.r = std::stoi(hexCode.substr(1, 2), nullptr, 16);
        color.g = std::stoi(hexCode.substr(3, 2), nullptr, 16);
        color.b = std::stoi(hexCode.substr(5, 2), nullptr, 16);
        sclideColor = color;
        rebuildFastTrackColor();
        return 0;
    }

    int setShadeDegradingColor(int r, int g, int b) {
        shadeDegradingColor.r = r;
        shadeDegradingColor.g = g;
        shadeDegradingColor.b = b;
        rebuildFastTrackColor();
        return 0;
    }
    int setShadeDegradingColor(std::string hexCode) {
        // Convert hex code to rgb
        rgb color;
        color.r = std::stoi(hexCode.substr(1, 2), nullptr, 16);
        color.g = std::stoi(hexCode.substr(3, 2), nullptr, 16);
        color.b = std::stoi(hexCode.substr(5, 2), nullptr, 16);
        shadeDegradingColor = color;
        rebuildFastTrackColor();
        return 0;
    }

    int resetSclideColor() {
        sclideColor = {255, 255, 255};
        rebuildFastTrackColor();
        return 0;
    }

    int setSpeed(double newSpeed) {
        speed = newSpeed;
        return 0;
    }

    int changeShades(std::vector<std::string> newShades) {
        for(int i = 0; i < 4; i++) {
            shades[i] = newShades[i];
        }
        resetPreMadeLines();
        makePreMadeLines();
        return 0;
    }

    int clean(){
        // using a buffer string
        std::string buffer;
        std::string hideCursor = "\033[?25l"; // Hide cursor

        buffer = hideCursor + "\033[" + std::to_string(startingRow) + ";1H" + fullLinePreMade;
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

        // Manual final animation: follow the same pattern used earlier in the method,
        // but apply it to the last visible row so the screen is cleaned smoothly.

        // so i taking exact time the row stopped at so
        int stoppedRow = startingRow + heightOfTerminal - startingRow - 4;
        // now i do stopeedRow++ and remove line then shade then another untill sapce fills last line

        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade + fastTrackShadeThird+thirdShadePreMade + fastTrackShadeSecond+secondShadePreMade + fastTrackShadeFirst+firstShadePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // next transition
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade + fastTrackShadeSecond+secondShadePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // next one
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade + fastTrackShadeThird+thirdShadePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));
        
        // next one
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        // last one
        buffer = "\033[" + std::to_string(stoppedRow++) + ";1H" + spacePreMade;
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        std::this_thread::sleep_for(std::chrono::duration<double>(speed));

        std::cout << "\033[" << startingRow << ";1H"; // Move cursor to the starting row
        std::cout << "\033[0m"; // Reset color
        std::cout << "\033[?25h"; // Show cursor
        return 0;
    }
};
    sclide mySclide(0);
          
    inline void limpar() {
        mySclide.clean();
    }
}

namespace Historico {
    std::deque<std::string> emMemoria; // cache em RAM; o arquivo .potish_historico é o "backup" persistente

    // descobre a pasta onde o executavel do potish esta rodando (nao a pasta atual do terminal)
    // assim o arquivo de historico sempre fica junto do binario, independente de onde o usuario chamou o shell
    inline std::string obterCaminhoArquivo() {
        char buffer[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        std::string pastaExecutavel = "."; // fallback caso o readlink falhe

        if (len != -1) {
            buffer[len] = '\0';
            std::string caminhoCompleto(buffer);
            size_t pos = caminhoCompleto.find_last_of('/');
            if (pos != std::string::npos) {
                pastaExecutavel = caminhoCompleto.substr(0, pos);
            }
        }
        return pastaExecutavel + "/.potish_historico";
    }

    // chamada uma unica vez, no inicio do main(), pra popular a memoria com o que ja foi salvo em execucoes anteriores
    inline void carregar() {
        std::ifstream arquivo(obterCaminhoArquivo());
        if (!arquivo.is_open()) {
            return; // primeira vez rodando o shell, arquivo ainda nem existe
        }

        std::string linha;
        while (std::getline(arquivo, linha)) {
            if (!linha.empty()) {
                // o arquivo guarda do mais antigo (topo) pro mais novo (fim)
                // dar push_front na ordem de leitura deixa o comando mais recente na frente do deque
                emMemoria.push_front(linha);
            }
        }
    }

    inline void adicionar(const std::string& comando) {
        emMemoria.push_front(comando);

        // ios::app so acrescenta a linha no final do arquivo, sem reescrever o historico inteiro
        std::ofstream arquivo(obterCaminhoArquivo(), std::ios::app);
        if (arquivo.is_open()) {
            arquivo << comando << '\n';
        }
    }

    inline void imprimir() {
        for (int i = emMemoria.size() - 1; i >= 0; --i) {
            std::cout << i << " " << emMemoria[i] << '\n';
        }
    }

    inline void limpar() {
        emMemoria.clear();
        // abrir em modo trunc e fechar na hora ja apaga todo o conteudo do arquivo
        std::ofstream arquivo(obterCaminhoArquivo(), std::ios::trunc);
    }

    inline void executarPorOffset(int offset) { 
        if (offset < 0 || (offset + 1) >= (int)emMemoria.size()) {
            throw std::runtime_error("poti$h erro: Offset de history inválido.");
        }
        std::string cmdSalvo = emMemoria[offset + 1];
        std::cout << cmdSalvo << '\n'; 
        process_command(cmdSalvo);    
    }
}

int main() {
    Historico::carregar();
    const std::string nomeBashFormatado = Cor::amarelo("poti$h🦐 ") + Cor::ciano("❯ ");
    potishLoop(true, nomeBashFormatado);
    return 0;
}

std::vector<std::string> geraVetorAgumentos(std::string comando) {
    std::vector<std::string> argumentosString;
    size_t inicio = 0;
    size_t fim = comando.find(' ');

    while (fim != std::string::npos) {
        if (inicio != fim) {
            argumentosString.push_back(comando.substr(inicio, fim - inicio));
        }
        inicio = fim + 1; 
        fim = comando.find(' ', inicio); 
    }
    if (inicio < comando.length()) {
        argumentosString.push_back(comando.substr(inicio));
    }
    return argumentosString;
}

std::vector<char*> converterParaArgv(std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (size_t i = 0; i < args.size(); ++i) {
        argv.push_back(const_cast<char*>(args[i].c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}

void verificacoesErros(const std::string& absolute_path, const std::string& programa) {
    if (access(absolute_path.c_str(), F_OK) != 0) {
        throw std::runtime_error("poti$h erro: Comando não encontrado: " + programa);
    }
    if (access(absolute_path.c_str(), X_OK) != 0) {
        throw std::runtime_error("poti$h erro: Sem permissão para executar: " + programa);
    }
}

bool executarComandosInternos(const std::vector<std::string>& args) {
    if (args.empty()) {
        return false;
    }
    
    std::string comando = args[0];

    if (comando == "sair" || comando == "exit") {
        exit(0);
        return true;
    }
    if (comando == "ajuda" || comando == "help") {
        std::cout << '\n';
        std::cout << "  " << Cor::amarelo("poti$h🦐 ") << "- Comandos Internos Disponíveis (PT/EN)\n";
        std::cout << "  ------------------------------------------------------------\n";
        std::cout << "  " << Cor::ciano("ajuda, help") << "           Exibe este menu de ajuda\n";
        std::cout << "  " << Cor::ciano("cd ") << Cor::verde("[dir]") << "              Muda o diretório atual de trabalho\n";
        std::cout << "  " << Cor::ciano("historico, history") << "    Lista os últimos 10 comandos digitados\n";
        std::cout << "  " << Cor::ciano("historico ") << Cor::verde("-c") << "         Limpa todo o histórico da sessão\n";
        std::cout << "  " << Cor::ciano("historico ") << Cor::verde("[0-9]") << "      Executa um comando salvo pelo seu índice\n";
        std::cout << "  " << Cor::ciano("limpar, clear, cls") << "    Limpa a tela do terminal\n";
        std::cout << "  " << Cor::ciano("potish ") << Cor::verde("-v, --versao") << "   Exibe a versão atual do shell\n";
        std::cout << "  " << Cor::ciano("pwd") << "                   Exibe o caminho do diretório atual\n";
        std::cout << "  " << Cor::ciano("sair, exit") << "            Encerra o shell\n";
        std::cout << '\n';
        return true;
    }
    if (comando == "potish" && args.size() > 1) {
        if (args[1] == "--versao" || args[1] == "-versao" || args[1] == "-v") {
            std::cout << "1.1" << '\n';
            return true;
        }
    }
    if (comando == "limpar" || comando == "clear" || comando == "cls") {
        Terminal::limpar();
        return true;
    }
    if (comando == "cd") {
        if (args.size() < 2) {
            throw std::runtime_error("poti$h erro: 'cd' precisa de um diretório alvo.");
        } else {
            if (chdir(args[1].c_str()) != 0) {
                throw std::runtime_error("poti$h erro: Diretório não encontrado: " + args[1]);
            }
        }
        return true;
    }
    if (comando == "pwd") {
        char buffer[2048];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            std::cout << buffer << '\n';
        } else {
            throw std::runtime_error("poti$h erro: Falha ao obter o diretório atual.");
        }
        return true;
        
    }
    if (comando == "history" || comando == "historico") {
        if (args.size() == 1) {
            Historico::imprimir();
            return true;
        } 
        else if (args.size() == 2) {
            if (args[1] == "-c") {
                Historico::limpar();
                return true;
            } 
            else {
                try {
                    int offset = std::stoi(args[1]); 
                    Historico::executarPorOffset(offset); 
                    return true;
                } catch (const std::invalid_argument&) {
                    throw std::runtime_error("poti$h erro: history offset deve ser um número válido. (1 à 10)");
                }
            }
        }
    }
    return false; // se nao caiu em nenhum é interno
}

void executarComandosExterno(std::vector<std::string>& args) {
    std::string programa = args[0];
    std::string absolute_path = "/bin/" + programa;

    if (std::count(programa.begin(), programa.end(), '/') > 0) {
        absolute_path = programa;
    }

    verificacoesErros(absolute_path, programa);

    pid_t pid = fork();

    if (pid < 0) {
        throw std::invalid_argument("poti$h erro: Falha no (fork) " + programa);
    } else if (pid == 0) {
        std::vector<char*> argv = converterParaArgv(args);
        execve(absolute_path.c_str(), argv.data(), environ);
        exit(1);
    } else {
        waitpid(pid, nullptr, 0);
    }
}

void process_command(std::string command) {
    std::vector<std::string> args = geraVetorAgumentos(command);
    if (args.empty()) {
        return; 
    } 
    if (executarComandosInternos(args)) {
        return; 
    } else {
        executarComandosExterno(args);
    }

    
}

void potishLoop(bool interruptor, std::string nomeShell){
    while (interruptor) {
        std::cout << nomeShell;
        std::string command;
        getline(std::cin, command);

        if (!command.empty()) {
            Historico::adicionar(command);
            try {
                process_command(command);
            }
            catch (const std::exception& erro) {
                std::cout << Cor::vermelho(erro.what()) << "\n";
            }
        }
    }
}