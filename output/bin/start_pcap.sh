#!/bin/bash
APP_NAME="pro_pcap"
CONFIG_FILE_PATH="./config"
CONFIG_FILE="pro_config.conf"

ATLKS_BIN_DIR="/usr/bin/atlks"
ATLKS_UPER_DIR="./cv2x-channel"

v2x_channel_num=$(grep -E "V2X_Rx_Channel_Num=" "${CONFIG_FILE_PATH}/${CONFIG_FILE}" | sed -E 's/.*=([0-9]+);.*/\1/')
v2x_power=$(grep -E "V2X_Rx_Power=" "${CONFIG_FILE_PATH}/${CONFIG_FILE}" | sed -E 's/.*=([0-9]+);.*/\1/')
v2x_bandwidth=$(grep -E "V2X_Rx_Bandwidth=" "${CONFIG_FILE_PATH}/${CONFIG_FILE}" | sed -E 's/.*=([0-9]+);.*/\1/')


if [ -z "$v2x_channel_num" ]; then
    echo "config.conf에서 V2X_Rx_Channel_Num 값을 찾지 못했습니다."
    exit 1
fi

echo "Rx Channel : $v2x_channel_num" ", " "Rx Power : $v2x_power" ", " "Rx Bandwidth : $v2x_bandwidth"

found_files=$(find ${ATLKS_UPER_DIR}/ \( \( -name "*${v2x_channel_num}*" -a -name "*${v2x_power}*"  \) -a -name "*${v2x_bandwidth}*" \)  -a ! -name "*.uper.*" -a -type f)

if [ -n "$found_files" ]; then
    echo "$found_files"
else
    echo "파일을 찾지 못했습니다."
fi

ln -sf /usr/bin/atlks/calib_file.bin calib_file.bin
ln -sf $found_files cv2x_rrc_config.uper
ln -sf /usr/bin/atlks/cv2x_sw_config.txt cv2x_sw_config.txt
ln -sf /usr/bin/atlks/dsp_sw_rev3.bin dsp_sw_rev3.bin
ln -sf /usr/bin/atlks/rf_config.bin rf_config.bin
ln -sf /usr/bin/atlks/sw_config_internal.txt sw_config_internal.txt
ln -sf /usr/bin/atlks/sw_config.txt sw_config.txt

./${APP_NAME}