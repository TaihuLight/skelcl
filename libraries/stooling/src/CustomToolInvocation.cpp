#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#endif

#include <llvm/Support/Host.h>
#include <clang/Basic/FileManager.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/Tool.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#if (LLVM_VERSION_MAJOR >= 3 && LLVM_VERSION_MINOR <= 4)
  #include <llvm/ADT/OwningPtr.h>
#else
  #include <memory>
  template<class T> using OwningPtr = std::unique_ptr<T>;
#endif

#pragma GCC diagnostic pop

#include "stooling/CustomToolInvocation.h"


#include <iostream>

using namespace llvm;
using namespace clang;

namespace {

clang::driver::Driver* newDriver(clang::DiagnosticsEngine* diagnostics,
                                 const char* binaryName)
{
  const std::string defaultOutputName = "a.out";
  auto compilerDriver =
    new clang::driver::Driver(
      binaryName, llvm::sys::getDefaultTargetTriple(),
#if (LLVM_VERSION_MAJOR >= 3 && LLVM_VERSION_MINOR <= 4)
      defaultOutputName,
#endif
      *diagnostics);
  compilerDriver->setTitle("clang_based_tool");
  return compilerDriver;
}

const clang::driver::ArgStringList* getCC1Arguments(
    clang::DiagnosticsEngine* diagnostics,
    clang::driver::Compilation* compilation)
{
  // We expect to get back exactly one Command job, if we didn't something
  // failed. Extract that job from the Compilation.
  auto& jobs = compilation->getJobs();
  if (jobs.size() != 1 || !isa<clang::driver::Command>(*jobs.begin())) {
    SmallString<256> error_msg;
    llvm::raw_svector_ostream error_stream(error_msg);
#if (LLVM_VERSION_MAJOR >= 3 && LLVM_VERSION_MINOR <= 3)
    compilation->PrintJob(error_stream, compilation->getJobs(), "; ", true);
#else
		for (auto& j : jobs) {
			j->Print(error_stream, "; ", true);
		}
#endif
    diagnostics->Report(clang::diag::err_fe_expected_compiler_job)
        << error_stream.str();
    return NULL;
  }

  // The one job we find should be to invoke clang again.
  auto cmd = cast<clang::driver::Command>(*jobs.begin());
  if (StringRef(cmd->getCreator().getName()) != "clang") {
    diagnostics->Report(clang::diag::err_fe_expected_clang_command);
    return NULL;
  }

  return &cmd->getArguments();
}

/// \brief Returns a clang build invocation initialized from the CC1 flags.
clang::CompilerInvocation* newInvocation(
    clang::DiagnosticsEngine* diagnostics,
    const clang::driver::ArgStringList& CC1Args) {
  assert(!CC1Args.empty() && "Must at least contain the program name!");
  auto invocation = new clang::CompilerInvocation;
  clang::CompilerInvocation::CreateFromArgs(
      *invocation, CC1Args.data() + 1, CC1Args.data() + CC1Args.size(),
      *diagnostics);
  invocation->getFrontendOpts().DisableFree = false;
  return invocation;
}

} // namespace

namespace stooling {

CustomToolInvocation::CustomToolInvocation(const std::string& code,
                                           const std::vector<std::string>& args)
  : _compiler(),
    _fileName("input.cc"),
    _commandLine({ "clang-tool",
                   "-fsyntax-only" }),
    _files((FileSystemOptions())),
    _fileContent(code)
{
  _commandLine.insert(_commandLine.end(), args.begin(), args.end());
  _commandLine.push_back(_fileName);
}

CustomToolInvocation::~CustomToolInvocation()
{
  _compiler.resetAndLeakFileManager();
  _files.clearStatCaches();
}

clang::SourceManager& CustomToolInvocation::getSources()
{
  return _compiler.getSourceManager();
}

bool CustomToolInvocation::run(clang::FrontendAction* action)
{
  std::vector<const char*> argv;
  for (size_t i = 0, e = _commandLine.size(); i != e; ++i) {
    argv.push_back(_commandLine[i].c_str());
  }
  const char *const binaryName = argv[0];
  IntrusiveRefCntPtr<DiagnosticOptions> diagOpts = new DiagnosticOptions();
  //TextDiagnosticPrinter diagnosticPrinter(llvm::errs(), &*diagOpts);
  DiagnosticsEngine diagnostics(
    IntrusiveRefCntPtr<clang::DiagnosticIDs>(new DiagnosticIDs()),
    &*diagOpts);


  const OwningPtr<clang::driver::Driver> driver(
      newDriver(&diagnostics, binaryName));
  // Since the input might only be virtual, don't check whether it exists.
  driver->setCheckInputsExist(false);
  const OwningPtr<clang::driver::Compilation> compilation(
      driver->BuildCompilation(llvm::makeArrayRef(argv)));
  auto CC1Args = getCC1Arguments(&diagnostics, compilation.get());
  if (CC1Args == NULL) {
    return false;
  }
  OwningPtr<clang::CompilerInvocation> invocation(
      newInvocation(&diagnostics, *CC1Args));
  return runInvocation(action, compilation.get(),
#if (LLVM_VERSION_MAJOR >= 3 && LLVM_VERSION_MINOR <= 4)
                       invocation.take()
#else
                       invocation.release()
#endif
                      );
}

bool CustomToolInvocation::runInvocation(
    clang::FrontendAction* action,
    clang::driver::Compilation* compilation,
    clang::CompilerInvocation* invocation)
{
  // Show the invocation, with -v.
  if (invocation->getHeaderSearchOpts().Verbose) {
    llvm::errs() << "clang Invocation:\n";
#if (LLVM_VERSION_MAJOR >= 3 && LLVM_VERSION_MINOR <= 3)
    compilation->PrintJob(llvm::errs(), compilation->getJobs(), "\n", true);
#else
		for (auto& j : compilation->getJobs()) {
			j->Print(llvm::errs(), "\n", true);
		}
#endif
    llvm::errs() << "\n";
  }

  // Create a compiler instance to handle the actual work.
  _compiler.setInvocation(invocation);
  _compiler.setFileManager(&_files);
  // FIXME: What about LangOpts?

  // ToolAction can have lifetime requirements for Compiler or its members, and
  // we need to ensure it's deleted earlier than Compiler. So we pass it to an
  // OwningPtr declared after the Compiler variable.
  OwningPtr<FrontendAction> scopedToolAction(action);

  // Create the compilers actual diagnostics engine.
  _compiler.createDiagnostics();
  if (!_compiler.hasDiagnostics())
    return false;

  _compiler.createSourceManager(_files);
  addFileMappingsTo(_compiler.getSourceManager());

  // suppress all diagnostics
  _compiler.getDiagnostics().setSuppressAllDiagnostics();

  return _compiler.ExecuteAction(*scopedToolAction);
}

const std::string& CustomToolInvocation::code() const
{
  return _fileContent;
}

void CustomToolInvocation::addFileMappingsTo(SourceManager& sources) {
  // Inject the code as the given file name into the preprocessor options.
  auto input = llvm::MemoryBuffer::getMemBuffer(_fileContent);
  // FIXME: figure out what '0' stands for.
  auto fromFile = _files.getVirtualFile(_fileName,
      static_cast<long>(input->getBufferSize()), 0);
  sources.overrideFileContents(fromFile, input);
}

} // namespace stooling

