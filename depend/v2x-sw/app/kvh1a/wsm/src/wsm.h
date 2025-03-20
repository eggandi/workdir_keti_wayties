/**
 * @file
 * @brief wsm-lte-v2x 어플리케이션 메인 헤더파일
 * @date 2021-02-24
 * @author gyun
 */

#ifndef V2X_WSM_LTE_V2X_H
#define V2X_WSM_LTE_V2X_H


// 시스템 헤더 파일
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

// 라이브러리 헤더 파일
#include "ltev2x-hal/ltev2x-hal.h"
#include "ffasn1c/ffasn1-dot3-2016.h"


/*
 * 입력 파라미터 기본값
 */
#define DEFAULT_DEV_NAME "/dev/spidev1.1"
#define DEFAULT_TX_FLOW_TYPE 1 // SPS
#define DEFAULT_PSID 32
#define DEFAULT_CHAN_NUM 178
#define DEFAULT_POWER 10
#define DEFAULT_DATARATE 12
#define DEFAULT_PRIORITY 7
#define DEFAULT_WSM_BODY_LEN 100
#define DEFAULT_TX_INTERVAL 100000
#define DEFAULT_DBG 1
#define DEFAULT_LIB_DBG 1

#define WSM_EXT_CNT 3
#define WSM_EXT_ID_TX_POWER 4
#define WSM_EXT_ID_CHANNEL 15
#define WSM_EXT_ID_DATERATE 16

/*
 * 로그출력 매크로
 * 컴파일 시 "_DEBUG_*_"가 정의되지 않으면 로그출력 코드가 제거되어 컴파일된다.
 */
#if defined(_DEBUG_STDOUT_) || defined(_DEBUG_SYSLOG_)
/// 로그 출력 매크로
#define Log(l, f, a...) do { if (g_mib.dbg >= l) { WSM_PrintLog(__FUNCTION__, f, ## a); } } while(0)
/// 에러레벨 로그 출력 매크로
#define Err(f, a ...) do { if (g_mib.dbg >= kDbgMsgLevel_err) { WSM_PrintLog(__FUNCTION__,f,## a); } } while(0)
#else
#define Log(l, f, a ...) do {} while(0)
#define Err(f, a ...) do {} while(0)
#endif


/**
 * @brief 어플리케이션 동작 유형
 */
enum eOperationType
{
  kOperationType_rx_only, ///< 수신동작만 수행
  kOperationType_trx, ///< 송수신동작 수행
  kOperationType_max = kOperationType_trx
};
typedef unsigned int Operation; ///< @ref eOperationType


/**
 * @brief 로그메시지 출력 레벨
 */
enum eDbgMsgLevel
{
  kDbgMsgLevel_None, ///< 미출력
  kDbgMsgLevel_Err, ///< 에러
  kDbgMsgLevel_Init, ///< 에러
  kDbgMsgLevel_Event, ///< 이벤트 출력
  kDbgMsgLevel_Dump, ///< 메시지 hexdump 출력
  kDbgMsgLevel_max = kDbgMsgLevel_Dump
};
typedef unsigned int DbgMsgLevel; ///< @ref eDbgMsgLevel


/**
 * @brief 어플리케이션 관리정보
 */
struct MIB
{
  Operation op; ///< 어플리케이션 동작 유형
  char dev_name[256]; ///< LTE-V2X 통신 디바이스 이름
  LTEV2XHALTxType tx_type; ///< 송신 유형
  unsigned int psid; ///< 송신 또는 수신하고자 하는 PSID
  DbgMsgLevel dbg; ///< 디버그 메시지 출력 레벨
  unsigned int lib_dbg; ///< V2X 라이브러리 디버그 메시지 출력 레벨
  unsigned int tx_chan_num; ///< WSM 송신 채널번호 (실제 전송에는 사용되지 않고 WSM 확장 헤더에 수납되는 용도로만 사용)
  unsigned int tx_datarate; ///< WSM 송신 데이터레이트 (실제 전송에는 사용되지 않고 WSM 확장 헤더에 수납되는 용도로만 사용)
  LTEV2XHALPower tx_power; ///< WSM 송신 파워 (실제 전송에도 사용되며, WSM 확장 헤더에도 수납된다)
  LTEV2XHALPriority tx_priority; ///< WSM 송신에 사용되는 우선순위
  LTEV2XHALMSDUSize tx_wsm_body_len; ///< 송신 WSM body 의 길이
  unsigned int tx_interval; ///< 송신 주기 (usec 단위)

  bool tx_running;
};


/*
 * 프로그램에서 사용되는 전역 변수 및 함수
 */
extern struct MIB g_mib;
void WSM_PrintLog(const char *func, const char *format, ...);
void WSM_LTE_V2X_Print(const char *func, const char *format, ...);
int WSM_ParsingInputParameters(int argc, char *argv[]);
void WSM_ProcessRxMSDUCallback(const uint8_t *msdu, LTEV2XHALMSDUSize msdu_size, struct LTEV2XHALMSDURxParams rx_params);
int WSM_InitTxOperation(unsigned int interval);

/* wsm-asn.c */
int WSM_FillVarLengthNumber(int from, dot3VarLengthNumber *to);
int WSM_ParseVarLengthNumber(const dot3VarLengthNumber *var_len_num);

#endif //V2X_WSM_LTE_V2X_H
