#ifndef    __MB_CB_M_H__
#define    __MB_CB_M_H__
/* ----------------------- Modbus includes ----------------------------------*/
#include "port_m.h"
#include "mbport_m.h"
#include "mbproto_m.h"
#include "mb_m.h"
#include "mbframe_m.h"
#include "mbconfig_m.h"
#include "mbutils_m.h"


/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START        0
#define M_DISCRETE_INPUT_NDISCRETES   8
#define M_COIL_START                  0
#define M_COIL_NCOILS                 8
#define M_REG_INPUT_START             0
#define M_REG_INPUT_NREGS             8
#define M_REG_HOLDING_START           4096
#define M_REG_HOLDING_NREGS           8
/* master mode: holding register's all address */
#define          M_HD_RESERVE                     0
/* master mode: input register's all address */
#define          M_IN_RESERVE                     0
/* master mode: coil's all address */
#define          M_CO_RESERVE                     0
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     0

#endif
