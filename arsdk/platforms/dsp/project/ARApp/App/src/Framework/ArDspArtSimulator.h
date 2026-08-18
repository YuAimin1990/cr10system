

#ifndef APP_SRC_ARDSPARTSIMULATOR_H_
#define APP_SRC_ARDSPARTSIMULATOR_H_

//#define ArDspArtSimulator

#ifdef ArDspArtSimulator
#define  xDebugPrint printf
#endif

#define  SERVERTARGETFLAG  			0x33800000
#define  SERVERTARGETTASKMEMLEN  	0x33801000
#define  SERVERTARGETTASKMEM        0x33801004;
#define  SERVERTARGETRSLTMEMLEN     0x33c02000;
#define  SERVERTARGETRSLTMEM        0x33c02004;

typedef enum
{
	CmdNone 		 = 0,
	CmdHeartBeat	 = 1,
	CmdOpFInfer 	 = 2,
	CmdOpFInferAck	 = 3,
	CmdShutDown 	 = 4,
	CmdShutDownAck	 = 5

} CmdType;


void xArBareMetalArtSimulator();


#endif /* APP_SRC_ARDSPARTSIMULATOR_H_ */
