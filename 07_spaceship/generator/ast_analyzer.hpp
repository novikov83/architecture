#pragma once

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/ASTContext.h>

#include <string>
#include <vector>

struct ArgumentInfo
{
    std::string _name;
    std::string _type;
};

struct MethodInfo
{
    std::string _name;
    std::string _return_type;

    std::vector<ArgumentInfo> _argument;

    bool _is_const = false;
    bool _is_volatile = false;
    bool _is_noexcept = false;

    std::string _ref_qualifier;
};

struct ClassInfo
{
    std::string _name;
    std::string _namespace;
    std::string _qualified_name;
    std::string _header_path;

    std::vector<MethodInfo> _methods;
};

class AstAnalyzer: public clang::RecursiveASTVisitor<AstAnalyzer>
{
    clang::ASTContext&      _context;
    std::vector<ClassInfo>  _classes;

public:
    explicit AstAnalyzer(clang::ASTContext& context);

    // Метод визитора (вызывается при каждом посещении нового класса)
    bool VisitCXXRecordDecl(clang::CXXRecordDecl* declaration);
    const std::vector<ClassInfo>& get_classes() const;

private:
    std::string get_namespace(const clang::DeclContext* context) const;
    std::string get_header_path(clang::SourceLocation location) const;
    MethodInfo parse_method(const clang::CXXMethodDecl* method) const;
};
