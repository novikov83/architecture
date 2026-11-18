#pragma once

#include "ast_analyzer.hpp"

#include <fstream>
#include <filesystem>

class Generator {
private:
    std::filesystem::path   _output_directory;  // директория для генерации
    std::ofstream           _main_output;       // глобальный include-файл

public:
    explicit Generator(std::filesystem::path output_directory);

    void generate(const ClassInfo& class_info);

private:
    void generate_method(std::ostream& output, const MethodInfo& method) const;
    void generate_namespace_open(std::ostream& output, const std::string& namespace_name) const;
    void generate_namespace_close(std::ostream& output, const std::string& namespace_name) const;
};
