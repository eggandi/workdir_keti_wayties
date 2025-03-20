/**
 * @file
 * @brief
 * @date 2024-06-30
 * @author user
 */

#ifndef V2X_SW_LTEV2X_HAL_API_H
#define V2X_SW_LTEV2X_HAL_API_H


// 라이브러리 헤더파일
#include "ltev2x-hal-api-params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 라이브러리 및 칩디바이스를 초기화한다.
 * @param[in] log_level 라이브러리 로그메시지 출력 레벨
 * @param[in] dev_name 모듈 통신 다바이스 또는 인터페이스 이름 (ex. /dev/spidev1.1)
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_Init(LTEV2XHALLogLevel log_level, char *dev_name);


/**
 * @brief 라이브러리 및 칩디바이스를 종료한다.
 */
void LTEV2XHAL_Close(void);

/**
 * @brief 송신 플로우를 등록한다.
 * @param[in] flow_params 등록할 송신 플로우 정보 구조체
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_RegisterTransmitFlow(struct LTEV2XHALTxFlowParams flow_params);

/**
 * @brief 송신 플로우를 검색한다.
 * @param[in] index 검색할 송신 플로우 인덱스 (0 or 1)
 * @param[out] flow_params 반환할 저장된 송신 플로우 정보 구조체 포인터
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_RetrieveTransmitFlow(LTEV2XHALTxFlowIndex index, struct LTEV2XHALTxFlowParams *flow_params);

/**
 * @brief 송신 플로우를 삭제한다.
 * @param[in] index 삭제할 송신 플로우 인덱스 (0 or 1)
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_DeleteTransmitFlow(LTEV2XHALTxFlowIndex index);

/**
 * @brief MSDU를 전송한다.
 * @param[in] msdu 전송할 MSDU
 * @param[in] msdu_size 전송할 MSDU 길이
 * @param[in] tx_param 송신 파라미터
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_TransmitMSDU(const uint8_t *msdu, LTEV2XHALMSDUSize msdu_size, struct LTEV2XHALMSDUTxParams tx_params);

/**
 * @brief MSDU 수신을 위한 콜백함수를 등록한다
 * @param[in] ProcessMSDUCallback 콜백함수 포인터
 */
void LTEV2XHAL_RegisterCallbackProcessMSDU(void (*ProcessMSDUCallback)(const uint8_t *msdu, LTEV2XHALMSDUSize msdu_size, struct LTEV2XHALMSDURxParams rx_param));

/**
 * @brief L2 ID를 확인한다.
 * @param[out] l2_id 저장할 L2 ID 포인터
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_GetL2ID(LTEV2XHALL2ID *l2_id);

/**
 * @brief L2 ID를 설정한다.
 * @param[in] l2_id 설정할 L2 ID
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_SetL2ID(LTEV2XHALL2ID l2_id);


/**
 * @brief tx profile을 등록하고 IP 통신을 활성화 한다.
 * @param[in] tx_profile 등록할 tx profile 정보 구조체
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_RegisterTransmitProfile(struct LTEV2XHALTxProfile tx_profile);


/**
 * @brief 등록된 tx profile을 제거하고 IP 통신을 비활성화 한다.
 * @retval 0: 성공
 * @retval 음수(-LTEV2XHALResultCode): 실패
 */
LTEV2XHALResultCode LTEV2XHAL_DeleteTransmitProfile(void);

/**
 * @brief 디바이스에 있는 PPS 카운터를 읽어서 반환한다.
 * @retval 0: 실패
 * @retval 양수: PPS 카운터
 */
unsigned int LTEV2XHAL_GetPPSCounter(void);

#ifdef __cplusplus
}
#endif


#endif //V2X_SW_LTEV2X_HAL_API_H
