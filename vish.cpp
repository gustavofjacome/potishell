#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdexcept> 
#include <sys/stat.h> 
#include <stdio.h>
#include <algorithm>

void process_command(std::string command) {
    // comandos internos
    if (command == "exit") {
        exit(0);
    }

    if (command == "vish --version") {
        std::cout << "1.0" << '\n';
        return;
    }
   
    // caminho onde serão procurados os binarios /bin/comando
    std::string absolute_path = "/bin/" + command;
    // std::string temp_path = "/bin/temp/" + command;

    if (std::count(command.begin(), command.end(), '/') > 0) {
    absolute_path = command;
    }

    if (access(absolute_path.c_str(), F_OK) == 0 /*|| access(temp_path.c_str(), F_OK) == 0*/) { // existe o arquivo?
        if (access(absolute_path.c_str(), X_OK) == 0 /*|| access(temp_path.c_str(), X_OK) == 0*/) { // esse arquivo que existe tem permissão para executar?
            pid_t pid = fork(); // cria o clone

        if (pid < 0) { // o fork retorna 0 ou 1 se deu menor que 0 aconteceu erro
            // Erro
            std::cout << "Erro de execução!" << std::endl;
            return;
        } else if (pid == 0) { // se ele retornou 0 é o filho
            char* argv[2] = {
                (char*)command.c_str(), // como as chamadas de sistemas foram feitas em C temos que transformar a string em um array de char e é isso que o c_str faz transforma o std::string em uma string C
                nullptr
            };
                // if (access(absolute_path.c_str(), F_OK) == 0)
                // {
                     execve(absolute_path.c_str(), argv, NULL); // o filho vai rodar o /bin/comando no outro fork 
                // } else {
                //     execve(temp_path.c_str(), argv, NULL);
                // }
            } else {
                waitpid(pid, nullptr, 0); // enquanto o isso o pai so espera o filho terminar
            }
        } else {
            std::cout << "permission denied: " << command << std::endl; // se nao tem permissão
        }
    } else {
        std::cout << "Command not found: " << command << std::endl; // se nao existe
    }
}


/* Eu tava implementando um jeito de criar uma pasta oculta para poder enviar todos os binários que não estivessem na pasta bin. A ideia era que, quando eu estivesse na Shell e rodasse "./main", esse main seria copiado para a bin/temp e só depois executado, já que a bin/temp estaria configurada na minha Shell. No começo, pensei em criar a /bin/temp, mas durante a implementação percebi que isso daria errado porque eu não teria acesso ao sudo para criar e modificar arquivos dentro de /bin. Pesquisando, vi que uma solução desse tipo também exigiria lidar com autenticação e permissões, o que acabaria adicionando uma complexidade desnecessária. Foi então que percebi que essa pasta não necessariamente precisaria estar dentro da /bin: ela poderia ser dinâmica e criada no próprio diretório em que eu estivesse. A ideia mais recente seria criar um diretório oculto para armazenar temporariamente os programas, executá-los e depois apagar tudo. Porém, essa abordagem também teria um custo desnecessário, já que seria preciso fazer cópias dos binários e realizar várias operações de leitura e escrita. Imagine quantas cópias seriam feitas caso o diretório tivesse muitos binários. No fim, depois de pesquisar como as Shells atuais fazem isso, percebi que nenhuma dessas abordagens era realmente necessária e decidi seguir o funcionamento tradicional das Shells.
 ;( mas valeu o aprendizado */



// ideia antiga pode ignorar

// std::string ocultaDiretorio(const std::string& nomeDir) {
//     if (!nomeDir.empty() && nomeDir[0] == '.') {
//         return nomeDir;
//     }
//     return "." + nomeDir;
// }


// void criaPastaOculta(char* nomeDir){
//     if (nomeDir == NULL || strlen(nomeDir) == 0) { // Adicionado == 0 para lançar a exceção corretamente
//         throw std::invalid_argument("o valor nao pode ser nulo nem vazio");
//     }
//     if (nomeDir[0] == '.') { // ja ta oculto
//         mkdir(nomeDir, 0777); 0777 é as permissoes que a gente tem que somar 
//     } else {
//         std::string diretorioPath = ocultaDiretorio(nomeDir);
//         mkdir(diretorioPath.c_str(), 0777);
//     }
// }





int main() {
    while (true) {
        std::cout << "vi$h> "; 

        std::string command;
        getline(std::cin, command); 
        
        if (!command.empty()) { 
            process_command(command); 
        }
    }

    return 0;
}