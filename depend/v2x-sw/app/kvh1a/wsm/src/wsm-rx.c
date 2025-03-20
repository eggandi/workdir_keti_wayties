/**
 * @file
 * @brief WSM 수신 처리 기능을 구현한 파일
 * @date 2021-02-24
 * @author gyun
 */


// 시스템 헤더 파일
#include <stdio.h>

// 어플리케이션 헤더 파일
#include "wsm.h"


/**
 * @brief LTE-V2X MSDU 수신처리 콜백함수. lteaccess 라이브러리에서 호출된다.
 * @param[in] msdu 수신된 MSDU (= WSM 헤더 + WSM body)
 * @param[in] msdu_size 수신된 MSDU의 크기
 */
void WSM_ProcessRxMSDUCallback(const uint8_t *msdu, LTEV2XHALMSDUSize msdu_size, struct LTEV2XHALMSDURxParams rx_params)
{
  Log(kDbgMsgLevel_Event, "Process rx MSDU - msdu_size: %zu\n", msdu_size);

  ASN1Error err;
  dot3ShortMsgNpdu *wsm = NULL;
  asn1_ssize_t decoded_size = 0;

  int power = 999;
  int datarate = 999;
  int channel = 999;

  // MSDU를 디코딩한다.
  decoded_size = asn1_uper_decode((void **) &wsm, asn1_type_dot3ShortMsgNpdu, msdu, msdu_size, &err);
  if (decoded_size < 0 || wsm == NULL) {
    Log(kDbgMsgLevel_Err, "Fail to decode dot3ShortMsgNpdu - asn1_uper_decode() failed: %d\n", decoded_size);
    goto clear;
  }
  if (wsm->subtype.u.nullNetworking.nExtensions_option == true) {
    for (unsigned int i = 0; i < wsm->subtype.u.nullNetworking.nExtensions.count; i++) {
      switch (wsm->subtype.u.nullNetworking.nExtensions.tab[i].extensionId) {
        case WSM_EXT_ID_CHANNEL:
          channel = *((int *) wsm->subtype.u.nullNetworking.nExtensions.tab[i].value.u.data);
          break;
        case WSM_EXT_ID_TX_POWER:
          power = *((int *) wsm->subtype.u.nullNetworking.nExtensions.tab[i].value.u.data);
          break;
        case WSM_EXT_ID_DATERATE:
          datarate = *((int *) wsm->subtype.u.nullNetworking.nExtensions.tab[i].value.u.data);
          break;
      }
    }
  }

  unsigned int rx_psid = (unsigned int) WSM_ParseVarLengthNumber(&wsm->transport.u.bcMode.destAddress);

  // 수신할 PSID인지 확인한다.
  if (rx_psid != g_mib.psid) {
    Log(kDbgMsgLevel_Err, "Not process rx MSDU - not interested PSID %d\n", rx_psid);
    return;
  }

  // 수신 파라미터 출력
  Log(kDbgMsgLevel_Event, "rx_params: \n");
  Log(kDbgMsgLevel_Event, "  rx_time: %"PRIu64", rx_power: %ddBm, rx_priority: %d, src_l2_id: %08X, dst_l2_id: %08X\n",
      rx_params.rx_time,
      rx_params.rx_power,
      rx_params.rx_priority,
      rx_params.src_l2_id,
      rx_params.dst_l2_id);

  // WSM 데이터 출력
  Log(kDbgMsgLevel_Event, "wsm_params: \n");
  Log(kDbgMsgLevel_Event, "  psid: %u, chan: %u, datarate: %u*500kbps, power: %ddBm\n",
      rx_psid,
      channel,
      datarate,
      power);

  if (g_mib.dbg >= kDbgMsgLevel_Dump) {
    Log(kDbgMsgLevel_Dump, "received_msdu: ");
    for (unsigned int i = 0; i < msdu_size; i++) {
      printf("%02X ", msdu[i]);
    }
    printf("\n");
  }

  clear:
  if (wsm != NULL) {
    asn1_free_value(asn1_type_dot3ShortMsgNpdu, wsm);
    wsm = NULL;
  }
}
