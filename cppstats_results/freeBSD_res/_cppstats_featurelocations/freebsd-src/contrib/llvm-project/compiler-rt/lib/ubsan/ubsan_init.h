










#if !defined(UBSAN_INIT_H)
#define UBSAN_INIT_H

namespace __ubsan {


const char *GetSanititizerToolName();



void InitAsStandalone();


void InitAsStandaloneIfNecessary();



void InitAsPlugin();

}

#endif
