
#include <stdint.h>
#include "dwc_ddrphy_phyinit_struct.h"
/** \file
 * \brief structures and enumeration definitions
 */

//#define IMEM_SIZE 16384
//#define DMEM_SIZE 8192
#define IMEM_SIZE 32768
//#define DMEM_SIZE 4096
#define DMEM_SIZE_1D 1744
#define DMEM_SIZE_2D 1476
#define DMEM_SIZE_DIAG 6888
#define DMEM_ST_ADDR 0x54000

///> Added for multiple PHY interface.
#ifndef DWC_DDRPHY_NUM_PHY 
#define DWC_DDRPHY_NUM_PHY (1)
#endif
//-------------------------------------------------------------
// Defines for SR Firmware Images 
// - point to IMEM incv files, 
// - indicate IMEM size (bytes) 
//-------------------------------------------------------------
/*! \def SR_FW_FILES_LOC
  \brief location of optional retention save restore firmware image.
 */
/*! \def SR_IMEM_SIZE
  \brief max IMEM size of retention save/restore firmware.
 */
/*! \def SR_IMEM_INCV_FILENAME
  \brief file name of retention save/restore IMEM image.
 */

#define SR_IMEM_SIZE 16384

// Message Block Structure Definitions.
#include "mnPmuSramMsgBlock_ddr3.h"

#include "mnPmuSramMsgBlock_ddr4.h"
#include "mnPmuSramMsgBlock_ddr4_2d.h"
#include "mnPmuSramMsgBlock_ddr4_diag.h"


//------------------
// Type definitions
//------------------

/// A structure used to SRAM memory address space.
typedef enum {return_offset, return_lastaddr} return_offset_lastaddr_t;

/// A structure to store the sequence function runtime input variables.
typedef struct runtime_config {
  int Train2D;      ///< train2d input parameter
  int skip_train;   ///< skip_train input parameter
  int debug;        ///< print debug messages
  int RetEn;        ///< Retention Enable input parameter, instructs phyinit to \n
                    ///< issue register reads during initialization to retention registers.
} runtime_config_t;

/// enumeration of instructions for PhyInit Register Interface 
typedef enum {
  startTrack,       ///< start register tracking 
  stopTrack,        ///< stop register tracking 
  saveRegs,         ///< save(read) tracked register values 
  restoreRegs,      ///< restore (write) saved register values 
  dumpRegs,         ///< write register address,value pairs to file
  importRegs        ///< import register address,value pairs to file
} regInstr;

/// data structure to store register address, value pairs
typedef struct Reg_Addr_Val {

  uint32_t Address; ///< register address
  uint16_t Value;   ///< register value 
} __attribute__((packed)) Reg_Addr_Val_t;

/// TargetCSR Target CSR for the impedance value for dwc_ddrphy_phyinit_mapDrvStren()
enum DrvType {
  DrvStrenFSDqP,  ///< 
  DrvStrenFSDqN,  ///< 
  ODTStrenP,      ///< 
  ODTStrenN,      ///< 
  ADrvStrenP,     ///< 
  ADrvStrenN      ///< 
};

//-------------------------------
// Global variables - defined in dwc_ddrphy_phyinit_globals.c
//-------------------------------

extern int ARdPtrInitVal[4];

/*! \def MAX_NUM_RET_REGS
 *  \brief default Max number of retention registers
 *
 * This define is only used by the PhyInit Register interface to define the max
 * amount of registered that can be saved. The user may increase this variable
 * as desired if a larger number of registers need to be restored.
*/
#define MAX_NUM_RET_REGS 600

/**  Array of Address/value pairs used to store register values for the purpose
 * of retention restore.
 */
Reg_Addr_Val_t *RetRegList;
unsigned int *NumRegSaved;    ///< Current Number of registers saved.

#include "dwc_ddrphy_csr_ALL_cdefines.h"
