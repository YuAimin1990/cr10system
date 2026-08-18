
/** \file dwc_ddrphy_phyinit_struct.h 
 *  \brief This file defines the internal data structures used in PhyInit to store user configuration.
 *
 *  Please see \ref docref to obtain necessary information for program variables
 *  correctly for your PHY variant and process technology.
 */

/**  \addtogroup structDef
 *  @{
 */

/** Enumerator for DRAM Type */
typedef enum DramTypes { 
    DDR4,    /*!< DDR4 */
    DDR3,    /*!< DDR3 */
    LPDDR4,  /*!< LPDDR4 */
    LPDDR3,  /*!< LPDDR3 */
    LPDDR2,  /*!< LPDDR2 */
    DDR5     /*!< DDR5 */
} DramType_t;

/** Enumerator for DIMM Type */
typedef enum DimmTypes { 
    UDIMM,   /*!< UDIMM */
    SODIMM,  /*!< SODIMM */
    RDIMM,   /*!< RDIMM (DDR3/DDR4 only) */
    LRDIMM,  /*!< LRDIMM (DDR4 only) */
    NODIMM   /*!< No DIMM (Soldered-on) */
} DimmType_t;


/** \brief Structure for basic (mandatory) user inputs
 *  
 * The following basic data structure must be set and completed correctly so
 * that the PhyInit software package can accurate program PHY registers.
 */
