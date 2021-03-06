









































#define NM_TAR "tar"
#define NM_CPIO "cpio"
#define NM_PAX "pax"






#define NONE "none"




#define AF 0x00000001
#define BF 0x00000002
#define CF 0x00000004
#define DF 0x00000008
#define FF 0x00000010
#define IF 0x00000020
#define KF 0x00000040
#define LF 0x00000080
#define NF 0x00000100
#define OF 0x00000200
#define PF 0x00000400
#define RF 0x00000800
#define SF 0x00001000
#define TF 0x00002000
#define UF 0x00004000
#define VF 0x00008000
#define WF 0x00010000
#define XF 0x00020000
#define CBF 0x00040000
#define CDF 0x00080000
#define CEF 0x00100000
#define CGF 0x00200000
#define CHF 0x00400000
#define CLF 0x00800000
#define CPF 0x01000000
#define CTF 0x02000000
#define CUF 0x04000000
#define CXF 0x08000000
#define CYF 0x10000000
#define CZF 0x20000000





#define FLGCH "abcdfiklnoprstuvwxBDEGHLPTUXYZ"





#define ISLIST(x) (((x) & (RF|WF)) == 0)
#define ISEXTRACT(x) (((x) & (RF|WF)) == RF)
#define ISARCHIVE(x) (((x) & (AF|RF|WF)) == WF)
#define ISAPPND(x) (((x) & (AF|RF|WF)) == (AF|WF))
#define ISCOPY(x) (((x) & (RF|WF)) == (RF|WF))
#define ISWRITE(x) (((x) & (RF|WF)) == WF)





#define BDEXTR (AF|BF|LF|TF|WF|XF|CBF|CHF|CLF|CPF|CXF)
#define BDARCH (CF|KF|LF|NF|PF|RF|CDF|CEF|CYF|CZF)
#define BDCOPY (AF|BF|FF|OF|XF|CBF|CEF)
#define BDLIST (AF|BF|IF|KF|LF|OF|PF|RF|TF|UF|WF|XF|CBF|CDF|CHF|CLF|CPF|CXF|CYF|CZF)
