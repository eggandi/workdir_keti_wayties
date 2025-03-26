/**
 * @file
 * @brief
 * @date 2024-06-30
 * @author user
 */

#ifndef V2X_SW_LTEV2X_HAL_API_PARAMS_H
#define V2X_SW_LTEV2X_HAL_API_PARAMS_H

// 라이브러리 헤더 파일
#include "ltev2x-hal-types.h"

/**
 * @brief MSDU 송신 파라미터
 */
struct LTEV2XHALMSDURxParams
{
  uint64_t rx_time;                         ///< 수신 시간 (2004/01/01 UTC 기준 마이크로초 단위)
  LTEV2XHALPower rx_power;                  ///< 수신 세기 (dBm)
  LTEV2XHALPriority rx_priority;            ///< 수신 우선순위
  LTEV2XHALL2ID src_l2_id;                  ///< 송신지 L2 ID
  LTEV2XHALL2ID dst_l2_id;                  ///< 목적지 L2 ID
};

/**
 * @brief MSDU 송신 파라미터
 */
struct LTEV2XHALMSDUTxParams
{
  LTEV2XHALTxFlowType tx_flow_type;          ///< 송신 플로우 유형 (0: Ad-Hoc, 1: SPS)
  LTEV2XHALTxFlowIndex tx_flow_index;        ///< 송신 플로우 인덱스 (0 or 1, 송신 유형이 SPS인 경우에만 사용된다)
  LTEV2XHALPriority priority;                ///< 송신 우선순위 (송신 유형이 Ad-Hoc인 경우에만 사용된다)
  LTEV2XHALL2ID dst_l2_id;                   ///< 목적지 L2 ID
  LTEV2XHALPower tx_power;                   ///< 송신 세기 (dBm)
};

/**
 * @brief 송신 플로우 파라미터. 주기적으로 전송되는 메시지에 대한 정보를 등록하는데 사용된다. (SPS)
 */
struct LTEV2XHALTxFlowParams
{
  LTEV2XHALTxFlowIndex index;                ///< 송신 플로우 인덱스 (0 or 1)
  LTEV2XHALTxFlowInterval interval;          ///< 송신 주기 (millisecond 단위)
  LTEV2XHALPriority priority;                ///< 송신 우선순위 (0 ~ 7)
  LTEV2XHALMSDUSize size;                    ///< 송신 페이로드 최대 길이 (상위계층 페이로드 대상 - 예: WSM)
};

/**
 * @brief IP 통신 TxProfile 정보
 */
struct LTEV2XHALTxProfile
{
  LTEV2XHALPower power;
  LTEV2XHALPriority priority;
};

#endif //V2X_SW_LTEV2X_HAL_API_PARAMS_H
