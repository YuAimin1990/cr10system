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
. ${PKG_PATH_CUSTOM_CONFIGS}/build_keyops

########################
# INIT
########################


function USAGE()
{
    cat << EOF
Secure Boot Cert Chain Build Script
Outputs cert chains into ${PKG_OUTPUT_DIR_SBCHAINS}

USAGE:  $0 [Secure_Boot_Phase]
where [Secure_Boot_Phase] is the boot phase 0, 1 or 2 to be created

EOF
}

function PKG_SB_CLEANUP
{
    local rc=$1
    local lineno=$2
    local caller_details=$3

    # cleanup here
    # only clean once
    if [[ -n ${CLEAN_ON_EXIT} && ${CLEAN_ON_EXIT} == 1 ]]; then
        echo Cleaning Up...

        if [[ -d "${PKG_OUTPUT_DIR_SBTMP}" ]]; then
            rm -rf "${PKG_OUTPUT_DIR_SBTMP}"
        fi
    fi

    # on error; print stack first trapped signal only
    if [[ -z $_PKG_CLEANUP_ONETIME && $rc != 0 ]]; then
        echo "Exiting Early due to error; Err info follows:"
        echo "[lineno] [func] [source]"
        echo "${caller_details}"
    fi

    _PKG_CLEANUP_ONETIME=1
    CLEAN_ON_EXIT=0
    exit $rc
}

trap 'PKG_SB_CLEANUP $? $LINENO "$(print_caller)"' INT TERM EXIT ERR

BUILD_ARG_SB_PHASE=$1

if [[ -z ${BUILD_ARG_SB_PHASE} \
    || ${BUILD_ARG_SB_PHASE} -lt 0 \
    || ${BUILD_ARG_SB_PHASE} -gt 2 ]]; then
    USAGE
    exit 1
fi


########################
# Pre build activities
# Build SBoot CERT Chains

# get sub cert name for this build phase
eval PKG_SB_CERT_S_BASENAME='$PKG_SB_CERT_S'"${BUILD_ARG_SB_PHASE}"'_BASENAME'

# get entity cert name for this build phase
eval PKG_SB_CERT_E_BASENAME='$PKG_SB_CERT_E'"${BUILD_ARG_SB_PHASE}"'_BASENAME'

# get phase image path for this build phase
eval PKG_SB_PHASE_IMAGE_PATH='$PKG_SB_PHASE'"${BUILD_ARG_SB_PHASE}"'_IMAGE_PATH'

eval PKG_SB_PHASE_IMAGE_ENCRYPT_KEY_PATH='$PKG_SB_PHASE'"${BUILD_ARG_SB_PHASE}"'_IMAGE_ENCRYPT_KEY_PATH'

echo "Setting up Secure Boot Certificate Chain"
mkdir -p ${PKG_OUTPUT_DIR_SBCERTS}
mkdir -p ${PKG_OUTPUT_DIR_SBTMP}

cd ${PKG_OUTPUT_DIR_SBCERTS}

echo "Fetching all certs into $(pwd)"


    # fetch/create all certs
    PKG_GET_CERT            "${PKG_SB_CERT_ROOT_BASENAME}"    "${PKG_SB_CERT_ROOT_EXTRACT_PRIV}"    "${PKG_SB_CRYPTO_KEY_TYPE}"

