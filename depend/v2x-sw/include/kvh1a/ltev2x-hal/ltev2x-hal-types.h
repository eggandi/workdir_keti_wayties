/**
 * @file
 * @brief ltev2x-hal 라이브러리 Open API에서 사용되는 자료/변수 유형을 정의한 헤더 파일
 * @date 2024-06-30
 * @author user
 */

#ifndef V2X_SW_LTEV2X_HAL_TYPES_H
#define V2X_SW_LTEV2X_HAL_TYPES_H


// 시스템 헤더파일
#include <stdint.h>

/**
 * @brief API 실행 결과코드
 */
enum eLTEV2XHALResultCode
{
  kLTEV2XHALResult_Success = 0,               ///< 성공
  kLTEV2XHALResult_DeviceSpecific,            ///< 디바이스에서 에러 리턴
  kLTEV2XHALResult_InvalidSPSIndex,           ///< 유효하지 않은 SPS 인덱스
  kLTEV2XHALResult_InvalidParams,             ///< 유효하지 않은 퍼라미터
  kLTEV2XHALResult_InvalidFamilyID,           ///< 유효하지 않은 Family ID
  kLTEV2XHALResult_InconsistentTxFlow,        ///< 일치하지 않은 송신 플로우
  kLTEV2XHALResult_InconsistentL2ID,          ///< 일치하지 않은 L2 ID
  kLTEV2XHALResult_TxNotReady,                ///< 디바이스의 Tx가 준비되지 않음
  kLTEV2XHALResult_InvalidBufferSize,         ///< 버퍼 사이즈 초과
  kLTEV2XHALResult_NoMemory,                  ///< 메모리 부족
  kLTEV2XHALResult_Unspecified,               ///< 지정되지 않은 에러
  kLTEV2XHALResult_Num,                       ///< 정의된 결과코드 개수
};
typedef int LTEV2XHALResultCode; ///< @ref eLTEV2XHALResultCode


/**
 * @brief 로그메시지 출력 레벨
 *
 * 높은 레벨은 낮은 레벨의 범위를 포함한다.
 * 즉, 로그변수가 높은 레벨로 설정되어 있으면, 그 하위레벨에 해당되는 로그는 함께 출력된다.
 */
enum eLTEV2XHALLogLevel
{
  kLTEV2XHALLogLevel_None = 0, ///< 아무 로그도 출력하지 않는다.
  kLTEV2XHALLogLevel_Err, ///< 에러 로그를 출력한다.
  kLTEV2XHALLogLevel_Init, ///< 초기화 로그를 출력한다.
  kLTEV2XHALLogLevel_Event, ///< 각종 이벤트(패킷 송수신, 설정 등)에 관련된 로그를 출력한다.
  kLTEV2XHALLogLevel_Dump, ///< 패킷 덤프 데이터 로그를 출력한다.
  kLTEV2XHALLogLevel_All, ///< 모든 로그를 출력한다.
  kLTEV2XHALLogLevel_Num, ///< 정의된 로그레벨 개수
  kLTEV2XHALLogLevel_Min = kLTEV2XHALLogLevel_None,
  kLTEV2XHALLogLevel_Max = kLTEV2XHALLogLevel_All,
};
typedef int LTEV2XHALLogLevel; ///< @ref eLTEV2XHALLogLevel

/**
 * @brief SPS 송신 플로우 인덱스
 *
 * Transmit flow는 총 2개이며, 0 또는 1으로 표현된다.
 */
enum eLTEV2XHALTxFlowIndex
{
  kLTEV2XHALTxFLowIndex_SPS1 = 0,
  kLTEV2XHALTxFLowIndex_SPS2 = 1,
  kLTEV2XHALTxFLowIndex_Default = kLTEV2XHALTxFLowIndex_SPS1,
  kLTEV2XHALTxFLowIndex_Min = kLTEV2XHALTxFLowIndex_SPS1,
  kLTEV2XHALTxFLowIndex_Max = kLTEV2XHALTxFLowIndex_SPS2,
};
typedef unsigned int LTEV2XHALTxFlowIndex; ///< @ref eLTEV2XHALTxFlowIndex

/**
 * @brief 송신 플로우 주기 (msec 단위)
 */
