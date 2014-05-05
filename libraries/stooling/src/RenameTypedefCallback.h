#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Refactoring.h>

#pragma GCC diagnostic pop

#include <string>

#ifndef RENAME_TYPEDEF_CALLBACK_H
#define RENAME_TYPEDEF_CALLBACK_H

namespace stooling {

class RenameTypedefCallback
  : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
  RenameTypedefCallback(clang::tooling::Replacements& replacements,
                         const std::string& oldName,
                         const std::string& newName);

  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& result);

private:
  clang::tooling::Replacements& _replacements;
  const std::string&            _oldName;
  const std::string&            _newName;
};

} // namespace stooling

#endif // RENAME_FUNCTION_CALLBACK_H