if [[ ${SB_MODE} == 2 ]]; then
    PKG_GET_CERT            "${PKG_SB_CERT_S0_BASENAME}"      "${PKG_SB_CERT_S0_EXTRACT_PRIV}"      "${PKG_SB_CRYPTO_KEY_TYPE}"
    PKG_GET_CERT            "${PKG_SB_CERT_E0_BASENAME}"      "${PKG_SB_CERT_E0_EXTRACT_PRIV}"      "${PKG_SB_CRYPTO_KEY_TYPE}"

    PKG_GET_CERT            "${PKG_SB_CERT_S1_BASENAME}"      "${PKG_SB_CERT_S1_EXTRACT_PRIV}"      "${PKG_SB_CRYPTO_KEY_TYPE}"
    PKG_GET_CERT            "${PKG_SB_CERT_E1_BASENAME}"      "${PKG_SB_CERT_E1_EXTRACT_PRIV}"      "${PKG_SB_CRYPTO_KEY_TYPE}"

    PKG_GET_CERT            "${PKG_SB_CERT_S2_BASENAME}"      "${PKG_SB_CERT_S2_EXTRACT_PRIV}"      "${PKG_SB_CRYPTO_KEY_TYPE}"
    PKG_GET_CERT            "${PKG_SB_CERT_E2_BASENAME}"      "${PKG_SB_CERT_E2_EXTRACT_PRIV}"      "${PKG_SB_CRYPTO_KEY_TYPE}"

    echo "Transforming certs into SAAP chain into ${PKG_OUTPUT_DIR_SBTMP}"
    # install sb cert configs into sbtmp for each cert in chain
    ln -sf ${PKG_PATH_SB_CERT_CONFIGS}/troot_sb_cert_root.cfg ${PKG_OUTPUT_DIR_SBTMP}/${PKG_SB_CERT_ROOT_BASENAME}.cfg
    ln -sf ${PKG_PATH_SB_CERT_CONFIGS}/troot_sb_cert_sub.cfg ${PKG_OUTPUT_DIR_SBTMP}/${PKG_SB_CERT_S0_BASENAME}.cfg
    ln -sf ${PKG_PATH_SB_CERT_CONFIGS}/troot_sb_cert.cfg ${PKG_OUTPUT_DIR_SBTMP}/${PKG_SB_CERT_E0_BASENAME}.cfg

    ln -sf ${PKG_PATH_SB_CERT_CONFIGS}/troot_sb_cert_sub.cfg ${PKG_OUTPUT_DIR_SBTMP}/${PKG_SB_CERT_S1_BASENAME}.cfg
    ln -sf ${PKG_PATH_SB_CERT_CONFIGS}/troot_sb_cert.cfg ${PKG_OUTPUT_DIR_SBTMP}/${PKG_SB_CERT_E1_BASENAME}.cfg

    ln -sf ${PKG_PATH_SB_CERT_CONFIGS}/troot_sb_cert_sub.cfg ${PKG_OUTPUT_DIR_SBTMP}/${PKG_SB_CERT_S2_BASENAME}.cfg
    ln -sf ${PKG_PATH_SB_CERT_CONFIGS}/troot_sb_cert.cfg ${PKG_OUTPUT_DIR_SBTMP}/${PKG_SB_CERT_E2_BASENAME}.cfg

fi
    cd ${PKG_OUTPUT_DIR_SBTMP}

    # export and transform into synopsys frmts
    PKG_EXPORT_CERTS        ${PKG_OUTPUT_DIR_SBCERTS}       ${PKG_OUTPUT_DIR_SBTMP}
    HEADER_IV_PATH=
    IMAGE_IV_PATH=
    ENCRYPTED_DATA_PATH=${PKG_SB_PHASE_IMAGE_PATH}
if [[ ${SB_MODE} == 2 ]]; then
    PKG_SAAP_XPORTS

    # chain the certs (R<-S0<-S1<-S2)
    PKG_SAAP_XCERT          ${PKG_SB_CERT_S0_BASENAME}      ${PKG_SB_CERT_ROOT_BASENAME}
    PKG_SAAP_XCERT          ${PKG_SB_CERT_S1_BASENAME}      ${PKG_SB_CERT_S0_BASENAME}
    PKG_SAAP_XCERT          ${PKG_SB_CERT_S2_BASENAME}      ${PKG_SB_CERT_S1_BASENAME}

    # Encrypt data image
    if [[ ${PKG_SB_PHASE_IMAGE_ENCRYPT_KEY_PATH} != "" ]]; then
       # Create a header IV.
       HEADER_IV_PATH=${PKG_OUTPUT_DIR_SBCHAINS}/sboot_hdr_iv_${BUILD_ARG_SB_PHASE}.bin
       PKG_GET_IV   ${HEADER_IV_PATH}

       # Encrypt Image
       ENCRYPTED_DATA_PATH=${PKG_OUTPUT_DIR_SBCHAINS}/sboot_encrypted_data_${BUILD_ARG_SB_PHASE}.bin
       IMAGE_IV_PATH=${PKG_OUTPUT_DIR_SBCHAINS}/sboot_image_iv_${BUILD_ARG_SB_PHASE}.bin
       PKG_ENCRYPT_IMAGE   ${PKG_SB_PHASE_IMAGE_PATH} ${PKG_SB_PHASE_IMAGE_ENCRYPT_KEY_PATH} ${IMAGE_IV_PATH} ${ENCRYPTED_DATA_PATH}
    fi
