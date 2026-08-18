/** \file 
 * \brief defines _PMU_SMB_DIAG data structure
 */

/**  \brief DIAG firmware message block structure 
 *
 *  Please refer to the Training Firmware App Note for futher information about
 *  the usage for Message Block.
 */
typedef struct _PMU_SMB_DIAG_t {
	uint8_t DiagTestNum;		// Byte offset 0x400, CSR Addr 0x54200, Direction=N/
	uint8_t DiagSubTest;		// Byte offset 0x401, CSR Addr 0x54200, Direction=N/A
	uint8_t DiagPrbs;		// Byte offset 0x402, CSR Addr 0x54201, Direction=N/A
	uint8_t DiagRank;		// Byte offset 0x403, CSR Addr 0x54201, Direction=N/A
	uint8_t DiagChannel;		// Byte offset 0x404, CSR Addr 0x54202, Direction=N/A
	uint8_t DiagRepeatCount;	// Byte offset 0x405, CSR Addr 0x54202, Direction=N/A
	uint8_t DiagLoopCount;		// Byte offset 0x406, CSR Addr 0x54203, Direction=N/A
	uint8_t DiagByte;		// Byte offset 0x407, CSR Addr 0x54203, Direction=N/A
	uint8_t DiagLane;		// Byte offset 0x408, CSR Addr 0x54204, Direction=N/A
	uint8_t DiagVrefInc;		// Byte offset 0x409, CSR Addr 0x54204, Direction=N/A
	uint8_t DiagReserved0A;		// Byte offset 0x40a, CSR Addr 0x54205, Direction=N/A
	uint8_t DiagXCount;		// Byte offset 0x40b, CSR Addr 0x54205, Direction=N/A
	uint16_t DiagAddrLow;		// Byte offset 0x40c, CSR Addr 0x54206, Direction=N/A
	uint16_t DiagAddrHigh;		// Byte offset 0x40e, CSR Addr 0x54207, Direction=N/A
	uint16_t DiagPatternLow;	// Byte offset 0x410, CSR Addr 0x54208, Direction=N/A
	uint16_t DiagPatternHigh;	// Byte offset 0x412, CSR Addr 0x54209, Direction=N/A
	uint8_t DiagMisc0;		// Byte offset 0x414, CSR Addr 0x5420a, Direction=N/A
	uint8_t DiagReserved15;		// Byte offset 0x415, CSR Addr 0x5420a, Direction=N/A
	uint16_t DiagReturnData;	// Byte offset 0x416, CSR Addr 0x5420b, Direction=N/A
					// The result of the test chosen is returned starting
					// at this location. The first 2 bytes of return data
					// are at 0x5420b; the next 2 bytes are at 0x5420c, etc;
					// please see individual tests for details of the
					// data structure.
} __attribute__ ((packed)) PMU_SMB_DIAG_t;