typedef struct user_input_basic {

    DramType_t DramType;    ///< DRAM Module Type: 

                            ///< - must be set as hex
                            ///<
                            ///< Value | Description
                            ///< ----- | ------
                            ///<   0x0 | DDR4
                            ///<   0x1 | DDR3
                            ///<   0x2 | LPDDR4
                            ///<   0x3 | LPDDR3 
                            ///<   0x4 | LPDDR2 
                            
    DimmType_t DimmType;    ///< Choose the Dimm type from one of below: 

                            ///< - must be set as hex
                            ///<
                            ///< Value | Description
                            ///< ----- | ------
                            ///<   0x0 | UDIMM 
                            ///<   0x1 | SODIMM 
                            ///<   0x2 | RDIMM (DDR3/DDR4 only)
                            ///<   0x3 | LRDIMM (DDR4 only)
                            ///<   0x4 | No DIMM (Soldered-on)
                            
    int Lp4xMode;           ///< indicates LPDDR4X mode support. 

                            ///< Obsolete - Unused.

    int NumDbyte;           ///< Enter number of dbytes physically instantiated in RTL
                        
                            ///< - Must be decimal integer.
                            ///< - Must match verilog define DWC_DDRPHY_NUM_DBYTE_*in dwc_ddrphy_*_VDEFINES.v.
                            ///<   For example if DWC_DDRPHY_NUM_DBYTE_8 is defined in RTL dwc_ddrphy_ac12d8ch2_VDEFINES.v, 
                            ///<   this variable must be set to decimal 8.


    int NumActiveDbyteDfi0; ///< Enter number of active dbytes to be controlled by dfi0

                            ///< - See PUB databook section on supported PHY configurations for valid settings.
                            ///< - Must be decimal integer.

    int NumActiveDbyteDfi1; ///< Enter number of active dbytes to be controlled by dfi1

                            ///< - See PUB databook section on supported PHY configurations for valid settings.
                            ///< - Only used for LPDDR3 and LPDDR4
                            ///< - Must be decimal integer.
    
    int NumAnib;            ///< Enter number of ANIBs physically instantiated
    
                            ///< - See PUB databook section on supported PHY configurations for valid settings.
                            ///< - Must be decimal integer.
                            ///< - Must match verilog define DWC_DDRPHY_NUM_ANIB_* in dwc_ddrphy_*_VDEFINES.v
                            ///<   for example if DWC_DDRPHY_NUM_ANIB_12 is defined in RTL dwc_ddrphy_ac12d8ch2_VDEFINES.v
                            ///<   this variable must be set to decimal 12.

    int NumRank_dfi0;       ///< Number of ranks in DFI0 channel
    
                            ///< - See PUB databook section on supported PHY configurations for valid settings.
                            ///< - Must match the number of DRAM ranks attached to the PHY.
                            ///< - Must be decimal integer.

    int NumRank_dfi1;       ///< Number of ranks in DFI1 channel (if DFI1 exists)

                            ///< - See PUB databook section on supported PHY configurations for valid settings.
                            ///< - Only used for LPDDR3 and LPDDR4
                            ///< - Must match the number of DRAM ranks attached to the PHY.
                            ///< - Must be decimal integer.

    int DramDataWidth;      ///< Width of the DRAM device.

                            ///< - Enter 4,8,16 or 32 depending on protocol and dram type according below table.
                            ///< - Must be decimal integer.
                            ///< - See PUB databook section "Supported System Configurations" for DRAM width options 
                            ///<   supported by your PHY product.
                            ///<
                            ///< Protocol | Valid Options | Default
                            ///< -------- | ------------- | ---
                            ///< DDR3     | 4,8,16        | 8 
                            ///< DDR4     | 4,8,16        | 8 
                            ///< LPDDR3   | 16,32         | 16
                            ///< LPDDR4   | 8,16          | 16
                            ///< LPDDR2   | 16,32         | 16
                            ///<
                            ///< \note LPDDR4 x8 width device will be supported in future PHY product.
                            ///< For mixed x8 and x16 width devices, set variable to x8.
                            ///<

    int NumPStates;         ///< Number of p-states used

                            ///< - Must be decimal integer.
    
    int Frequency[4];       ///< Memclk Frequency for each PState.

                            ///< - Must be decimal integer.
                            ///< - Memclk frequency in MHz round up to next highest integer.  Enter 334 for 333.333, etc.
                            ///<
                            ///< [0] - P0 Memclk frequency in MHz \n
                            ///< [1] - P1 Memclk frequency in MHz \n
                            ///< [2] - P2 Memclk frequency in MHz \n
                            ///< [3] - P3 Memclk frequency in MHz \n
    
    int PllBypass[4];       ///< indicates if PLL should be in Bypass mode.

                            ///< - See PUB Databook section "PLL Bypass Mode" under "Clocking and Timing" for requirements.
                            ///< - At datarates below DDR333 rate PLL must be in Bypass Mode.
                            ///< - Must be set as hex.
                            ///<
                            ///< [0] - PLL Bypass Enable for P0 \n
                            ///< [1] - PLL Bypass Enable for P1 \n
                            ///< [2] - PLL Bypass Enable for P2 \n
                            ///< [3] - PLL Bypass Enable for P3 \n
                            ///<
                            ///< Value | Description
                            ///< ----- | ------
                            ///<   0x1 | Enabled
                            ///<   0x0 | Disabled
    
    int DfiFreqRatio[4];    ///< Selected Dfi Frequency ratio

                            ///< - Used to program the DfiFreqRatio register. This register controls how dfi_freq_ratio 
                            ///    input pin should be driven inaccordance with DFI Spec.
                            ///< - See PUB databook section "DfiClk" on detailes on how set value.
                            ///< - must be set as hex
                            ///<
                            ///< [0] - DFI Frequency Ratio for P0 \n
                            ///< [1] - DFI Frequency Ratio for P1 \n
                            ///< [2] - DFI Frequency Ratio for P2 \n
                            ///< [3] - DFI Frequency Ratio for P3 \n
                            ///<
                            ///< Binary Value | Description
                            ///<        ----- | ------
                            ///<        2'b00 | 1:1 DFI Frequency Ratio
                            ///<        2'b01 | 1:2 DFI Frequency Ratio (defualt)
                            ///<        2'b1x | 1:4 DFI Frequency Ratio
    
    int Dfi1Exists;         ///< Indicates whether they PHY configuration has Dfi1 channel 
                           
                            ///< - Must match value of Verilog define DWC_DDRPHY_DFI1_EXISTS in RTL file dwc_ddrphy_*_VDEFINES.v. 
                            ///< - must be set as hex

    int Train2D;            ///< Obsolete, Not used. 

                            ///< - This variable is set via input variable to function dwc_ddrphy_phyinit_sequence()
                            ///< - This exists for backward compatibility and no longer used.

    int HardMacroVer;       ///< Hard Macro Family version in use.

                            ///< - Please see technology specific PHY 
                            ///<   Databook for "Hard Macro Family" version.  The variable is
                            ///<   used to fine tune analog register value settings.
                            ///< - Must be decimal integer.
                            ///<
                            ///< Value | Description
                            ///< ----- | ------
                            ///<   0   | hardmacro family A
                            ///<   1   | hardmacro family B
                            ///<   2   | hardmacro family C
                            ///<   3   | hardmacro family D
                            ///<   4   | hardmacro family E

    int ReadDBIEnable[4];   ///< Obsolete. Not Used. 

                            ///< - Use messageBlock DMIPinPreset instead if DBI is enabled.
                            ///< - This exists for backward compatibility and no longer used.

    int DfiMode;            ///< Obsolete. Not Used. 

                            ///< - DfiMode register is programed based on other parameters
                            ///< - This exists for backward compatibility and no longer used.

} user_input_basic_t;



