/**
 * @file
 * @brief WSM 송신 관련 기능을 구현한 파일
 * @date 2019-08-12
 * @author gyun
 */

// 시스템 헤더 파일
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// 어플리케이션 헤더 파일
#include "wsm.h"


/// 송신타이머
timer_t g_tx_timer;

/// 송신 페이로드 (더미 페이로드)
uint8_t g_payload[kLTEV2XHALMSDUSize_Max];


/**
 * @brief LTE-V2X MSDU를 생성하여 전송한다.
 * @param[in] wsdu 전송할 WSDU (=WSM body에 수납되는 페이로드)
 * @param[in] wsdu_size 전송할 WSDU의 크기
 */
static void WSM_TransmitWSM(const uint8_t *wsdu, size_t wsdu_size)
{
  int ret;
  Log(kDbgMsgLevel_Event, "Transmit WSM\n");

  dot3ShortMsgNpdu *wsm = NULL;
  uint8_t *msdu = NULL;
  asn1_ssize_t msdu_size = 0;

  /*
   * WSM을 생성한다.
   */
  wsm = asn1_mallocz_value(asn1_type_dot3ShortMsgNpdu);
  if (wsm == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to allocate dot3ShortMsgNpdu memory - asn1_mallocz_value() failed\n");
    goto clear;
  }

  /* N-Header */
  wsm->subtype.choice = dot3ShortMsgSubtype_nullNetworking;
  wsm->subtype.u.nullNetworking.version = 3;

  /* N-Header extension */
  wsm->subtype.u.nullNetworking.nExtensions_option = true;
  wsm->subtype.u.nullNetworking.nExtensions.count = WSM_EXT_CNT;
  wsm->subtype.u.nullNetworking.nExtensions.tab = asn1_mallocz(sizeof(dot3ShortMsgNextension) * WSM_EXT_CNT);
  if (wsm->subtype.u.nullNetworking.nExtensions.tab == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to allocate dot3ShortMsgNextension memory - asn1_mallocz() failed\n");
    goto clear;
  }

  dot3ShortMsgNextension *ext = NULL;
  ext = &wsm->subtype.u.nullNetworking.nExtensions.tab[0];
  ext->extensionId = WSM_EXT_ID_TX_POWER;
  ext->value.type = (ASN1CType *) asn1_type_dot3TXpower80211;
  ext->value.u.data = asn1_mallocz_value(ext->value.type);
  if (ext->value.u.data == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to allocate dot3TXpower80211 memory - asn1_mallocz_value() failed\n");
    goto clear;
  }
  dot3TXpower80211 *tx_power = (dot3TXpower80211 *) ext->value.u.data;
  *tx_power = g_mib.tx_power;

  ext = &wsm->subtype.u.nullNetworking.nExtensions.tab[1];
  ext->extensionId = WSM_EXT_ID_CHANNEL;
  ext->value.type = (ASN1CType *) asn1_type_dot3ChannelNumber80211;
  ext->value.u.data = asn1_mallocz_value(ext->value.type);
  if (ext->value.u.data == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to allocate dot3ChannelNumber80211 memory - asn1_mallocz_value() failed\n");
    goto clear;
  }
  dot3ChannelNumber80211 *channel = (dot3ChannelNumber80211 *) ext->value.u.data;
  *channel = (int) g_mib.tx_chan_num;

  ext = &wsm->subtype.u.nullNetworking.nExtensions.tab[2];
  ext->extensionId = WSM_EXT_ID_DATERATE;
  ext->value.type = (ASN1CType *) asn1_type_dot3DataRate80211;
  ext->value.u.data = asn1_mallocz_value(ext->value.type);
  if (ext->value.u.data == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to allocate dot3DataRate80211 memory - asn1_mallocz_value() failed\n");
    goto clear;
  }
  dot3DataRate80211 *datarate = (dot3DataRate80211 *) ext->value.u.data;
  *datarate = (int) g_mib.tx_datarate;

  /* T-Header */
  wsm->transport.choice = dot3ShortMsgTpdus_bcMode;
  ret = WSM_FillVarLengthNumber(g_mib.psid, &wsm->transport.u.bcMode.destAddress);
  if (ret < 0) {
    Log(kDbgMsgLevel_Err, "Fail to fill destAddress(VarLengthNumber) - WSM_FillVarLengthNumber() failed: %d\n", ret);
    goto clear;
  }

  /* Body */
  wsm->body.len = wsdu_size;
  wsm->body.buf = asn1_mallocz(wsdu_size);
  if (wsm->body.buf == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to allocate body memory - asn1_mallocz() failed\n");
    goto clear;
  }
  memcpy(wsm->body.buf, wsdu, wsdu_size);

  msdu_size = asn1_uper_encode(&msdu, asn1_type_dot3ShortMsgNpdu, wsm);
  if (msdu_size < 0 || msdu == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to encode dot3ShortMsgNpdu - asn1_uper_encode() failed: %d\n", msdu_size);
    goto clear;
  }

  struct LTEV2XHALMSDUTxParams tx_params;
  memset(&tx_params, 0x00, sizeof(struct LTEV2XHALMSDUTxParams));
  tx_params.tx_type = g_mib.tx_type;
  tx_params.tx_flow_index = kLTEV2XHALTxFLowIndex_SPS1;
  tx_params.priority = g_mib.tx_priority;
  tx_params.dst_l2_id = kLTEV2XHALL2ID_Broadcast;
  tx_params.tx_power = g_mib.tx_power;
  ret = LTEV2XHAL_TransmitMSDU(msdu, msdu_size, tx_params);
  if (ret < 0) {
    Log(kDbgMsgLevel_Err, "Fail to transmit MSDU - LTEV2XHAL_TransmitMSDU() failed: %d\n", ret);
    goto clear;
  }

  if (g_mib.dbg >= kDbgMsgLevel_Dump) {
    Log(kDbgMsgLevel_Dump, "transmitted_msdu: ");
    for (int i = 0; i < msdu_size; i++) {
      printf("%02X ", msdu[i]);
    }
    printf("\n");
  }

  Log(kDbgMsgLevel_Event, "Success to transmit WSM - msdu_size: %d\n", msdu_size);

  clear:
  if (wsm != NULL) {
    asn1_free_value(asn1_type_dot3ShortMsgNpdu, wsm);
    wsm = NULL;
  }
  if (msdu != NULL) {
    free(msdu);
    msdu = NULL;
  }
}

