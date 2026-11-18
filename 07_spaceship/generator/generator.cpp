#include "generator.hpp"

#include <stdexcept>
#include <utility>

Generator::Generator(std::filesystem::path output_directory)
    : _output_directory(std::move(output_directory))
{
    std::filesystem::create_directories(_output_directory);
    const auto file_path = _output_directory / "generated.hpp";
    _main_output.open(file_path, std::ios::app);
    if (!_main_output.is_open()) {
        throw std::runtime_error("Cannot create file '" + file_path.string() + "'");
    }
}

void Generator::generate(const ClassInfo& class_info)
{
    // Проверка перед генерацией (подходящий ли это класс?)
    // Пропускаем классы с методами не set/get
    for (const auto& method : class_info._methods) {
        if (method._name.substr(0, 3) != "set" && method._name.substr(0, 3) != "get") {
            return;
        }
    }

    // Генерация
    const std::string adapter_name = class_info._name + "Adapter";

    const auto file_path = _output_directory / (adapter_name + ".hpp");

    _main_output << "#include \"" << file_path.string() << "\"\n";

    std::ofstream output(file_path);
    if (!output.is_open()) {
        throw std::runtime_error("Cannot create file '" + file_path.string() + "'");
    }

    output << "#pragma once\n\n";

    // output << "#include <utility>\n";
    output << "#include <ioc.hpp>\n";
    output << "#include \""
           << class_info._header_path
           << "\"\n\n";

    generate_namespace_open(output, class_info._namespace);

    output << "class " << adapter_name << ": public " << class_info._name << "\n{\n";
    output << "    UniversalItem&   _obj;\n";
    output << "    IoC&             _ioc;\n\n";
    output << "public:\n";
    output << "    " << adapter_name << "(IoC& ioc, UniversalItem& obj): _ioc(ioc), _obj(obj) {}\n\n";

    for (const auto& method : class_info._methods) {
        generate_method(output, method);
    }

    output << "};\n";

    generate_namespace_close(output, class_info._namespace);
}

// PRIVATE:
void Generator::generate_namespace_open(std::ostream& output, const std::string& namespace_name) const
{
    if (namespace_name.empty()) {
        return;
    }

    output << "namespace " << namespace_name << " {\n\n";
}

void Generator::generate_namespace_close(std::ostream& output, const std::string& namespace_name) const
{
    if (namespace_name.empty()) {
        return;
    }

    output << "\n} // namespace " << namespace_name << "\n";
}

void Generator::generate_method(std::ostream& output, const MethodInfo& method) const
{
    if (method._argument.size() > 1) {
        // аргумент метода должен быть один
        throw(std::runtime_error("Max argument may be 1"));
    }
    bool is_set = method._name.substr(0, 3) == "set" ? true : false;
    bool is_get = method._name.substr(0, 3) == "get" ? true : false;
    std::string prefix = method._name.substr(0, 3);

    std::string property_name = method._name.substr(3);

    output << "    " << method._return_type << " " << method._name << "(";

    std::string argument_name;
    if (method._argument.size() > 0) {
        argument_name = method._argument[0]._name.empty() ? "new" + property_name : method._argument[0]._name;
        // llvm::outs() << "Arg name: " << method._argument[0]._name << "\n";
        // llvm::outs() << "argument_name: " << argument_name << "\n";
        output << method._argument[0]._type << " " << argument_name;
    }

    output << ")";

    // const
    if (method._is_const) {
        output << " const";
    }

    // volatile
    if (method._is_volatile) {
        output << " volatile";
    }

    // ref qualifier
    if (!method._ref_qualifier.empty()) {
        output << " " << method._ref_qualifier;
    }

    // noexcept
    if (method._is_noexcept) {
        output << " noexcept";
    }

    output << " {\n";

    output << "        ";

    if (method._return_type != "void") {
        output << "return ";
    }

    // output << "_obj." << prefix << "Property";
    output << "_ioc.resolve";// << method._return_type << ">(\"UniversalItem." << prefix <<;
    if (is_get) {
        output << "<" << method._return_type << ">";
    }
    else {
        output << "<" << "std::shared_ptr<ICommand>" << ">";
    }
    output << "(\"UniversalItem." << prefix << property_name << "\"";
    output << ", &_obj";
    if (is_set) {
        output << ", " << argument_name << ")->Execute();\n";
    }
    else {
        output << ");\n";
    }

    output << "    }\n\n";
}