else
    PKG_SB_CERT_E_BASENAME=${PKG_SB_CERT_ROOT_BASENAME}
fi

PKG_SB_PHASE_IMAGE_PATH=${ENCRYPTED_DATA_PATH}

# sign image with phase/stage entity
echo "Signing image $(basename ${PKG_SB_PHASE_IMAGE_PATH}) with ${PKG_SB_CERT_E_BASENAME} cert"
PKG_SIGN_BIN            $PKG_SB_PHASE_IMAGE_PATH        $PKG_SB_CERT_E_BASENAME


echo "Binding Secure Boot cert chain for phase ${BUILD_ARG_SB_PHASE} to the image $(basename ${PKG_SB_PHASE_IMAGE_PATH})"
# create the sb header and updated sb config
PKG_SB_TOOL             ${BUILD_ARG_SB_PHASE} \
                        ${PKG_SB_CERT_ROOT_BASENAME} \
                        ${PKG_SB_PATH_TROOT_CFG} \
                        ${SB_MODE} \
                        ${PKG_SB_PHASE_IMAGE_PATH} \
                        ${PKG_SB_PHASE_IMAGE_ENCRYPT_KEY_PATH} \
                        ${IMAGE_IV_PATH} \
                        ${HEADER_IV_PATH}

if [[ ${SB_MODE} == 1 ]]; then
    cp -f ./MX_hdr.bin ${PKG_OUTPUT_DIR_SBCHAINS}/M1_hdr.bin
fi

if [[ ${SB_MODE} == 2 ]]; then
    # chain this phase's entity to its sub with additional sb header custom data
    PKG_SAAP_XCERT          ${PKG_SB_CERT_E_BASENAME} \
                            ${PKG_SB_CERT_S_BASENAME} \
                            "./MX_hdr.bin"

    echo "Assembling Secure Boot cert chain for phase ${BUILD_ARG_SB_PHASE}"

    # build the sb chain for this phase
    PKG_SB_SAAP_XCHAIN      ${BUILD_ARG_SB_PHASE} \
                            ${PKG_SB_CERT_ROOT_BASENAME} \
                            ${PKG_SB_CERT_E_BASENAME}

    # install cert chain and cfg
    cp -f ./${PKG_SB_CERT_E_BASENAME}_certs.img         ${PKG_OUTPUT_DIR_SBCHAINS}
    cp -f ./$(basename ${PKG_SB_PATH_TROOT_CFG})        ${PKG_OUTPUT_DIR_SBCHAINS}/${PKG_SB_CERT_E_BASENAME}_$(basename ${PKG_SB_PATH_TROOT_CFG})

fi

# only SB PHASE 0 cfg remains as the original basename(PKG_SB_PATH_TROOT_CFG) name
# other cfgs are not used currently since they require dynamic configuration support
if [[ ${BUILD_ARG_SB_PHASE} == 0 ]]; then
    cp -f ./$(basename ${PKG_SB_PATH_TROOT_CFG})    ${PKG_OUTPUT_DIR_SBCHAINS}/
fi


echo "Secure Boot cert chain for phase ${BUILD_ARG_SB_PHASE} complete... Done"
