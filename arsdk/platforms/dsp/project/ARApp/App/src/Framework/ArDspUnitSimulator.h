

#ifndef APP_SRC_ARDSPUNITSIMULATOR_H_
#define APP_SRC_ARDSPUNITSIMULATOR_H_


//#define ArDspUnitSimulator
//#define ArNpuCallbackSimualtor

#ifdef ArDspUnitSimulator
#define  xDebugPrint printf
#endif

typedef struct
{
	AR_U32 u32InputAddress;
	AR_U32 u32InputSize;
	AR_U32 u32OutputAddress;
	AR_U32 u32OutputSize;
	AR_U32 u32GoldenAddress;
	AR_U32 u32GoldenSize;
} Sim_Model_Handle_S;

int xArBareMetalUnitSimulatorWithNpu(void *vDspInputFile, void *vDspOutputFile, void *CjsonFile, void *vDspGoldenFile);

int xArUnitSimulatorLoadModel(Sim_Model_Handle_S *SimHandle,char *InputFile, char *GoldenFile);
int xArUnitSimulatorUnLoadModel(Sim_Model_Handle_S *SimHandle,char *OutputFile);
int xArBareMetalUnitSimulatorWithArm(void *UserDefinedParam, char *OperatorName);



#endif /* APP_SRC_ARDSPUNITSIMULATOR_H_ */
