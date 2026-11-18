#include "ast_analyzer.hpp"
#include "generator.hpp"

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <string>

// Опции командной строки
static llvm::cl::OptionCategory GeneratorCategory(
    "generator options"
);
// Опция задающая папку для сгенерированного кода
static llvm::cl::opt<std::string> OutputDirectory(
    "output-dir",                                   // название опции (--output-dir)
    llvm::cl::desc("Generated files directory"),    // описание
    llvm::cl::init("generated"),                    // значение по-умолчанию
    llvm::cl::cat(GeneratorCategory)                // привязка
);


class GeneratorConsumer: public clang::ASTConsumer {
public:

    // Вызывается когда AST готов
    void HandleTranslationUnit(clang::ASTContext& context) override {
        AstAnalyzer analyzer(context);

        // запуск обхода AST
        analyzer.TraverseDecl(context.getTranslationUnitDecl());

        Generator generator(OutputDirectory.getValue());

        for (const auto& class_info : analyzer.get_classes()) {
            llvm::outs()
                << "Generating adapter for: "
                << class_info._qualified_name
                << "\n";

            generator.generate(class_info);
        }
    }
};


// clang вызывает мой CreateASTConsumer() для каждого файла
class GeneratorAction: public clang::ASTFrontendAction {
public:
    std::unique_ptr<clang::ASTConsumer>
    CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override {
        return std::make_unique<GeneratorConsumer>();
    }
};

int main(int argc, const char** argv)
{
    auto parser_result =
        clang::tooling::CommonOptionsParser::create(
            argc,
            argv,
            GeneratorCategory
        );

    if (!parser_result) {
        llvm::errs()
            << llvm::toString(parser_result.takeError())
            << "\n";

        return 1;
    }

    auto& options = parser_result.get();

    // создание tool который и будет выполнять всю работу
    clang::tooling::ClangTool tool(
        options.getCompilations(),
        options.getSourcePathList()
    );

    // фабрика по созданию моего GeneratorAction
    auto factory = clang::tooling::newFrontendActionFactory<GeneratorAction>();

    // проход по всем файлам и вызов моего GeneratorAction
    return tool.run(factory.get());
}