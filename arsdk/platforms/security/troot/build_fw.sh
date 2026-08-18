#!/bin/bash
#-----------------------------------------------------------------------
#
# Copyright 2014 - 2017 Synopsys, Inc.
#
# This Synopsys software and all associated documentation
# are proprietary to Synopsys, Inc. and may only be used pursuant to the
# terms and conditions of a written license agreement with Synopsys,
# Inc. All other use, reproduction, modification, or distribution of the
# Synopsys software or the associated documentation
# is strictly prohibited.
#
#-----------------------------------------------------------------------
########################
# REQ: bash 3.0 or greater
########################


########################
#copy image to sample
########################
cp -f $1 samples/

# Always exit on errors
set -e
# fail on unset vars
#set -u

########################
#INCLUDES
########################
. ./build_lib
. ./.build_fw_init
. ${PKG_PATH_CUSTOM_CONFIGS}/sboot/build_fw_sboot_parms

BUILD_ARG_SAMPLE_TYPE=Secure_Boot

#if [ $# -ne 0 ]; then
#   BUILD_ARG_SAMPLE_TYPE=$1
#fi

if [[ -z $BUILD_ARG_SAMPLE_TYPE ]]; then
    cat << EOF
BUILD FW SCRIPT
This script builds the tRoot Image. Provides selection of sample package
and outputs an encrypted image which can be loaded on the ESM.

USAGE:
    $0 [SAMPLE TYPE]
    where [SAMPLE TYPE] is one of the following:
        * TROOT_H5

Example:
$0 TROOT_H5
EOF
    exit 1
fi

########################
# Cleanup previous builds
########################
if [[ -n "${PKG_PATH_CONFIGS}" ]]; then
    rm -rf ${PKG_PATH_CONFIGS}
    mkdir ${PKG_PATH_CONFIGS}
fi

########################
# Copy the sample files to the correct location
########################

