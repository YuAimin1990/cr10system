/*-----------------------------------------------------------------------
//
// Copyright 2014 - 2017 Synopsys, Inc.
//
// This Synopsys software and all associated documentation
// are proprietary to Synopsys, Inc. and may only be used pursuant to the
// terms and conditions of a written license agreement with Synopsys,
// Inc. All other use, reproduction, modification, or distribution of the
// Synopsys software or the associated documentation
// is strictly prohibited.
//
//-----------------------------------------------------------------------

---------------- Steps For Generating encrypted ESM firmware for execution  ----------------

1. Modify the build_fw_sboot_parms file with your data:

   SB_MODE = < Secure Boot Mode 0/1/2 >
   PKG_SB_CRYPTO_KEY_TYPE = < Secure Boot Key Type  rsa2048/rsa3072 >
   PKG_SB_PHASEx_IMAGE_PATH=  < Location of Phase x image >
   PKG_SB_PHASEx_IMAGE_SRC_ADDRESS= <Address location of Phase x image >

   // For each phase, the key certificates are optiona.  If hese certificates
   // are not found, they will be generated using a Synopsys Key Generator.
   PKG_SB_CERT_ROOT_BASENAME= < location of the root certificate >
   PKG_SB_CERT_Sx_BASENAME= < name of the signing Key certificate >
   PKG_SB_CERT_Ex_BASENAME= < name of the image signing Key certificate >

2. Edit the troot_base_config.cfg with the correct clock speed.

3. The build_fw.sh script will build the final ESM firmware image using the
   provided default keys and settings.  Simply run

  ./build_fw.sh

  Note:  build_fw.sh script and tools are designed to run on an 32-bit x-86 processor.

  Note:  The script will try to find the license in the main directory first
         and then the sample applications.

  ./build_fw.sh  builds the 2 firmware ESM files (the contents are the same):
    firmware/firmware.le (raw binary format for loading)
    firmware/firmware.hex (verilog $readmemh format for simulation)

  The ./build_fw.sh output indicates the image information to be copied to the correct memory
  locations prior to executing the Secure Boot application.


----------------------------- Additional Information  -------------------------------------

1) Use a 3rd party Key generation and signing tools.

   To use a 3rd party Key generation and signing tools, modify the following
   script:

   build_keyops

   Replace the Synopsys tool implementation with the 3rd party tool calls.


2) Updating the configuration information.

   Execute the troot_base_config_interface to generate a new configuration
   binary file for the firmware.

The troot_base_config_interface requires the following data:
   -i ./config/troot_base_config.cfg           - base location for the configuration files
                                                 Update with configuration data.
   -g ./config/troot_gpio_mapping.cfg          - mapping table for the gpo pins
                                                 Update with configuration data
   -k ./config/troot_hpi_interrupt_mapping.cfg - mapping for the hpi interrupt information
                                                 Update with configuration data
   -f ../firmware/firmware.aic                 - the firmware.aic file (not required to change)
   -l ../samples/licensekey_sample             - sample license key
                                                 Update with license key supplied by Synopsys
   -s 3412d370dd36154c0a228df15534ab26         - seed for encryption (optional to change)
   -c ./config/certificate_sample.txt          - sample configuration certificate
                                                 Update with certificate supplied with the license

3) Logging

Firmware logging can be enabled or disabled and dumped to a file using the Host Library.
To parse the files into a human readable format, use the logviewer provided in the tools
directory. To parsing a log dump:

   ./tools/log_viewer -i <log_dump>

Note: The log_dump file is the binary dump of the external memory.

Outputs the data to STDOUT (to console)

Firmware exception logging can be dumped to a file using the Host Library.
To parse the data into human readable format, use the exception_viewer
provided in the tools directory.  To parse an exception dump:

   ./tools/troot_exception_viewer_interface -i <exception_dump>

Note: The exception_dump file is the binary dump of the external memory.

Outputs the data to STDOUT (to console)
