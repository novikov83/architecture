#include "ast_analyzer.hpp"

#include <clang/Basic/SourceManager.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/Type.h>

#include <llvm/Support/Casting.h>

#include <algorithm>

AstAnalyzer::AstAnalyzer(clang::ASTContext& context): _context(context)
{
}

bool AstAnalyzer::VisitCXXRecordDecl(clang::CXXRecordDecl* declaration)
{
    // Анализируем только текущий заголовок, а не его транзитивные include
    auto& source_manager = _context.getSourceManager();
    if (!source_manager.isInMainFile(
            source_manager.getExpansionLoc(
                declaration->getLocation()
            ))) {
        return true;
    }

    if (!declaration->isThisDeclarationADefinition()) {
        return true;
    }

    if (declaration->isImplicit()) {
        return true;
    }

    // Пропускаем не абстрактные классы
    if (!declaration->isAbstract()) {
        return true;
    }

    // Пропускаем лямбды
    if (declaration->isLambda()) {
        return true;
    }

    // Пропускаем системное
    if (_context.getSourceManager().isInSystemHeader(declaration->getLocation())) {
        return true;
    }

    // Пропускаем forward declaration и служебные объявления
    if (declaration->getIdentifier() == nullptr) {
        return true;
    }

    ClassInfo class_info;

    class_info._name = declaration->getNameAsString();
    class_info._qualified_name = declaration->getQualifiedNameAsString();
    class_info._namespace = get_namespace(declaration->getDeclContext());
    class_info._header_path = get_header_path(declaration->getLocation());

    // Получаем чисто виртуальные методы
    for (const auto* method : declaration->methods()) {
        if (!method->isPureVirtual()) {
            continue;
        }

        // Деструктор не генерируем как обычный метод
        if (method->isVirtualAsWritten() && method->isDefaulted()) {
            continue;
        }

        class_info._methods.push_back(parse_method(method));
    }

    if (!class_info._methods.empty()) {
        _classes.push_back(std::move(class_info));
    }

    return true;
}

const std::vector<ClassInfo>& AstAnalyzer::get_classes() const
{
    return _classes;
}

// PRIVATE:
std::string AstAnalyzer::get_namespace(const clang::DeclContext* context) const
{
    std::vector<std::string> namespaces;

    // Смотрим вложенность namespace'ов
    while (context != nullptr) {
        if (const auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(context)) {
            if (!ns->isAnonymousNamespace()) {
                namespaces.push_back(ns->getNameAsString());
            }
        }
        context = context->getParent();
    }
    std::reverse(namespaces.begin(), namespaces.end());

    // Формирование строки namespace'ов
    std::string result;
    for (const auto& ns : namespaces) {
        if (!result.empty()) {
            result += "::";
        }
        result += ns;
    }

    return result;
}

std::string AstAnalyzer::get_header_path(clang::SourceLocation location) const
{
    auto& source_manager = _context.getSourceManager();

    location = source_manager.getSpellingLoc(location);

    return source_manager.getFilename(location).str();
}

MethodInfo AstAnalyzer::parse_method(const clang::CXXMethodDecl* method) const
{
    MethodInfo result;

    clang::PrintingPolicy policy(_context.getLangOpts());

    result._name = method->getNameAsString();
    result._return_type = method->getReturnType().getAsString(policy);

    result._is_const = method->isConst();
    result._is_volatile = method->isVolatile();

    // Получаем параметры (название и тип)
    for (const auto* parameter : method->parameters()) {
        result._argument.push_back({
            parameter->getNameAsString(),
            parameter->getType().getAsString(policy)
        });
    }

    // Проверяем noexcept
    if (const auto* function_type = method->getType()->getAs<clang::FunctionProtoType>()) {
        result._is_noexcept = function_type->isNothrow();
    }

    // Проверяем ref-квалификатор
    switch (method->getRefQualifier()) {
        case clang::RQ_LValue:
            result._ref_qualifier = "&";
            break;

        case clang::RQ_RValue:
            result._ref_qualifier = "&&";
            break;

        case clang::RQ_None:
        default:
            break;
    }

    return result;
}