if [ -e ./license/${BUILD_ARG_SAMPLE_TYPE} ]; then
    # install product
    if [ -e ./license/licensekey.lic ]; then
        cp -f ./license/licensekey.lic ${PKG_PATH_CONFIGS}/licensekey.lic
    else
        echo "FAILED: ./licensekey.lic does not exist for configuration ./${BUILD_ARG_SAMPLE_TYPE}"
        exit 1
    fi
    cp -f ./license/${BUILD_ARG_SAMPLE_TYPE}/*  ${PKG_PATH_CONFIGS}/.
elif [ -e ${PKG_PATH_SAMPLES}/${BUILD_ARG_SAMPLE_TYPE} ]; then
    # install selected sample
    cp -f ${PKG_PATH_SAMPLES}/licensekey_sample.lic         ${PKG_PATH_CONFIGS}/licensekey.lic
    cp -f ${PKG_PATH_SAMPLES}/${BUILD_ARG_SAMPLE_TYPE}/*    ${PKG_PATH_CONFIGS}/.
    cp -f ${PKG_PATH_CONFIGS}/certificate_sample.crt        ${PKG_PATH_CONFIGS}/certificate.crt
else
   echo "FAILED: Configuration ${BUILD_ARG_SAMPLE_TYPE} does not exist"
   exit 1
fi

# always install samples
mkdir -p ${PKG_OUTPUT_DIR_SBCERTS}
mkdir -p ${PKG_OUTPUT_DIR_SDCERTS}/data
cp -f ${PKG_PATH_SAMPLES}/sboot_root_sample.*           ${PKG_OUTPUT_DIR_SBCERTS}

cd ${PKG_PATH_TOOLS};

if [ -e ./firmware.rom ]; then
   rm -f ./firmware.rom
fi
if [ -e ./firmware.aic ]; then
   rm -f ./firmware.aic
fi

FIRMWARE_ROM=${PKG_PATH_ROOT}/firmware/firmware.rom
FIRMWARE_AIC=${PKG_PATH_ROOT}/firmware/firmware.aic

ln -sf  $FIRMWARE_ROM ./firmware.rom
ln -sf  $FIRMWARE_AIC ./firmware.aic


#################################
#
# Building secure boot cert chain for phase 0
#
#################################
cd ${PKG_PATH_ROOT}
./build_fw_sboot_certchain.sh 0
if [[ ${SB_MODE} == 2 ]]; then
./build_fw_sboot_certchain.sh 1
./build_fw_sboot_certchain.sh 2
fi
cd -
cp -f ${PKG_OUTPUT_DIR_SBCHAINS}/$(basename ${PKG_SB_PATH_TROOT_CFG}) ${PKG_PATH_CONFIGS}

#################################
#
# Building firmware config
#
#################################
if [ -e ./troot_config_table.bin ]; then
   rm -f ./troot_config_table.bin
fi
echo "Building firmware configuration binary ..."
# Note -s option can be changed but it must match the value in the ./verify_fw.sh script
./troot_base_config_interface \
        -i ${PKG_PATH_CONFIGS}/troot_base_config.cfg \
        -g ${PKG_PATH_CONFIGS}/troot_gpio_mapping.cfg \
        -k ${PKG_PATH_CONFIGS}/troot_hpi_interrupt_mapping.cfg \
        -o ./ \
        -f ./firmware.aic \
        -l ${PKG_PATH_CONFIGS}/licensekey.lic \
        -s 3412d370dd36154c0a228df15534ab26 \
        -t 1 \
        -c ${PKG_PATH_CONFIGS}/certificate.crt

################################
#
# Create the encrypted firmware.le image.
#
################################

if [ -e ${PKG_PATH_ROOT}/firmware/firmware.hex ]; then
    rm -f ${PKG_PATH_ROOT}/firmware/firmware.hex
fi
echo "Generating encrypted firmware images:"
./aictool --format=verilog -o ${PKG_PATH_ROOT}/firmware/firmware.hex -f ./firmware.aic -f ${PKG_PATH_CONFIGS}/licensekey.lic

if [ -e ${PKG_PATH_ROOT}/firmware/firmware.le ]; then
    rm -f ${PKG_PATH_ROOT}/firmware/firmware.le
fi
./aictool --format=binary-le -o ${PKG_PATH_ROOT}/firmware/firmware.le -f ./firmware.aic  -f ./config/licensekey.lic

cd ..
echo " Files for the ESM Operation"
echo "   firmware/firmware.hex"
echo "   firmware/firmware.le"

if [[ ${SB_MODE} == 0 ]]; then
echo "   ${PKG_SB_PHASE0_IMAGE_PATH} image needs to be placed at address location ${PKG_SB_PHASE0_IMAGE_SRC_ADDRESS}"
fi

if [[ ${SB_MODE} == 1 ]]; then
echo "   ${PKG_SB_PHASE0_IMAGE_PATH} image needs to be placed at address location ${PKG_SB_PHASE0_IMAGE_SRC_ADDRESS}"
echo "   tools/sboot_chains/M1_hdr.bin to be copied to address location ${PKG_SB_PHASE0_CERTCHAIN_ADDRESS}"
fi

if [[ ${SB_MODE} == 2 ]]; then
if [[ ${PKG_SB_PHASE0_IMAGE_ENCRYPT_KEY_PATH} == "" ]]; then
echo "   ${PKG_SB_PHASE0_IMAGE_PATH} image needs to be placed at address location ${PKG_SB_PHASE0_IMAGE_SRC_ADDRESS}"
else
echo "   ${PKG_OUTPUT_DIR_SBCHAINS}/sboot_encrypted_data_0.bin image needs to be placed at address location ${PKG_SB_PHASE0_IMAGE_SRC_ADDRESS}"
fi
if [[ ${PKG_SB_PHASE1_IMAGE_ENCRYPT_KEY_PATH} == "" ]]; then
echo "   ${PKG_SB_PHASE1_IMAGE_PATH} image needs to be placed at address location ${PKG_SB_PHASE1_IMAGE_SRC_ADDRESS}"
else
echo "   ${PKG_OUTPUT_DIR_SBCHAINS}/sboot_encrypted_data_1.bin image needs to be placed at address location ${PKG_SB_PHASE1_IMAGE_SRC_ADDRESS}"
fi
if [[ ${PKG_SB_PHASE2_IMAGE_ENCRYPT_KEY_PATH} == "" ]]; then
echo "   ${PKG_SB_PHASE2_IMAGE_PATH} image needs to be placed at address location ${PKG_SB_PHASE2_IMAGE_SRC_ADDRESS}"
else
echo "   ${PKG_OUTPUT_DIR_SBCHAINS}/sboot_encrypted_data_2.bin image needs to be placed at address location ${PKG_SB_PHASE2_IMAGE_SRC_ADDRESS}"
fi
echo "   tools/sboot_chains/${PKG_SB_CERT_E0_BASENAME}_certs.img to be copied to address location ${PKG_SB_PHASE0_CERTCHAIN_ADDRESS}"
echo "   tools/sboot_chains/${PKG_SB_CERT_E1_BASENAME}_certs.img to be copied to address location ${PKG_SB_PHASE1_CERTCHAIN_ADDRESS}"
echo "   tools/sboot_chains/${PKG_SB_CERT_E2_BASENAME}_certs.img to be copied to address location ${PKG_SB_PHASE2_CERTCHAIN_ADDRESS}"
fi

echo "All finished."
