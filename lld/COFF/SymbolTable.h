//===- SymbolTable.h ------------------------------------------------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLD_COFF_SYMBOL_TABLE_H
#define LLD_COFF_SYMBOL_TABLE_H

#include "InputFiles.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
struct LTOCodeGenerator;
}

namespace lld {
namespace coff {

class Chunk;
class Defined;
class Lazy;
class SymbolBody;
struct Symbol;

// SymbolTable is a bucket of all known symbols, including defined,
// undefined, or lazy symbols (the last one is symbols in archive
// files whose archive members are not yet loaded).
//
// We put all symbols of all files to a SymbolTable, and the
// SymbolTable selects the "best" symbols if there are name
// conflicts. For example, obviously, a defined symbol is better than
// an undefined symbol. Or, if there's a conflict between a lazy and a
// undefined, it'll read an archive member to read a real definition
// to replace the lazy symbol. The logic is implemented in resolve().
class SymbolTable {
public:
  SymbolTable();
  void addFile(std::unique_ptr<InputFile> File);
  std::error_code run();
  std::error_code readArchives();
  std::error_code readObjects();
  size_t getVersion() { return Version; }

  // Print an error message on undefined symbols.
  bool reportRemainingUndefines();

  // Returns a list of chunks of selected symbols.
  std::vector<Chunk *> getChunks();

  // Returns a symbol for a given name. It's not guaranteed that the
  // returned symbol actually has the same name (because of various
  // mechanisms to allow aliases, a name can be resolved to a
  // different symbol). Returns a nullptr if not found.
  Defined *find(StringRef Name);
  Symbol *findSymbol(StringRef Name);

  // Find a symbol assuming that Name is a function name.
  // Not only a given string but its mangled names (in MSVC C++ manner)
  // will be searched.
  std::pair<StringRef, Symbol *> findMangled(StringRef Name);

  // Print a layout map to OS.
  void printMap(llvm::raw_ostream &OS);

  // Build a COFF object representing the combined contents of BitcodeFiles
  // and add it to the symbol table. Called after all files are added and
  // before the writer writes results to a file.
  std::error_code addCombinedLTOObject();

  // The writer needs to handle DLL import libraries specially in
  // order to create the import descriptor table.
  std::vector<ImportFile *> ImportFiles;

  // The writer needs to infer the machine type from the object files.
  std::vector<ObjectFile *> ObjectFiles;

  // Creates an Undefined symbol for a given name.
  std::error_code addUndefined(StringRef Name);

  // Rename From -> To in the symbol table.
  std::error_code rename(StringRef From, StringRef To);

  // A list of chunks which to be added to .rdata.
  std::vector<Chunk *> LocalImportChunks;

private:
  std::error_code addSymbol(SymbolBody *New);
  void addLazy(Lazy *New, std::vector<Symbol *> *Accum);

  std::error_code addMemberFile(Lazy *Body);
  ErrorOr<ObjectFile *> createLTOObject(llvm::LTOCodeGenerator *CG);

  llvm::DenseMap<StringRef, Symbol *> Symtab;

  std::vector<std::unique_ptr<InputFile>> Files;
  std::vector<ArchiveFile *> ArchiveQueue;
  std::vector<InputFile *> ObjectQueue;

  std::vector<BitcodeFile *> BitcodeFiles;
  std::unique_ptr<MemoryBuffer> LTOMB;
  llvm::BumpPtrAllocator Alloc;

  // This variable is incremented every time Symtab is updated.
  size_t Version = 0;
};

} // namespace coff
} // namespace lld

#endif