enum eLTEV2XHALTxFlowInterval
{
  kLTEV2XHALTxFLowInterval_Default = 100,
  kLTEV2XHALTxFLowInterval_Min = 20,      ///< SECTON에서 최소 20msec까지 지원
  kLTEV2XHALTxFLowInterval_Max = 1000,    ///< SECTON에서 최대 1000msec까지 지원
  kLTEV2XHALTxFLowInterval_None = 0,
};
typedef unsigned int LTEV2XHALTxFlowInterval; ///< @ref eLTEV2XHALTxFlowInterval

/**
 * @brief 송신 우선순위. 값이 작을수록 우선순위가 높다.
 */
enum eLTEV2XHALPriority
{
  kLTEV2XHALPriority_MostUrgent = 0, ///< 가장 급한 우선순위
  kLTEV2XHALPriority_Background = 7, ///< 백그라운드 우선순위
  kLTEV2XHALPriority_Default = 5,
  kLTEV2XHALPriority_Min = kLTEV2XHALPriority_MostUrgent,
  kLTEV2XHALPriority_Max = kLTEV2XHALPriority_Background,
  kLTEV2XHALPriority_None = 0,
};
typedef unsigned int LTEV2XHALPriority; ///< @ref eLTEV2XHALPriority

/**
 * @brief MSDU 메시지 크기
 */
enum eLTEV2XHALMSDUSize
{
  kLTEV2XHALMSDUSize_Default = 4,
  kLTEV2XHALMSDUSize_Min = 4,
  kLTEV2XHALMSDUSize_Max = 4202, ///< MSDU 최대길이 (임의로 지정).
  kLTEV2XHALMSDUSize_None = 0,
};
typedef unsigned int LTEV2XHALMSDUSize; ///< @ref eLTEV2XHALMSDUSize

/**
 * @brief 송신 유형
 */
enum eLTEV2XHALTxType
{
  kLTEV2XHALTxType_Ad_Hoc = 0,
  kLTEV2XHALTxType_SPS = 1,
  kLTEV2XHALTxType_IP = 2,
  kLTEV2XHALTxType_Default = kLTEV2XHALTxType_Ad_Hoc,
  kLTEV2XHALTxType_Min = kLTEV2XHALTxType_Ad_Hoc,
  kLTEV2XHALTxType_Max = kLTEV2XHALTxType_SPS,
};
typedef unsigned int LTEV2XHALTxType; ///< @ref eLTEV2XHALTxType;

/**
 * @brief L2 ID
 */
enum eLTEV2XHALL2ID
{
  kLTEV2XHALL2ID_Broadcast = 0xFFFFFFU,
  kLTEV2XHALL2ID_Invalid = 0xFFFFFFFFU,
  kLTEV2XHALL2ID_Min = 0x000000U,
  kLTEV2XHALL2ID_Max = 0xFFFFFFU,
};
typedef unsigned int LTEV2XHALL2ID; ///< @ref eLTEV2XHALL2ID

/**
 * @brief
 */
enum eLTEV2XHALPower
{
  kLTEV2XHALPower_Min = -128,
  kLTEV2XHALPower_Max = 127,
  kLTEV2XHALPower_Default = 20,
};
typedef int LTEV2XHALPower; ///< @ref eLTEV2XHALPower

enum eLTEV2XHALLatitude
{
  kLTEV2XHALLatitude_Min = -900000000,
  kLTEV2XHALLatitude_Max = 900000000,
  kLTEV2XHALLatitude_Unavailable = 900000001
};
typedef int LTEV2XHALLatitude; ///< @ref eLTEV2XHALLatitude

enum eLTEV2XHALLongitude
{
  kLTEV2XHALLongitude_Min = -1799999999,
  kLTEV2XHALLongitude_Max = 1800000000,
  kLTEV2XHALLongitude_Unavailable = 1800000001
};
typedef int LTEV2XHALLongitude; ///< @ref eLTEV2XHALLongitude

enum eLTEV2XHALElevation
{
  kLTEV2XHALElevation_Min = -4095/*0xF001*/, ///< 최소 고도(-409.5m)
  kLTEV2XHALElevation_Max = 61439/*0xEFFF*/, ///< 최대 고도(6143.9m)
  kLTEV2XHALElevation_Unavailable = -4096/*0xF000*/
};
typedef int LTEV2XHALElevation; ///< @ref eLTEV2XHALElevation

enum eLTEV2XHALSpeed
{
  kLTEV2XHALSpeed_Min = 0,
  kLTEV2XHALSpeed_Max = 999,
};
typedef unsigned int LTEV2XHALSpeed; ///< @ref eLTEV2XHALSpeed

#endif //V2X_SW_LTEV2X_HAL_TYPES_H
