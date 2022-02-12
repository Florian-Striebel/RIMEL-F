NB_VAR = 0
NB_ARCH = 0
NB_OS = 0
NB_VAR_OS = 0
NB_ARCH_OS = 0
NB_VAR_CONST = 0
#NB_PLATFORM = 0

ARCHS = ["arch64","arch32","arm","amd","i386","mips","powerpc","risc"]
OS = ["linux","win32","win64","windows","macos","mac_os","apple","freebsd"]

def set_of_feature():
    global NB_VAR
    global NB_VAR_OS
    global NB_ARCH_OS
    global NB_VAR_CONST
    fileobj = open('cppstats_featurelocations.csv','r')
    list_of_var = set([])
    for line in fileobj:
        NB_VAR = NB_VAR + 1
        line_str = line.strip().split(",")
        if len(line_str)>5 :
            const_str_list = line_str[5].split(";")
            for i in range(len(const_str_list)):
                list_of_var.add(const_str_list[i])
                NB_VAR_CONST = NB_VAR_CONST +1
                for arch in ARCHS :
                    if arch in const_str_list[i].lower():
                        NB_VAR_OS = NB_VAR_OS + 1
                for os in OS :
                    if os in const_str_list[i].lower():
                        NB_ARCH_OS = NB_ARCH_OS + 1
    fileobj.close()
    return list_of_var

def main():
    global NB_OS
    global NB_ARCH
    features = set_of_feature()
    print("number of variation :",NB_VAR)
    print("number of constant :",len(features))
    for constant in features : 
        for arch in ARCHS:
            if arch in constant.lower():
                NB_ARCH = NB_ARCH + 1
        for os in OS :
            if os in constant.lower():
                NB_OS = NB_OS + 1
    print("number of architecture :",NB_ARCH)
    print("number of os :", NB_OS)
    print("---")
    print("number of constant in variation :",NB_VAR_CONST)
    print("number of os in variation :", NB_VAR_OS)
    print("number of architecture in variation :",NB_ARCH_OS)
main()
