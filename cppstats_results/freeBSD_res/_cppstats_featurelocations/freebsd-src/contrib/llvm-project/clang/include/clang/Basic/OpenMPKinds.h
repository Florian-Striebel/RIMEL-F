












#if !defined(LLVM_CLANG_BASIC_OPENMPKINDS_H)
#define LLVM_CLANG_BASIC_OPENMPKINDS_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Frontend/OpenMP/OMPConstants.h"

namespace clang {


using OpenMPDirectiveKind = llvm::omp::Directive;


using OpenMPClauseKind = llvm::omp::Clause;


enum OpenMPScheduleClauseKind {
#define OPENMP_SCHEDULE_KIND(Name) OMPC_SCHEDULE_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_SCHEDULE_unknown
};


enum OpenMPScheduleClauseModifier {
OMPC_SCHEDULE_MODIFIER_unknown = OMPC_SCHEDULE_unknown,
#define OPENMP_SCHEDULE_MODIFIER(Name) OMPC_SCHEDULE_MODIFIER_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_SCHEDULE_MODIFIER_last
};


enum OpenMPDeviceClauseModifier {
#define OPENMP_DEVICE_MODIFIER(Name) OMPC_DEVICE_##Name,
#include "clang/Basic/OpenMPKinds.def"
OMPC_DEVICE_unknown,
};


enum OpenMPDependClauseKind {
#define OPENMP_DEPEND_KIND(Name) OMPC_DEPEND_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_DEPEND_unknown
};


enum OpenMPLinearClauseKind {
#define OPENMP_LINEAR_KIND(Name) OMPC_LINEAR_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_LINEAR_unknown
};


enum OpenMPMapClauseKind {
#define OPENMP_MAP_KIND(Name) OMPC_MAP_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_MAP_unknown
};


enum OpenMPMapModifierKind {
OMPC_MAP_MODIFIER_unknown = OMPC_MAP_unknown,
#define OPENMP_MAP_MODIFIER_KIND(Name) OMPC_MAP_MODIFIER_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_MAP_MODIFIER_last
};


static constexpr unsigned NumberOfOMPMapClauseModifiers =
OMPC_MAP_MODIFIER_last - OMPC_MAP_MODIFIER_unknown - 1;


enum OpenMPMotionModifierKind {
#define OPENMP_MOTION_MODIFIER_KIND(Name) OMPC_MOTION_MODIFIER_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_MOTION_MODIFIER_unknown
};


static constexpr unsigned NumberOfOMPMotionModifiers =
OMPC_MOTION_MODIFIER_unknown;


enum OpenMPDistScheduleClauseKind {
#define OPENMP_DIST_SCHEDULE_KIND(Name) OMPC_DIST_SCHEDULE_##Name,
#include "clang/Basic/OpenMPKinds.def"
OMPC_DIST_SCHEDULE_unknown
};


enum OpenMPDefaultmapClauseKind {
#define OPENMP_DEFAULTMAP_KIND(Name) OMPC_DEFAULTMAP_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_DEFAULTMAP_unknown
};


enum OpenMPDefaultmapClauseModifier {
OMPC_DEFAULTMAP_MODIFIER_unknown = OMPC_DEFAULTMAP_unknown,
#define OPENMP_DEFAULTMAP_MODIFIER(Name) OMPC_DEFAULTMAP_MODIFIER_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_DEFAULTMAP_MODIFIER_last
};


enum OpenMPAtomicDefaultMemOrderClauseKind {
#define OPENMP_ATOMIC_DEFAULT_MEM_ORDER_KIND(Name) OMPC_ATOMIC_DEFAULT_MEM_ORDER_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_ATOMIC_DEFAULT_MEM_ORDER_unknown
};


enum OpenMPDeviceType {
#define OPENMP_DEVICE_TYPE_KIND(Name) OMPC_DEVICE_TYPE_##Name,

#include "clang/Basic/OpenMPKinds.def"
OMPC_DEVICE_TYPE_unknown
};


enum OpenMPLastprivateModifier {
#define OPENMP_LASTPRIVATE_KIND(Name) OMPC_LASTPRIVATE_##Name,
#include "clang/Basic/OpenMPKinds.def"
OMPC_LASTPRIVATE_unknown,
};


enum OpenMPOrderClauseKind {
#define OPENMP_ORDER_KIND(Name) OMPC_ORDER_##Name,
#include "clang/Basic/OpenMPKinds.def"
OMPC_ORDER_unknown,
};


struct OpenMPScheduleTy final {
OpenMPScheduleClauseKind Schedule = OMPC_SCHEDULE_unknown;
OpenMPScheduleClauseModifier M1 = OMPC_SCHEDULE_MODIFIER_unknown;
OpenMPScheduleClauseModifier M2 = OMPC_SCHEDULE_MODIFIER_unknown;
};


enum OpenMPReductionClauseModifier {
#define OPENMP_REDUCTION_MODIFIER(Name) OMPC_REDUCTION_##Name,
#include "clang/Basic/OpenMPKinds.def"
OMPC_REDUCTION_unknown,
};

unsigned getOpenMPSimpleClauseType(OpenMPClauseKind Kind, llvm::StringRef Str,
unsigned OpenMPVersion);
const char *getOpenMPSimpleClauseTypeName(OpenMPClauseKind Kind, unsigned Type);






bool isOpenMPLoopDirective(OpenMPDirectiveKind DKind);





bool isOpenMPWorksharingDirective(OpenMPDirectiveKind DKind);





bool isOpenMPTaskLoopDirective(OpenMPDirectiveKind DKind);





bool isOpenMPParallelDirective(OpenMPDirectiveKind DKind);






bool isOpenMPTargetExecutionDirective(OpenMPDirectiveKind DKind);







bool isOpenMPTargetDataManagementDirective(OpenMPDirectiveKind DKind);







bool isOpenMPNestingTeamsDirective(OpenMPDirectiveKind DKind);





bool isOpenMPTeamsDirective(OpenMPDirectiveKind DKind);





bool isOpenMPSimdDirective(OpenMPDirectiveKind DKind);






bool isOpenMPDistributeDirective(OpenMPDirectiveKind DKind);







bool isOpenMPNestingDistributeDirective(OpenMPDirectiveKind DKind);





bool isOpenMPPrivate(OpenMPClauseKind Kind);





bool isOpenMPThreadPrivate(OpenMPClauseKind Kind);




bool isOpenMPTaskingDirective(OpenMPDirectiveKind Kind);




bool isOpenMPLoopBoundSharingDirective(OpenMPDirectiveKind Kind);




bool isOpenMPLoopTransformationDirective(OpenMPDirectiveKind DKind);


void getOpenMPCaptureRegions(
llvm::SmallVectorImpl<OpenMPDirectiveKind> &CaptureRegions,
OpenMPDirectiveKind DKind);
}

#endif

