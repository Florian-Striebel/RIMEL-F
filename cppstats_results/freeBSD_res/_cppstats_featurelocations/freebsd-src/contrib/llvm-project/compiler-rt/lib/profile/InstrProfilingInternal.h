







#if !defined(PROFILE_INSTRPROFILING_INTERNALH_)
#define PROFILE_INSTRPROFILING_INTERNALH_

#include <stddef.h>

#include "InstrProfiling.h"







uint64_t __llvm_profile_get_size_for_buffer_internal(
const __llvm_profile_data *DataBegin, const __llvm_profile_data *DataEnd,
const uint64_t *CountersBegin, const uint64_t *CountersEnd,
const char *NamesBegin, const char *NamesEnd);










int __llvm_profile_write_buffer_internal(
char *Buffer, const __llvm_profile_data *DataBegin,
const __llvm_profile_data *DataEnd, const uint64_t *CountersBegin,
const uint64_t *CountersEnd, const char *NamesBegin, const char *NamesEnd);











typedef struct ProfDataIOVec {
const void *Data;
size_t ElmSize;
size_t NumElm;
int UseZeroPadding;
} ProfDataIOVec;

struct ProfDataWriter;
typedef uint32_t (*WriterCallback)(struct ProfDataWriter *This, ProfDataIOVec *,
uint32_t NumIOVecs);

typedef struct ProfDataWriter {
WriterCallback Write;
void *WriterCtx;
} ProfDataWriter;




typedef struct ProfBufferIO {
ProfDataWriter *FileWriter;
uint32_t OwnFileWriter;

uint8_t *BufferStart;

uint32_t BufferSz;

uint32_t CurOffset;
} ProfBufferIO;


ProfBufferIO *lprofCreateBufferIOInternal(void *File, uint32_t BufferSz);




ProfBufferIO *lprofCreateBufferIO(ProfDataWriter *FileWriter);





void lprofDeleteBufferIO(ProfBufferIO *BufferIO);





int lprofBufferIOWrite(ProfBufferIO *BufferIO, const uint8_t *Data,
uint32_t Size);




int lprofBufferIOFlush(ProfBufferIO *BufferIO);




uint32_t lprofBufferWriter(ProfDataWriter *This, ProfDataIOVec *IOVecs,
uint32_t NumIOVecs);
void initBufferWriter(ProfDataWriter *BufferWriter, char *Buffer);

struct ValueProfData;
struct ValueProfRecord;
struct InstrProfValueData;
struct ValueProfNode;





typedef struct VPDataReaderType {
uint32_t (*InitRTRecord)(const __llvm_profile_data *Data,
uint8_t *SiteCountArray[]);

uint32_t (*GetValueProfRecordHeaderSize)(uint32_t NumSites);

struct ValueProfRecord *(*GetFirstValueProfRecord)(struct ValueProfData *);

uint32_t (*GetNumValueDataForSite)(uint32_t VK, uint32_t Site);


uint32_t (*GetValueProfDataSize)(void);







struct ValueProfNode *(*GetValueData)(uint32_t ValueKind, uint32_t Site,
struct InstrProfValueData *Dst,
struct ValueProfNode *StartNode,
uint32_t N);
} VPDataReaderType;



int lprofWriteData(ProfDataWriter *Writer, VPDataReaderType *VPDataReader,
int SkipNameDataWrite);
int lprofWriteDataImpl(ProfDataWriter *Writer,
const __llvm_profile_data *DataBegin,
const __llvm_profile_data *DataEnd,
const uint64_t *CountersBegin,
const uint64_t *CountersEnd,
VPDataReaderType *VPDataReader, const char *NamesBegin,
const char *NamesEnd, int SkipNameDataWrite);



void lprofMergeValueProfData(struct ValueProfData *SrcValueProfData,
__llvm_profile_data *DstData);

VPDataReaderType *lprofGetVPDataReader();




void lprofSetMaxValsPerSite(uint32_t MaxVals);
void lprofSetupValueProfiler();






uint64_t lprofGetLoadModuleSignature();





unsigned lprofProfileDumped(void);
void lprofSetProfileDumped(unsigned);

COMPILER_RT_VISIBILITY extern void (*FreeHook)(void *);
COMPILER_RT_VISIBILITY extern uint8_t *DynamicBufferIOBuffer;
COMPILER_RT_VISIBILITY extern uint32_t VPBufferSize;
COMPILER_RT_VISIBILITY extern uint32_t VPMaxNumValsPerSite;

COMPILER_RT_VISIBILITY extern ValueProfNode *CurrentVNode;
COMPILER_RT_VISIBILITY extern ValueProfNode *EndVNode;
extern void (*VPMergeHook)(struct ValueProfData *, __llvm_profile_data *);





int __llvm_write_binary_ids(ProfDataWriter *Writer);

#endif