/**
 * @brief WSM 전송타이머를 제거한다.
 */
static void WSM_ReleaseTxTimer()
{
  Log(kDbgMsgLevel_Event, "Release tx timer\n");
  timer_delete(g_tx_timer);
}

/**
 * @brief WSM 송신타이머 만기 쓰레드. 송신타이머 만기 시마다 호출된다.
 * @param[in] arg 사용되지 않음
 */
static void WSM_TxTimerThread(union sigval arg)
{
  (void)arg;

  if (g_mib.tx_running == false) {
    WSM_ReleaseTxTimer();
  }

  // 페이로드 더미 데이터를 임의의 데이터로 설정한다.
  for (size_t i = 0; i < g_mib.tx_wsm_body_len; i++) {
    g_payload[i] = (uint8_t)i;
  }

  // WSM을 생성하여 전송한다.
  WSM_TransmitWSM(g_payload, g_mib.tx_wsm_body_len);
}


/**
 * @brief WSM 송신타이머를 초기화한다.
 * @param[in] interval 송신주기(usec)
 * @retval 0: 성공
 * @retval -1: 실패
 */
static int WSM_InitTxTimer(unsigned int interval)
{
  int ret;
  struct itimerspec ts;
  struct sigevent se;

  Log(kDbgMsgLevel_Init, "Initialize tx timer - interval: %uusec\n", interval);

  g_mib.tx_running = true;

  /*
   * 송신타이머 만기 시 송신타이머쓰레드(V2X_WSM_TxTimerThread)가 생성되도록 설정한다.
   */
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &g_tx_timer;
  se.sigev_notify_function = WSM_TxTimerThread;
  se.sigev_notify_attributes = NULL;

  ts.it_value.tv_sec = 0;
  ts.it_value.tv_nsec = 1000000;  // 최초타이머 주기 = 1msec
  ts.it_interval.tv_sec = (time_t)(interval / 1000000);
  ts.it_interval.tv_nsec = (long)((interval % 1000000) * 1000);

  /*
   * 송신타이머를 생성한다.
   */
  ret = timer_create(CLOCK_REALTIME, &se, &g_tx_timer);
  if (ret) {
    Log(kDbgMsgLevel_Err, "Fail to create tx timer - timer_create() failed: %m\n");
    return -1;
  }

  /*
   * 송신타이머 주기를 설정한다.
   */
  ret = timer_settime(g_tx_timer, 0, &ts, 0);
  if (ret) {
    Log(kDbgMsgLevel_Err, "Fail to set time for tx timer - timer_settime() failed: %m\n");
    return -1;
  }

  Log(kDbgMsgLevel_Init, "Success to initialize tx timer\n");
  return 0;
}


/**
 * @brief WSM 송신동작을 초기화한다.
 * @param[in] timer_interval 송신타이머 주기(usec 단위)
 * @retval 0: 성공
 * @retval -1: 실패
 */
int WSM_InitTxOperation(unsigned int interval)
{
  Log(kDbgMsgLevel_Init, "Initialize WSM tx operation\n");
  int ret;

  // 송신 플로우를 등록한다.
  if (g_mib.tx_type == kLTEV2XHALTxType_SPS) {
    struct LTEV2XHALTxFlowParams flow_params;
    memset(&flow_params, 0x00, sizeof(struct LTEV2XHALTxFlowParams));
    flow_params.index = kLTEV2XHALTxFLowIndex_SPS1;
    flow_params.interval = g_mib.tx_interval / 1000;
    flow_params.priority = g_mib.tx_priority;
    flow_params.size = g_mib.tx_wsm_body_len + 20; // WSM 헤더 등을 포함해서 대충.

    ret = LTEV2XHAL_RegisterTransmitFlow(flow_params);
    if (ret < 0) {
      Log(kDbgMsgLevel_Init, "Fail to register tx flow - LTEV2XHAL_RegisterTransmitFlow() failed: %d\n", ret);
      return -1;
    }
  }

  // 송신 타이머를 생성한다.
  ret = WSM_InitTxTimer(interval);
  if (ret < 0) {
    return -1;
  }

  Log(kDbgMsgLevel_Init, "Success to initialize tx operation\n");
  return 0;
}
