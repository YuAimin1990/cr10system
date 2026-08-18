README:

Requirements:
1. CEVA-XM6 V18 SDT installation
2. Python 3

Content:
1. Automation_test.py - Automation script
2. AutomationExample folder - CEVA application project which is launched by the Automation_test
3. tcs_config.tscmdb - TCS configuration file which is used by the Automation_test script.
	3.1. System_Cluster_Core.xml         - configuration files refered by the TCS configuration file
	3.2. System_Cluster_Core_memDef.xml  - configuration files refered by the TCS configuration file	
4. Automation.bat - batch file that compile the AutomationExample and than calls Automation_test.py

Usage:
1. Go to folder where Automation_test.py located in the same cmd window
2. Run: Automation.bat

Note! Automation.bat file include a local path for the XM6 tools in line 7.
Please adjust it to your own local machine