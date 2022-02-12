













#if !defined(LLVM_CLANG_ANALYSES_DATAFLOW_VALUES)
#define LLVM_CLANG_ANALYSES_DATAFLOW_VALUES

#include "clang/Analysis/CFG.h"
#include "clang/Analysis/ProgramPoint.h"
#include "llvm/ADT/DenseMap.h"

namespace clang {







namespace dataflow {
struct forward_analysis_tag {};
struct backward_analysis_tag {};
}





template <typename ValueTypes,
typename _AnalysisDirTag = dataflow::forward_analysis_tag >
class DataflowValues {





public:
typedef typename ValueTypes::ValTy ValTy;
typedef typename ValueTypes::AnalysisDataTy AnalysisDataTy;
typedef _AnalysisDirTag AnalysisDirTag;
typedef llvm::DenseMap<ProgramPoint, ValTy> EdgeDataMapTy;
typedef llvm::DenseMap<const CFGBlock*, ValTy> BlockDataMapTy;
typedef llvm::DenseMap<const Stmt*, ValTy> StmtDataMapTy;





public:


bool isForwardAnalysis() { return isForwardAnalysis(AnalysisDirTag()); }



bool isBackwardAnalysis() { return !isForwardAnalysis(); }

private:
bool isForwardAnalysis(dataflow::forward_analysis_tag) { return true; }
bool isForwardAnalysis(dataflow::backward_analysis_tag) { return false; }





public:
DataflowValues() : StmtDataMap(NULL) {}
~DataflowValues() { delete StmtDataMap; }



void InitializeValues(const CFG& cfg) {}




ValTy& getEdgeData(const BlockEdge &E) {
typename EdgeDataMapTy::iterator I = EdgeDataMap.find(E);
assert (I != EdgeDataMap.end() && "No data associated with Edge.");
return I->second;
}

const ValTy& getEdgeData(const BlockEdge &E) const {
return reinterpret_cast<DataflowValues*>(this)->getEdgeData(E);
}





ValTy& getBlockData(const CFGBlock *B) {
typename BlockDataMapTy::iterator I = BlockDataMap.find(B);
assert (I != BlockDataMap.end() && "No data associated with block.");
return I->second;
}

const ValTy& getBlockData(const CFGBlock *B) const {
return const_cast<DataflowValues*>(this)->getBlockData(B);
}







ValTy& getStmtData(const Stmt *S) {
assert (StmtDataMap && "Dataflow values were not computed for statements.");
typename StmtDataMapTy::iterator I = StmtDataMap->find(S);
assert (I != StmtDataMap->end() && "No data associated with statement.");
return I->second;
}

const ValTy& getStmtData(const Stmt *S) const {
return const_cast<DataflowValues*>(this)->getStmtData(S);
}




EdgeDataMapTy& getEdgeDataMap() { return EdgeDataMap; }
const EdgeDataMapTy& getEdgeDataMap() const { return EdgeDataMap; }






BlockDataMapTy& getBlockDataMap() { return BlockDataMap; }
const BlockDataMapTy& getBlockDataMap() const { return BlockDataMap; }



StmtDataMapTy& getStmtDataMap() {
if (!StmtDataMap) StmtDataMap = new StmtDataMapTy();
return *StmtDataMap;
}

const StmtDataMapTy& getStmtDataMap() const {
return const_cast<DataflowValues*>(this)->getStmtDataMap();
}





AnalysisDataTy& getAnalysisData() { return AnalysisData; }
const AnalysisDataTy& getAnalysisData() const { return AnalysisData; }





protected:
EdgeDataMapTy EdgeDataMap;
BlockDataMapTy BlockDataMap;
StmtDataMapTy* StmtDataMap;
AnalysisDataTy AnalysisData;
};

}
#endif
