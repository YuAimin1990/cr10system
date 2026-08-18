
#ifndef _MEMORYATTRIBUTION_H_
#define _MEMORYATTRIBUTION_H_

#include "RegisterAddr.h"
#include "RegisterContent.h"



extern int xConfigRegionCacheable(unsigned int addr, unsigned int region_index);
extern unsigned int xGetRegionStartAddr(const unsigned int region_index);
extern int xConfigRegionNonCacheable(unsigned int addr, unsigned int region_index);
extern int xConfigRegionProgramCacheable(unsigned int addr, unsigned int region_index);

void xConfigMssAccessProtectionEnable(unsigned int program_access_protection, unsigned int data_access_protection);

/*    AP/MODE:     Supervisor       User0         User1
 *     000          r/w				r/w				r/w
 *     001			r/w				r/w				r
 *     010			r/w				r				r
 *     011			r/w				r/w				no access
 *     100			r/w				no access		no access
 *     101			r				r				r
 *     110			r				no access		no access
 *     111			no access		no access		no access
*/
/* @param addr : access protection start phy address
 * @param region index : program access protection region index [0 - 15], index 0 form addr 0
 * @param active : region is active, 0:active, 1:inactive
 * @param ap_value : access protection operation mode 000 - 111
 * @param is_global_pcache : is enable global pragram cache
*/
extern void xConfigRegionProgramAccessProtection(unsigned int addr, int active, unsigned int ap_value, unsigned int region_index, unsigned int is_global_pcache);

/* @param addr : access protection start phy address
 * @param region index : data access protection region index [0 - 31], index 0 form addr 0
 * @param active : region is active, 0:active, 1:inactive
 * @param ap_value : access protection operation mode
 * @param is_global_dcache : is enable global data cache
*/
extern void xConfigRegionDataAccessProtection(unsigned int addr, int active, unsigned int ap_value, unsigned int region_index, unsigned int is_global_dcache);

/* @param stack_start_addr : defined the stack violation start address
 * @param stack_end_addr : defined the stack violation end address
 * @note : stack_end_addr<=stack access<=stack_start_addr
*/
extern void xConfigStackAccessVioltationRegion(unsigned int stack_start_addr, unsigned int stack_end_addr);
#endif // _MEMORY_ATTRIBUTION_H_

