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
#include <iostream>
// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;


StatementMatcher LoopMatcher =
    forStmt(hasLoopInit(declStmt(
                hasSingleDecl(varDecl(hasInitializer(integerLiteral(equals(0))))
                                  .bind("initVarName")))),
            hasIncrement(unaryOperator(
                hasOperatorName("++"),
                hasUnaryOperand(declRefExpr(
                    to(varDecl(hasType(isInteger())).bind("incVarName")))))),
            hasCondition(binaryOperator(
                hasOperatorName("<"),
                hasLHS(ignoringParenImpCasts(declRefExpr(
                    to(varDecl(hasType(isInteger())).bind("condVarName"))))),
                hasRHS(expr(hasType(isInteger())))))).bind("forLoop");

StatementMatcher AssignMatcher = binaryOperator(hasOperatorName("=")).bind("Assign");

static bool areSameVariable(const ValueDecl *First, const ValueDecl *Second) {
  return First && Second &&
         First->getCanonicalDecl() == Second->getCanonicalDecl();
}

class LoopPrinter : public MatchFinder::MatchCallback {
public :
  void run(const MatchFinder::MatchResult &Result) {
    ASTContext *Context = Result.Context;
    const ForStmt *FS = Result.Nodes.getNodeAs<ForStmt>("forLoop");
    // We do not want to convert header files!
    if (!FS || !Context->getSourceManager().isWrittenInMainFile(FS->getForLoc()))
      return;
    const VarDecl *IncVar = Result.Nodes.getNodeAs<VarDecl>("incVarName");
    const VarDecl *CondVar = Result.Nodes.getNodeAs<VarDecl>("condVarName");
    const VarDecl *InitVar = Result.Nodes.getNodeAs<VarDecl>("initVarName");

    if (!areSameVariable(IncVar, CondVar) || !areSameVariable(IncVar, InitVar))
      return;
    llvm::outs() << "Potential array-based loop discovered.\n";
  }
};

class MyCallback : public MatchFinder::MatchCallback {
 public:
  virtual void run(const MatchFinder::MatchResult &Result){
    std::cout << "Assign" << std::endl;
    if(const DeclStmt *DS = Result.Nodes.getNodeAs<clang::DeclStmt>("Assign"))
      DS->dump();

    }
};




int main(int argc, const char **argv) {
  auto CFGCategory = llvm::cl::OptionCategory("LOOP");
  CommonOptionsParser OptionsParser(argc, argv, CFGCategory);
  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
  LoopPrinter Printer;
  MyCallback Mycallback;
  MatchFinder Finder;
  Finder.addMatcher(LoopMatcher, &Printer);
  Finder.addMatcher(AssignMatcher, &Mycallback);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}