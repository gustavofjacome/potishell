# Potishell

> **Nota:** o nome do projeto será alterado de **Vish** para **Potishell**. A mudança surgiu porque lembrei do professor Lucena. Com isso, os comandos internos da shell também serão escritos em português.

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

No fim, depois de pesquisar como as Shells atuais fazem isso, percebi que nenhuma dessas abordagens era realmente necessária e decidi seguir o funcionamento tradicional das Shells.

Mas valeu o aprendizado.

## Funcionalidades atuais

* Execução de comandos presentes em `/bin`;
* Execução de programas utilizando caminhos absolutos `meucaminho/meucaminhoDois/./teste `;
* Comando interno para sair da Shell;
* Comando interno para exibir a versão da Shell.

## Referências

Algumas das funcionalidades e conceitos utilizados na implementação foram estudados a partir das seguintes referências:

* [Linux SystemCalls](https://linasm.sourceforge.net/docs/syscalls/filesystem.php#file) -> usei para ver os parametros dos systemcalls

* [Tutorial - Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/)    -> muito importante
* [making-linux-shell-c](https://www.geeksforgeeks.org/c/making-linux-shell-c/)         -> muito importante


* [Ask Ubuntu — How to really clear the terminal?](https://askubuntu.com/questions/25077/how-to-really-clear-the-terminal)   -> Usei para aprender a limpar o terminal 
* [TLDP — Bash Prompt HOWTO: Prompt Expansion](https://tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html)
* [TLDP — Bash Prompt HOWTO](https://tldp.org/HOWTO/Bash-Prompt-HOWTO/c327.html)
