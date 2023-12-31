#ifndef LLVM_INCLUDES_H
#define LLVM_INCLUDES_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"

#include <map>
#include <memory>

extern std::unique_ptr<llvm::LLVMContext> context;
extern std::unique_ptr<llvm::IRBuilder<>> builder; 
extern std::unique_ptr<llvm::Module> module;

// table containing all the variable id -> memory mappings for all the
// blocks in the current function 
extern std::map<std::string, std::map<std::string, llvm::AllocaInst*>> block_vars;

#endif