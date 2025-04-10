#ifndef ASSEMBLY_MIPS_H
#define ASSEMBLY_MIPS_H

#include "globals.h"
#include "cinter.h"  

#define MAX_REGS 64 //numero maximo de registradores

#define REG_OUT  0   // r0 - registrador de saída (OUT)
#define REG_RA   31  // r31 - return address (JAL)
#define REG_ZERO 63  // r63 - sempre contém 0

#endif