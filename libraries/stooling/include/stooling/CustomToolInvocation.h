#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wcast-align"
#ifdef __clang__
# pragma GCC diagnostic ignored "-Wshift-sign-overflow"
# if (__clang_major__ >= 3 && __clang_minor__ >= 3)
#   pragma GCC diagnostic ignored "-Wduplicate-enum"
# endif
#endif

#include <clang/Basic/FileManager.h>
#include <clang/Driver/Compilation.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>

#pragma GCC diagnostic pop

#include <map>
#include <string>
#include <vector>

#ifndef CUSTOM_TOOL_INVOCATION_H
#define CUSTOM_TOOL_INVOCATION_H

namespace stooling {

class CustomToolInvocation {
public:
  CustomToolInvocation(const std::string& code,
                       const std::vector<std::string>& args
                          = std::vector<std::string>({ "-x", "cl" }));

  ~CustomToolInvocation();

  bool run(clang::FrontendAction* action);

  clang::SourceManager& getSources();

  const std::string& code() const;

private:
  void addFileMappingsTo(clang::SourceManager& SourceManager);

  bool runInvocation(clang::FrontendAction* action,
                     clang::driver::Compilation *Compilation,
                     clang::CompilerInvocation *Invocation);

  clang::CompilerInstance   _compiler;
  std::string               _fileName;
  std::vector<std::string>  _commandLine;
  clang::FileManager        _files;
  std::string               _fileContent;
};

} // namespace stooling

#endif

