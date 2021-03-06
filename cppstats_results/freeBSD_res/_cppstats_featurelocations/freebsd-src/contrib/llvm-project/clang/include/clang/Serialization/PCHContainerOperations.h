







#if !defined(LLVM_CLANG_SERIALIZATION_PCHCONTAINEROPERATIONS_H)
#define LLVM_CLANG_SERIALIZATION_PCHCONTAINEROPERATIONS_H

#include "clang/Basic/Module.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/MemoryBuffer.h"
#include <memory>

namespace llvm {
class raw_pwrite_stream;
}

namespace clang {

class ASTConsumer;
class CodeGenOptions;
class DiagnosticsEngine;
class CompilerInstance;

struct PCHBuffer {
ASTFileSignature Signature;
llvm::SmallVector<char, 0> Data;
bool IsComplete;
};




class PCHContainerWriter {
public:
virtual ~PCHContainerWriter() = 0;
virtual llvm::StringRef getFormat() const = 0;




virtual std::unique_ptr<ASTConsumer>
CreatePCHContainerGenerator(CompilerInstance &CI,
const std::string &MainFileName,
const std::string &OutputFileName,
std::unique_ptr<llvm::raw_pwrite_stream> OS,
std::shared_ptr<PCHBuffer> Buffer) const = 0;
};




class PCHContainerReader {
public:
virtual ~PCHContainerReader() = 0;

virtual llvm::StringRef getFormat() const = 0;


virtual llvm::StringRef ExtractPCH(llvm::MemoryBufferRef Buffer) const = 0;
};


class RawPCHContainerWriter : public PCHContainerWriter {
llvm::StringRef getFormat() const override { return "raw"; }



std::unique_ptr<ASTConsumer>
CreatePCHContainerGenerator(CompilerInstance &CI,
const std::string &MainFileName,
const std::string &OutputFileName,
std::unique_ptr<llvm::raw_pwrite_stream> OS,
std::shared_ptr<PCHBuffer> Buffer) const override;
};


class RawPCHContainerReader : public PCHContainerReader {
llvm::StringRef getFormat() const override { return "raw"; }


llvm::StringRef ExtractPCH(llvm::MemoryBufferRef Buffer) const override;
};


class PCHContainerOperations {
llvm::StringMap<std::unique_ptr<PCHContainerWriter>> Writers;
llvm::StringMap<std::unique_ptr<PCHContainerReader>> Readers;
public:


PCHContainerOperations();
void registerWriter(std::unique_ptr<PCHContainerWriter> Writer) {
Writers[Writer->getFormat()] = std::move(Writer);
}
void registerReader(std::unique_ptr<PCHContainerReader> Reader) {
Readers[Reader->getFormat()] = std::move(Reader);
}
const PCHContainerWriter *getWriterOrNull(llvm::StringRef Format) {
return Writers[Format].get();
}
const PCHContainerReader *getReaderOrNull(llvm::StringRef Format) {
return Readers[Format].get();
}
const PCHContainerReader &getRawReader() {
return *getReaderOrNull("raw");
}
};

}

#endif
