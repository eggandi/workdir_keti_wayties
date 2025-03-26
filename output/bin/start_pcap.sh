#!/bin/bash
# 설정값
APP_NAME="pro_pcap"
ATLKS_BIN_DIR="/usr/bin/atlks"
ATLKS_UPER_DIR="./cv2x-channel"

# APP argument들 --uIP, --rx_ch, --config_path로 인자가 없으면 Config 파일에서 값 찾음 
UDP_IP="127.0.0.1" #기본값 127.0.0.1
V2X_CHANNEL_NUM="183" #기본값 183
CONFIG_FILE_PATH="." # 기본값 ./config/

##PORT는 Config 파일에서 수정 기본값은 12000(NAT-PVT(daemon)), 13000(NAT-PVT), 14000(WCE-V2X)
CONFIG_FILE="pro_config.conf"

#ATLKS UPER 파일 찾을 때 사용
V2X_POWER=""
V2X_BANDWIDTH=""

# 실행
ARGU_UDP_IP=""
ARBU_RX_CHANNEL_NUM=""
ARGU_CONFIG_FILE_PATH=""

if [ -n "$UDP_IP" ]; then
    ARGU_UDP_IP="--uIP ${UDP_IP}"
fi
if [ -n "$CONFIG_FILE_PATH" ]; then
    ARGU_CONFIG_FILE_PATH="--config_path ${CONFIG_FILE_PATH}"
fi
if [ -n "$V2X_CHANNEL_NUM" ]; then
    ARBU_RX_CHANNEL_NUM="--rx_ch ${V2X_CHANNEL_NUM}"
fi

# config 파일 있는지 확인 (없으면 코드에서 자동 생성)
if [ -f "${CONFIG_FILE_PATH}/${CONFIG_FILE}" ]; then
    echo "${CONFIG_FILE_PATH}/${CONFIG_FILE} 파일을 찾았습니다."
else
    ./${APP_NAME} ${ARGU_UPD_IP} ${ARGU_CONFIG_FILE_PATH} ${ARBU_RX_CHANNEL_NUM}
    sleep 1
    killall ${APP_NAME}dd
    echo "${CONFIG_FILE_PATH}/${CONFIG_FILE} 파일을 만들었습니다."
    sleep 3
fi

#  ATLKS UPER 파일 찾기
if [ -z "$V2X_CHANNEL_NUM" ]; then
    v2x_channel_num=$(grep -E "V2X_Rx_Channel_Num=" "${CONFIG_FILE_PATH}/${CONFIG_FILE}" | sed -E 's/.*=([0-9]+);.*/\1/')
else
    v2x_channel_num=$V2X_CHANNEL_NUM
fi
if [ -z "$V2X_POWER" ]; then
    v2x_power=$(grep -E "V2X_Rx_Power=" "${CONFIG_FILE_PATH}/${CONFIG_FILE}" | sed -E 's/.*=([0-9]+);.*/\1/')
else
    v2x_power=$V2X_POWER
fi
if [ -z "$V2X_BANDWIDTH" ]; then
    v2x_bandwidth=$(grep -E "V2X_Rx_Bandwidth=" "${CONFIG_FILE_PATH}/${CONFIG_FILE}" | sed -E 's/.*=([0-9]+);.*/\1/')
else
    v2x_bandwidth=$V2X_BANDWIDTH
fi

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

# ATLKS 바이너리, UPER 링크 생성
ln -sf ${ATLKS_BIN_DIR}/calib_file.bin calib_file.bin
ln -sf $found_files cv2x_rrc_config.uper
ln -sf ${ATLKS_BIN_DIR}/cv2x_sw_config.txt cv2x_sw_config.txt
ln -sf ${ATLKS_BIN_DIR}/dsp_sw_rev3.bin dsp_sw_rev3.bin
ln -sf ${ATLKS_BIN_DIR}/rf_config.bin rf_config.bin
ln -sf ${ATLKS_BIN_DIR}//sw_config_internal.txt sw_config_internal.txt
ln -sf ${ATLKS_BIN_DIR}//sw_config.txt sw_config.txt

./${APP_NAME} ${ARGU_UPD_IP} ${ARGU_CONFIG_FILE_PATH} ${ARBU_RX_CHANNEL_NUM}
