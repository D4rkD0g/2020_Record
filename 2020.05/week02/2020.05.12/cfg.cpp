// Clang includes
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Analysis/CFG.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

class MyCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  MyCallback(){}
  void run(const clang::ast_matchers::MatchFinder::MatchResult &Result){
    const auto* Function = Result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
    const auto CFG = clang::CFG::buildCFG(Function,
                                        Function->getBody(),
                                        Result.Context,
                                        clang::CFG::BuildOptions());
    for (const auto* blk : *CFG){
      blk->dump();        // Prints Basic Blocks.
      // do something more.
    }
  }
};

class MyConsumer : public clang::ASTConsumer {
public:
  explicit MyConsumer() : handler() {
    const auto matching_node = clang::ast_matchers::functionDecl(clang::ast_matchers::isExpansionInMainFile()).bind("fn");
    match_finder.addMatcher(matching_node, &handler);
  }
  void HandleTranslationUnit(clang::ASTContext& ctx) {
    match_finder.matchAST(ctx);
  }  
private:  
  MyCallback handler;  
  clang::ast_matchers::MatchFinder match_finder;  
};

class MyFrontendAction : public clang::ASTFrontendAction {
 public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override {
    return std::make_unique<MyConsumer>();
  }
};

struct ToolFactory : public clang::tooling::FrontendActionFactory {
  clang::FrontendAction* create() override {
    return new MyFrontendAction();
  }
};

int main(int argc, const char **argv) {
  auto CFGCategory = llvm::cl::OptionCategory("CFG");
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv, CFGCategory);
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
  // run the Clang Tool, creating a new FrontendAction.
  return Tool.run(new ToolFactory);
}