/** \brief Structure for advanced (optional) user inputs
 *
 *  if user does not enter a value for these parameters, a default recommended or
 *  default value will be used
 */
typedef struct user_input_advanced {

    
    int D4RxPreambleLength[4];   ///< Length of read preamble in DDR4 mode 

                                 ///< - This variable is used to calculate DDR4 MR4-OP[11] set in the meessageBlock.
                                 ///<   The training firmware will set DRAM MR according to MR value
                                 ///<   in the messageBlock at the end of training.
                                 ///< - Set value according to your SI analysis and DRAM requirement.
                                 ///< - Applicable only if DramType == DDR4 
                                 ///< - must be set as hex
                                 ///<
                                 ///< [0] - setting for P0 \n
                                 ///< [1] - setting for P1 \n
                                 ///< [2] - setting for P2 \n
                                 ///< [3] - setting for P3 \n
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///<   0x0 |  1 Tck 
                                 ///<   0x1 |  2 Tck (default)
    
    int D4TxPreambleLength[4];   ///< Length of write preamble in DDR4 mode 

                                 ///< - This variable is used to calculate DDR4 MR4-OP[12] set in the meessageBlock.
                                 ///<   The training firmware will set DRAM MR according to MR value
                                 ///<   in the messageBlock at the end of training.
                                 ///< - Set value according to your SI analysis and DRAM requirement.
                                 ///< - Applicable only if DramType == DDR4
                                 ///< - must be set as hex
                                 ///<
                                 ///< [0] - setting for P0 \n
                                 ///< [1] - setting for P1 \n
                                 ///< [2] - setting for P2 \n
                                 ///< [3] - setting for P3 \n
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///<   0x0 | 1 Tck (default)
                                 ///<   0x1 | 2 Tck
    
    int ExtCalResVal;            ///< External Impedance calibration pull-down resistor value select.

                                 ///< - Indicates value of impedance calibration pull-down resistor connected to BP_ZN pin of the PHY.
                                 ///<   See Section "Impedance Calibrator" section in the PUB Databook for details
                                 ///<   on PHY impedance engine and the ZN requirements for the reference external
                                 ///<   resistor.
                                 ///< - must be set as hex
                                 ///< - Drop-down menu:
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///<   0x0 | 240 ohm (default)
                                 ///<   0x1 | 120 ohm
                                 ///<   0x2 |  40 ohm
    
    int Is2Ttiming[4];           ///< Set to 1 to use 2T timing for address/command, otherwise 1T timing will be used 

                                 ///< - See section "2T Timing and Geardown Support" in PUB Databook. 
                                 ///<   Determine 1T or 2T Timing operation mode based on SI Analysis and DRAM Timing.
                                 ///<   - In 1T mode, CK, CS, CA all have the same nominal timing, ie. ATxDly[6:0] will have same value for all ANIBs
                                 ///<   - In 2T mode, CK, CS,have the same nominal timing (e.g. AtxDly[6:0]=0x00), while CA is delayed by 1UI
                                 ///<     (e.g. ATxDly[6:0]=0x40)
                                 ///< - This values is used to program PhyCfg setting in messageBlock.
                                 ///< - must be set as hex
                                 ///<
                                 ///< [0] - setting for P0 \n
                                 ///< [1] - setting for P1 \n
                                 ///< [2] - setting for P2 \n
                                 ///< [3] - setting for P3 \n
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///<   0x0 | 1T Timing (default)
                                 ///<   0x1 | 2T Timing
    
    int ODTImpedance[4];         ///< Enter desired ODT impedance in Ohm

                                 ///< - Used for programming TxOdtDrvStren registers.
                                 ///< - See "Electrical Parameters" in PHY Databook for supported ODT impedance
                                 ///<   range given your Hard Macro Family.  Set based on Typical values from
                                 ///<   "On-die termination (ODT) programable resistances" in the input conditions 
                                 ///<   table for each protocol. See table foot notes for more details.
                                 ///< - Based on SI Analysis and DRAM modules, select from supported values in the PHY Databook.
                                 ///<   Only values in the PUB databook are supported.
                                 ///< - Enter 0 for open/high-impedance
                                 ///< - Must be decimal integer.
                                 ///< - default:60
                                 ///<
                                 ///< [0] - ODT in Ohm for P0 \n
                                 ///< [1] - ODT in Ohm for P1 \n
                                 ///< [2] - ODT in Ohm for P2 \n
                                 ///< [3] - ODT in Ohm for P3 \n
                                 ///<
    
    int TxImpedance[4];          ///< Tx Drive Impedance for DQ/DQS in ohm 

                                 ///< - Used for programming TxImpedanceCtrl1 registers.
                                 ///< - See "Electrical Parameters" in PHY Databook for supported impedance
                                 ///<   range given your Hard Macro Family.  Set based on Typical values from
                                 ///<   "Outpu drive pull-up/down Impedance: DQ, DQS outputs" in "Common DC Output Paremeters" 
                                 ///<   table for each protocol. See table foot notes for more details.
                                 ///< - Based on SI Analysis and DRAM modules, select from supported values in the PHY Databook.
                                 ///<   Only values in the PUB databook are supported.
                                 ///< - Enter 0 for open/high-impedance
                                 ///< - Must be decimal integer. 
                                 ///< - Default value: 60
                                 ///< - Must be decimal integer.
                                 ///<
                                 ///< [0] - impedance in Ohm for P0 \n
                                 ///< [1] - impedance in Ohm for P1 \n
                                 ///< [2] - impedance in Ohm for P2 \n
                                 ///< [3] - impedance in Ohm for P3 \n
                                 
    int ATxImpedance;            ///< Tx Drive Impedance for AC in ohm 

                                 ///< - Used for programming ATxImpedance register.
                                 ///< - See "Electrical Parameters" in PHY Databook for supported impedance
                                 ///<   range given your Hard Macro Family.  Set based on Typical values from
                                 ///<   "Outpu drive pull-up/down Impedance: address, command, CLK outputs" in "Common DC Output Paremeters" 
                                 ///<   table for each protocol. See table foot notes for more details.
                                 ///< - Based on SI Analysis and DRAM modules, select from supported values in the PHY Databook.
                                 ///<   Only values in the PUB databook are supported.
                                 ///< - Enter 0 for open/high-impedance
                                 ///< - Must be decimal integer. 
                                 ///< - Default value: 20 (HMA,HMB,HMC,HMD), 40 (HME)
                                 ///< - Must be decimal integer.
    
    int MemAlertEn;              ///< Enables BP_ALERT programming of PHY registers.

                                 ///< - Used for programming MemAlertControl and MemAlertControl2 register.
                                 ///< - Program if you require using BP_ALERT pin (to receive or terminate signal) of the PHY  
                                 ///<   otherwise leave at default value, otherwise leave at default value to save power.
                                 ///< - Applicable only if DramType == DDR3 or DDR4
                                 ///< - Must be hex.
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///< 0x0 | Disable BP_ALERT (default)
                                 ///< 0x1 | Enable BP_ALERT pin.
    
    int MemAlertPUImp;           ///< Specify MemAlert Pull-up Termination Impedance

                                 ///< - Programs the pull-up termination on BP_ALERT.
                                 ///< - Only used if user_input_advanced.MemAlertEn==1
                                 ///< - Must be set as hex.
                                 ///< - Choose from the following settings:
                                 ///<
                                 ///< Binary Value | Pull Up Strength
                                 ///<   ----- | ------
                                 ///<    0000 |      No PullUp Strength
                                 ///<    0001 | 240 Ohm PullUp Strength
                                 ///<    0010 | 240 Ohm PullUp Strength
                                 ///<    0011 | 120 Ohm PullUp Strength
                                 ///<    0100 | 120 Ohm PullUp Strength
                                 ///<    0101 |  80 Ohm PullUp Strength
                                 ///<    0110 |  80 Ohm PullUp Strength
                                 ///<    0111 |  60 Ohm PullUp Strength
                                 ///<    1000 | 120 Ohm PullUp Strength
                                 ///<    1001 |  80 Ohm PullUp Strength
                                 ///<    1010 |  80 Ohm PullUp Strength 
                                 ///<    0101 |  80 Ohm PullUp Strength (Default)
                                 ///<    1011 |  60 Ohm PullUp Strength
                                 ///<    1100 |  60 Ohm PullUp Strength
                                 ///<    1101 |  48 Ohm PullUp Strength
                                 ///<    1110 |  48 Ohm PullUp Strength
                                 ///<    1111 |  40 Ohm PullUp Strength
    
    int MemAlertVrefLevel;       ///< Specify the Vref level for BP_ALERT(MemAlert) Receiver

                                 ///< - See register description for MemAlertControl.MALERTVrefLevel in the PUB databook for the formula to set desired
                                 ///<   VREF by :
                                 ///<   \f[ Vref = VDDQ \times (0.51 + MALERTVrefLevel[6:0] \times 0.00345) \f]
                                 ///< - Default VREF = 0.65 * VDDQ --> MemAlertVrefLevel[6:0] = 7'd41 = 0x29
                                 ///< - Must be set as hex.
   
    int MemAlertSyncBypass;      ///< When set, this bit bypasses the DfiClk synchronizer on dfi_alert_n

                                 ///< - See PUB databook register description for MemAlertControl2 for further details.
                                 ///< - Default = 0x0
                                 ///< - Must be set as hex.

    int DisDynAdrTri[4];         ///< Disable Dynamic Per-MEMCLK Address Tristate feature

                                 ///< - See PUB databook section "Dynamic Per-MEMCLK Address Tristate" and register
                                 ///<   description of TristateModeCA.DisDynAdrTri for detailed explanation of this feature.
                                 ///<   Program this variable if you require to disable this feature.
                                 ///<   - In DDR3/2T mode, the dynamic tristate feature should be disabled
                                 ///<     if the controller cannot follow the 2T PHY tristate protocol.
                                 ///<   - In DDR4/2T/2N mode, the dynamic tristate feature should be disabled
                                 ///<     if the controller cannot follow the 2T PHY tristate protocol.
                                 ///<   - In LPDDR4 mode, the dynamic tristate feature should be disabled.
                                 ///< - Must be set as hex.
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///<  0x0  | Enable Dynamic Tristate (default)
                                 ///<  0x1  | Disable Dynamic Tristate 

    int PhyMstrTrainInterval[4]; ///< Specifies the how frequent dfi_phymstr_req is issued by PHY

                                 ///< - See PUB databook section "DFI Master Interface" and DFI 3.1 
                                 ///<   Spec + 4.0v2 Addendum for details of DFI PHY Master interface.
                                 ///< - Based on SI analysis determine how frequent DRAM drift compensation and
                                 ///<   re-training is required. 
                                 ///< - Determine if Memory controller supports DFI PHY Master Interface.
                                 ///< - Program based on desired setting for PPTTrainSetup.PhyMstrTrainInterval register.
                                 ///<   See register description in PUB Databook for translation table
                                 ///<   of values to MEMCLKs.  Example values provided in below table. 
                                 ///< - Only required in LPDDR3 and LPDDR4.
                                 ///< - default : 0xa
                                 ///< - Must be set as hex.
                                 ///< - Example :
                                 ///<
                                 ///< Example Value | Description
                                 ///< ----- | ------
                                 ///<   0x0 | to Disable PHY Master Interface 
                                 ///<   0x3 | PPT Train Interval = 2097152 MEMCLKs 
                                 ///<   0xa | PPT Train Interval = 268435456 MEMCLKs (default)

    int PhyMstrMaxReqToAck[4];   ///< Max time from dfi_phymstr_req asserted to dfi_phymstr_ack asserted

                                 ///< - Based on your Memory controller's(MC) specification determine how long the PHY
                                 ///<   should wait for the assertion of dfi_phymstr_ack once dfi_phymstr_req has 
                                 ///<   been issued by the PHY. If the MC does not ack the PHY's request, PHY may issue 
                                 ///<   dfi_error.
                                 ///< - See PUB databook section "DFI Master Interface" and DFI 3.1 
                                 ///<   Spec + 4.0v2 Addendum for details of DFI PHY Master interface.
                                 ///< - This value will be used to program PPTTrainSetup.PhyMstrMaxReqToAck register.
                                 ///<   See detailed register description in PUB Databook.
                                 ///< - default: 0x5
                                 ///< - Only required in LPDDR3 and LPDDR4.
                                 ///< - Must be set as hex.
                                 ///< - Example:
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///<   0x0 | Disable PHY Master Interface
                                 ///<   0x3 | PPT Max. Req to Ack. = 2048 MEMCLKs
                                 ///<   0x5 | PPT Max. Req to Ack. = 8192 MEMCLKs (default)

    int WDQSExt;                 ///< Enable Write DQS Extension feature of PHY.  
    
                                 ///< - See App Note "DesignWare Cores LPDDR4 MultiPHY : WDQS Extension Application Note"
                                 ///< - Must be set as hex.
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///<   0x0 | Disable Write DQS Extension feature. (default)
                                 ///<   0x1 | Enable Write DQS Extension feature.

    int CalInterval;             ///< Specifies the interval between successive calibrations, in mS.

                                 ///< - See PUB Databook section "Impedance Calibrator" to learn about PHY Impedance 
                                 ///<   calibration FSM. Based on feedback from SI analysis, determine desired 
                                 ///<   calibration interval for your System.  
                                 ///< - Program variable based on desired setting for CalRate.CalInterval register.
                                 ///<   See detailed register description in PUB Databook.
                                 ///< - must be set as hex
                                 ///< - Example values:
                                 ///< 
                                 ///< Value |  Calibration Interval (mS) 
                                 ///<   --- |  ---  
                                 ///<  0x0  | 0 (continuous) 
                                 ///<  0x3  | 1 
                                 ///<  0x4  | 2 
                                 ///<  0x8  | 10 
                           
    int CalOnce;                 ///< This setting changes the behaviour of CalRun register.

                                 ///< - See PUB Databook section "Impedance Calibrator" to learn about PHY Impedance 
                                 ///<   calibration FSM. Based on feedback from SI analysis, determine desired 
                                 ///<   calibration interval for your System.  
                                 ///< - If you desire to manually trigger impedance calibration in mission mode set
                                 ///<   this variable to 1, and toggle CalRun in mission mode. See PUB Databook section
                                 ///<   "Impedance Calibration Engine Setup" and register description for CalRate.CalOnce.
                                 ///< - must be set as hex
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ------
                                 ///< 0x1   | The 0->1 transition of CSR CalRun causes a single iteration of the calibration sequence to occur.
                                 ///< 0x0   | Calibration will proceed at the rate determined by CalInterval. This field should only be changed while the calibrator is idle. ie before csr CalRun is set.
                    
    int DramByteSwap;            ///< DRAM Oscillator count source mapping for skip_training.

                                 ///< - The PHY supports swapping of DRAM oscillator count values between paired DBytes for the purpose of tDQSDQ DRAM Drift Compensation(DDC).
                                 ///< - Each DByte has a register bit to control the source of the oscillator count value used to perform tDQSDQ Drift compensation.  
                                 ///<   On silicon the training firmware will determine the DByte swap and program
                                 ///<   PptCtlStatic register to select oscillator count source.  When skip_train is used, training firmware is skipped thus 
                                 ///<   manual programming may be required depending on configuration.
                                 ///< - The default hardware configuration is for odd Dbyte instance n to use oscillator count values from its paired Dbyte instance n-1.
                                 ///<   So Dbyte1 will use the oscillator count values from Dbyte0, Dbyte3 will use Dbyte2 and so on.  This is required for DRAM Data width =16.
                                 ///< - Each bit of this field corresponds to a DBYTE:
                                 ///<   - bit-0 = setting for DBYTE0
                                 ///<   - bit-1 = setting for DBYTE1
                                 ///<   - bit-2 = setting for DBYTE2
                                 ///<   - . . .
                                 ///<   - bit-n = setting for DBYTEn
                                 ///< - By setting the associated bit for each DByte to 1, PHY will use non-default source for count value.
                                 ///<   - for even Dbytes, non-default source is to use the odd pair count value.
                                 ///<   - for odd Dbytes, no-default source to use data received directly from the DRAM.
                                 ///< - Byte swapping must be the same across different ranks.
                                 ///< - must be set as hex
                                 ///< - default : 0x0
                                 ///< - if Byte mode devices are indicated via the X8Mode messageBlock parameter, this variable is ignored as PHY only supports 
                                 ///<   a limited configuration set based on Byte mode configuration..
                                 ///< 
                                 ///< Example: 
                                 ///<   DramByteSwap = 0x03 - Dbyte0: use count values from Dbyte1, Dbyte1 uses count values received directly received from DRAM.
                                 ///<   Rest of Dbytes have default source for DRAM oscilator count.
                                 ///<
    
    int RxEnBackOff;             ///< Determines the Placement of PHY Read Gate signal

                                 ///< - Only used in LPDDR4 when Lp4RxPreambleMode==0 (static preamble)
                                 ///<   For other DramTypes or LPDDR4-toggling-preamble no options are available and
                                 ///<   PhyInit will set position as required.  See source code in dwc_ddrphy_phyinit_C_initPhyConfig() to see how the RxEnBackOff register is set.
                                 ///< - Synopsys Recomends keeping this setting at default value.  
                                 ///<   SI analysis is required to determine if default value needs to be changed. 
                                 ///< - must be set as hex
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ---  
                                 ///<   0x1 | Position read gate 1UI from the first valid edge of DQS_t (LPDDR4 Static preamble Only) (default)
                                 ///<   0x2 | Position read gate 2UI from the first valid edge of DQS_t (LPDDR4 static preamble Only)

    int TrainSequenceCtrl;       ///< Firmware Training Sequence Control

                                 ///< - This input is used to program SequenceCtrl in messageBlock.
                                 ///<   It controls the training stages executed by firmware.  
                                 ///<   consult the training firmware App Note section "1D Training Steps" for details on training stages.
                                 ///< - For production silicon Synopsys recomends to use default value programmed by PhyInit.
                                 ///< - If running simulation for the first time, program value according to "Initial interactions with the firmware" section in Training firmware App Note.

    int SnpsUmctlOpt;            ///< - Enable Fast Frequency Change (FFC) Optimizations specific to Synopsys UMCTL2 

                                 ///< - Consult Synopsys UMCTL2 documentation to ensure when optimizations can be enabled.
                                 ///< - Only valid for DDR4 RDIMM
                                 ///< - must be set as hex
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ---  
                                 ///< 1 | Enable FFC MRW optimization for FFC. (Only valid for DDR4-RDIMM)
                                 ///< 0 | Disable FFC MRW optimization (default)

    int SnpsUmctlF0RC5x[4];      ///< F0RX5x RCD Control Word when using Fast Frequency Change(FFC) optimizations specific to Synopsys UMCTL2

                                 ///< - When UMCTL2 optimizations are enabled PHY will perform RCD MRW during fast frequency change request.
                                 ///<   The correct RCD control word value for each PState must be programmed in this field.
                                 ///<   Consult the RCD spec and UMCTL documentation to determine the correct value based on DRAM configuration and operating speed.
                                 ///< - Only valid for DramType=DDR4 and DimmType=RDIMM
                                 ///< - Only valid for when SnpsUmctlOpt=1
                                 ///< - must be set as hex
                                 ///<
                                 ///< [0] - F0RC5x for PState 0 \n
                                 ///< [1] - F0RC5x for PState 1 \n
                                 ///< [2] - F0RC5x for PState 2 \n
                                 ///< [3] - F0RC5x for PState 3 \n

    int TxSlewRiseDQ[4];         ///< Pull-up slew rate control for DBYTE Tx
                                 
                                 ///< - Value specified here will be written to register TxSlewRate.TxPreP by PhyInit. 
                                 ///<   See register description for more information.
                                 ///< - Please consult the "Output Slew Rate" section of DQ Hspice App Note in
                                 ///<   specific technology for recommended settings for your technology and Hard Macro Family.
                                 ///<   Protocol specific values are provided under the rise_index column.
                                 ///< - must be set as hex
                                 ///<
                                 ///< [0] - setting for PState 0 \n
                                 ///< [1] - setting for PState 1 \n
                                 ///< [2] - setting for PState 2 \n
                                 ///< [3] - setting for PState 3 \n
                                   
    int TxSlewFallDQ[4];         ///< Pull-down slew rate control for DBYTE Tx
                                 
                                 ///< - Value specified here will be written to TxSlewRate.TxPreN by PhyInit.
                                 ///<   See register description for more information.
                                 ///< - Please consult the "Output Slew Rate" section of DQ HSPICE App Note in 
                                 ///<   specific technology for recommended settings for your technology and Hard Macro Family.
                                 ///< - Protocol specific values are provided under the fall_index column.
                                 ///< - must be set as hex
                                 ///<
                                 ///< [0] - setting for PState 0 \n
                                 ///< [1] - setting for PState 1 \n
                                 ///< [2] - setting for PState 2 \n
                                 ///< [3] - setting for PState 3 \n
                                   
    int TxSlewRiseAC;            ///< Pull-up slew rate control for ANIB Tx
                                 
                                 ///< - Value specified here will be written to ATxSlewRate.ATxPreP 
                                 ///<   See register description for more information.
                                 ///< - Please consult the "Output Slew Rate" section of CA HSPICE Model App Note in  
                                 ///<   specific technology for recommended settings for your technology and Hard Macro Family.
                                 ///< - Protocol specific values are provided under the rise_index column. 
                                 ///< - must be set as hex
                                   
    int TxSlewFallAC;            ///< Pull-down slew rate control for ANIB Tx
                                 
                                 ///< - Value specified here will be written to ATxSlewRate.ATxPreN
                                 ///<   See register description for more information.
                                 ///< - Please consult the "Output Slew Rate" section of CA HSPICE Model App Note in
                                 ///<   specific technology for recommended settings for your technology and Hard Macro Family.
                                 ///< - Protocol specific values are provided under the fall_index column.
                                 ///< - must be set as hex

    int EnableHighClkSkewFix;    ///< Enable alternative PIE program
                                 ///<
                                 ///< See STAR 9001264921 for details on this workaround.  If enabled the PIE reinitializes the 
                                 ///< FIFO pointers a second time due for designs with large skew between chiplet DfiClk 
                                 ///< branches. If enabled PIE latencies in all protocols are increased by 60 DfiClks.
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ---  
                                 ///<   0x1 | Enable
                                 ///<   0x0 | Disable (default)

     int DisableUnusedAddrLns;   ///< Turn off or tristate Address Lanes when possible.
                                 ///<
                                 ///< - When enabled, PHY will tristate unused address lanes to save power when possible 
                                 ///<   by using Acx4AnibDis and AForceTriCont registers. 
                                 ///< - This feature is only implemented for the default PHY Address bump mapping and 
                                 ///<   Ranks must be populated in order. ie Rank1 cannot be used if Rank0 is unpopulated. 
                                 ///< - For alternative bump mapping follow the following guideline to achieve maximum power savings:
                                 ///<   - For each unused BP_A bump program AForceTriCont[4:0] bits based on register description.
                                 ///<   - if all lanes of an Anib are unused _AND_ ANIB is not the first or last instance
                                 ///<     set bit associated with the instance in Acs4AnibDis registers. see register
                                 ///<     description for details.
                                 ///< 
                                 ///< \note This feature is BETA and untested. Future PhyInit version will fully enable
                                 ///< this feature.
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ---  
                                 ///<   0x1 | Enable
                                 ///<   0x0 | Disable (default)

     int PhyInitSequenceNum;     ///< Switches between supported phyinit training sequences
                                 ///<   refer to "Alternative PHY Training sequence" document for further details.
                                 ///<
                                 ///< Value | Description
                                 ///< ----- | ---  
                                 ///<   0x0 | Minimizes number of Imem/Dmem loads (default)
                                 ///<   0x1 | High frequency P1/P2/P3 support (DDR4/LP4 only)

     int RxPBDly[4][2];
     int RxClkDly[4][2];

 } user_input_advanced_t;


