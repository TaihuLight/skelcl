#ifndef STOOLING_SOURCE_CODE_H_
#define STOOLING_SOURCE_CODE_H_

#include <string>
#include <vector>

#include "detail/stoolingDll.h"

namespace stooling {

class RefactoringTool;

class STOOLING_API SourceCode {
public:
  SourceCode() = delete;

  SourceCode(const std::string& source);

  SourceCode(const SourceCode& rhs);
  SourceCode(SourceCode&& rhs);

  SourceCode& operator=(const SourceCode& rhs);
  SourceCode& operator=(SourceCode&& rhs);
  
  ~SourceCode();

  void transferParameters(const std::string& from,
                          unsigned int startIndex,
                          const std::string& to);

  void transferArguments(const std::string& from,
                         unsigned int startIndex,
                         const std::string& to);

  void renameFunction(const std::string& from, const std::string& to);

  void renameTypedef(const std::string& from, const std::string& to);

  void redefineTypedef(const std::string& typedefName,
                       const std::string& typeName);

  void fixKernelParameter(const std::string& kernel);

  const std::string& code() const;

  std::vector<std::string> parameterTypeNames(const std::string& funcName) const;

private:

  std::string       _source;
  RefactoringTool*  _tool;
};

} // namespace stooling

#endif // STOOLING_SOURCE_CODE_H_

