









#if !defined(LLVM_FUZZER_IO_H)
#define LLVM_FUZZER_IO_H

#include "FuzzerDefs.h"

namespace fuzzer {

long GetEpoch(const std::string &Path);

Unit FileToVector(const std::string &Path, size_t MaxSize = 0,
bool ExitOnError = true);

std::string FileToString(const std::string &Path);

void CopyFileToErr(const std::string &Path);

void WriteToFile(const uint8_t *Data, size_t Size, const std::string &Path);

void WriteToFile(const std::string &Data, const std::string &Path);
void WriteToFile(const Unit &U, const std::string &Path);

void AppendToFile(const uint8_t *Data, size_t Size, const std::string &Path);
void AppendToFile(const std::string &Data, const std::string &Path);

void ReadDirToVectorOfUnits(const char *Path, Vector<Unit> *V, long *Epoch,
size_t MaxSize, bool ExitOnError,
Vector<std::string> *VPaths = 0);


std::string DirPlusFile(const std::string &DirPath,
const std::string &FileName);


std::string DirName(const std::string &FileName);


std::string TmpDir();

std::string TempPath(const char *Prefix, const char *Extension);

bool IsInterestingCoverageFile(const std::string &FileName);

void DupAndCloseStderr();

void CloseStdout();

void Printf(const char *Fmt, ...);
void VPrintf(bool Verbose, const char *Fmt, ...);


void RawPrint(const char *Str);


bool IsFile(const std::string &Path);
bool IsDirectory(const std::string &Path);
size_t FileSize(const std::string &Path);

void ListFilesInDirRecursive(const std::string &Dir, long *Epoch,
Vector<std::string> *V, bool TopDir);

bool MkDirRecursive(const std::string &Dir);
void RmDirRecursive(const std::string &Dir);




void IterateDirRecursive(const std::string &Dir,
void (*DirPreCallback)(const std::string &Dir),
void (*DirPostCallback)(const std::string &Dir),
void (*FileCallback)(const std::string &Dir));

struct SizedFile {
std::string File;
size_t Size;
bool operator<(const SizedFile &B) const { return Size < B.Size; }
};

void GetSizedFilesFromDir(const std::string &Dir, Vector<SizedFile> *V);

char GetSeparator();
bool IsSeparator(char C);

std::string Basename(const std::string &Path);

FILE* OpenFile(int Fd, const char *Mode);

int CloseFile(int Fd);

int DuplicateFile(int Fd);

void RemoveFile(const std::string &Path);
void RenameFile(const std::string &OldPath, const std::string &NewPath);

intptr_t GetHandleFromFd(int fd);

void MkDir(const std::string &Path);
void RmDir(const std::string &Path);

const std::string &getDevNull();

}

#endif