/** \brief Structure for user input simulation options (Optional)
 *
 * This structure can be programed with DRAM timing parameters for strict use
 * of the SkipTrain or SkipTrain + DevInit initialization method.  If executing
 * training firmware, this structure is not used.
 *
 */
typedef struct user_input_sim {

    int tDQS2DQ;                 ///< Enter the value of tDQS2DQ for LPDDR4 dram (in ps)

                                 ///< - must be set as decimal integer.
                                 ///< - for simulation only.
                                 
    int tDQSCK;                  ///< Enter the value of tDQSCK in ps
                                 
                                 ///< - must be set as decimal integer.
                                 ///< - for simulation only.

    int tSTAOFF[4];              ///< Enter the value of tSTAOFF in ps
                                 
                                 ///< - Applicable only if DimmType==RDIMM or LRDIMM
                                 ///< - must be set as decimal integer.
                                 ///< - for simulation only.
                                 ///<
                                 ///< [0] - tSTAOFF for P0 \n
                                 ///< [1] - tSTAOFF for P1 \n
                                 ///< [2] - tSTAOFF for P2 \n
                                 ///< [3] - tSTAOFF for P3 \n
                                 
    int tPDM[4];                 ///< Enter the value of tPDM in ps
                                 
                                 ///< - LRDIMM Data Buffer Delay 
                                 ///< - Applicable to LRDIMM only 
                                 ///< - must be set as decimal integer.
                                 ///< - for simulation only.
                                 ///<
                                 ///< [0] - tPDM for P0 \n
                                 ///< [1] - tPDM for P1 \n
                                 ///< [2] - tPDM for P2 \n
                                 ///< [3] - tPDM for P3 \n

} user_input_sim_t;
/** @} */
