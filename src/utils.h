#pragma once

#include <string>
#include <vector>

std::vector<std::string> geraVetorAgumentos(std::string comando);
std::vector<char*> converterParaArgv(std::vector<std::string>& args);
void verificacoesErros(const std::string& absolute_path, const std::string& programa);
void executarComandosExterno(std::vector<std::string>& args);
