
# Potishell

> **Nota:** o nome do projeto será alterado de **Vish** para **Potishell**. Com isso, os comandos internos da shell também serão escritos em português.

## Sobre o processo de implementação

Durante o desenvolvimento, inicialmente pensei em criar um mecanismo para armazenar temporariamente binários que não estivessem disponíveis diretamente na pasta `/bin`.

A ideia era que, quando eu estivesse na Shell e executasse algo como:

```bash
./main
```

o programa `main` seria copiado para uma pasta temporária, que estaria configurada como parte do caminho de execução da Shell. Dessa forma, o programa poderia ser executado a partir dessa localização.

No começo, pensei em criar a pasta `/bin/temp`, mas durante a implementação percebi que isso daria errado porque eu não teria acesso ao `sudo` para criar e modificar arquivos dentro de `/bin`. Pesquisando, vi que uma solução desse tipo também exigiria lidar com autenticação e permissões, o que acabaria adicionando uma complexidade desnecessária.

Foi então que percebi que essa pasta não necessariamente precisaria estar dentro da `/bin`: ela poderia ser dinâmica e criada no próprio diretório em que eu estivesse.

A ideia mais recente seria criar um diretório oculto para armazenar temporariamente os programas, executá-los e depois apagar tudo.

Porém, essa abordagem também teria um custo desnecessário, já que seria preciso fazer cópias dos binários e realizar várias operações de leitura e escrita. Imagine quantas cópias seriam feitas caso o diretório tivesse muitos binários.

No fim, depois de pesquisar como as Shells atuais fazem isso, percebi que nenhuma dessas abordagens era realmente necessária e decidi seguir o funcionamento tradicional das Shells. Mas valeu o aprendizado.

## Funcionalidades atuais

A shell suporta processamento de comandos com tratamento de erros, formatação de cores e delegação para processos filhos (via `fork` e `execve`). Os comandos suportados são:

* **Execução de binários no sistema:** Executa nativamente os comandos presentes em `/bin`.
* **Caminhos relativos e absolutos:** Execução de scripts e programas utilizando caminhos específicos (ex: `meucaminho/meucaminhoDois/./teste`).
* **`cd` [dir]:** Comando interno para navegação entre diretórios do sistema.
* **`pwd`:** Exibe o caminho absoluto do diretório de trabalho atual.
* **`history`:** Gerenciador de histórico de comandos.
* Digitar `history` exibe os últimos 10 comandos.
* O uso da flag `-c` limpa o histórico atual.
* O uso de um offset (ex: `history 3`) reexecuta automaticamente um comando salvo.


* **`limpar` / `clear` / `cls`:** Comando interno para limpar o terminal.
* **`sair` / `exit`:** Comando interno para encerrar a shell graciosamente.
* **`potish --versao`:** Exibe a versão atual da shell.
* **`ajuda`:** Exibe o menu de comandos internos disponíveis.

## Aprendizados e Evolução Arquitetural

Durante o desenvolvimento deste projeto, aprendi e apliquei diversos conceitos que melhoraram muito a estrutura do código. Uma ideia que me ajudou bastante a entender melhor o fluxo da aplicação foi desacoplar as responsabilidades da função principal. O código foi modularizado ao ponto da minha função `main` ter apenas 3 linhas. Ainda percebo a existência de algumas "superfunções" (como a `executarComandosInternos`, que concentra vários ifs), mas isso é algo que vou trabalhar mais para frente, possivelmente dividindo o projeto em arquivos de cabeçalho (`.h`).

Para ajudar nessa organização, o uso de `namespace` (como `Cor`, `Terminal` e `Sessao`) foi fundamental para deixar a arquitetura mais limpa e evitar conflitos de nomes. Outro ponto interessante foi lidar com estruturas de dados. Eu já havia implementado estruturas do com coleguinha, mas no Potishell pude aproveitar a biblioteca padrão do C++ utilizando o `std::deque`. De primeira eu havia pensado em usar filas mas pensando melhor o deque era a melhor opção, já que possui complexidade O(1) na inserção e remoção pelas pontas.

Também pude aprofundar meu entendimento sobre o tratamento de exceções em C++ usando blocos `try-catch` e `std::runtime_error`, que é um pouquinho diferente do Java, linguagem com a qual estou mais acostumado. Além disso, aprendi a usar  o `std::stoi` para converter strings em inteiros no sistema de offset do histórico, e entendi o uso de funções `inline` para otimizar chamadas pequenas, mesmo sabendo que compiladores modernos como o GCC já fazem esse tipo de otimização automaticamente por baixo dos panos.

## Referências

Algumas das funcionalidades e conceitos utilizados na implementação foram estudados a partir das seguintes referências:

* [Linux SystemCalls](https://linasm.sourceforge.net/docs/syscalls/filesystem.php#file) -> usei para ver os parametros dos systemcalls
* [Tutorial - Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/) -> muito importante
* [Making Linux Shell in C](https://www.geeksforgeeks.org/c/making-linux-shell-c/) -> muito importante
* [Ask Ubuntu — How to really clear the terminal?](https://askubuntu.com/questions/25077/how-to-really-clear-the-terminal) -> Usei para aprender a limpar o terminal
* [TLDP — Bash Prompt HOWTO: Prompt Expansion](https://tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html)
* [TLDP — Bash Prompt HOWTO](https://tldp.org/HOWTO/Bash-Prompt-HOWTO/c327.html)
