#ifndef HLDREAD_H
#define HLDREAD_H

#include "evt.h"

#ifdef __cplusplus
extern "C" {
#endif

int startAnalysis(int argc, char **argv);
int analyseEvt(void *evt);
int stopAnalysis(void);

#ifdef __cplusplus
}
#endif

#endif
