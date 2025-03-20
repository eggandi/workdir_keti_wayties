/**
 * @file
 * @brief wsm-lte-v2x 어플리케이션 구현 메인 파일
 * @date 2021-02-24
 * @author gyun
 *
 * 더미 WSM(1609.2 미적용)을 송수신하는 테스트 어플리케이션
 */


// 시스템 헤더 파일
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// 어플리케이션 헤더 파일
#include "wsm.h"


/// 어플리케이션 관리정보
struct MIB g_mib;
volatile bool g_loop = true;


/**
 * @brief 어플리케이션 사용법을 출력한다.
 * @param[in] app_filename 어플리케이션 실행파일명
 */
static void WSM_Usage(char app_filename[])
{
  printf("\n\n Description: Sample application to transmit and receive WSMs on LTE-V2X using v2x-sw libraries\n");
  printf(" Version: %s\n", _VERSION_);
  printf(" Author: gyun\n");
  printf(" Email: junghg@keti.re.kr\n");

  printf("\n Usage: %s trx|rx [OPTIONS]\n", app_filename);

  printf("\n OPTIONS for \"TRANSMIT AND RECEIVE\" operation\n");
  printf("  --dev <dev_name>      Set device name to communication. If not specified, set to %s\n", DEFAULT_DEV_NAME);
  printf("  --flow <type>         Set tx flow type - \"sps\" or \"event\". If not specified, set to \"sps\"\n");
  printf("  --psid <psid>         Set PSID(in decimal) to transmit or receive. If not specified, set to %u\n", DEFAULT_PSID);
  printf("  --chan <channel>      Set tx channel number(Not for tx, only for WSM header). If not specified, set to %u\n", DEFAULT_CHAN_NUM);
  printf("  --rate <datarate>     Set tx datarate(in 500kbps)(Not for tx, only for WSM header). If not specified, set to %u(%uMbps)\n", DEFAULT_DATARATE, DEFAULT_DATARATE/2);
  printf("  --power <power>       Set tx power level(in dBm)(For tx and WSM header). If not specified, set to %d\n", DEFAULT_POWER);
  printf("  --prio <priority>     Set tx user priority(0~7)(For tx). If not specified, set to %u\n", DEFAULT_PRIORITY);
  printf("  --len <len>           Set tx WSM body length(in octets). If not specified, set to %u\n", DEFAULT_WSM_BODY_LEN);
  printf("  --interval <usec>     Set tx interval(in usec). If not specified, set to %u(%umsec)\n", DEFAULT_TX_INTERVAL, DEFAULT_TX_INTERVAL/1000);
  printf("  --dbg <level>         Set debug message print level. If not specified, set to %u\n", DEFAULT_DBG);
  printf("                            0: nothing, 1: err, 2: init, 3: event, 4: message hexdump\n");
  printf("  --libdbg <level>      Set v2x libraries debug message print level. If not specified, set to %u\n", DEFAULT_LIB_DBG);
  printf("                            0: nothing, 1: err, 2: init, 3: event, 4: message hexdump\n");

  printf("\n OPTIONS for \"RECEIVE ONLY\" operation\n");
  printf("  --psid <psid>         Set PSID(in decimal) to receive. If not specified, set to %u\n", DEFAULT_PSID);
  printf("  --dbg <level>         Set debug message print level. If not specified, set to %u\n", DEFAULT_DBG);
  printf("                            0: nothing, 1: event, 2: message hexdump\n");
  printf("  --libdbg <level>      Set v2x libraries debug message print level. If not specified, set to %u\n", DEFAULT_LIB_DBG);
  printf("                            0: nothing, 1: err, 2: init, 3: event, 4: message hexdump\n");

  printf("\n Example\n");
  printf("  1) %s trx             Sending periodically and receiving test WSM(psid=%u)\n", app_filename, DEFAULT_PSID);
  printf("  2) %s rx              Receiving all WSMs in any interface/channel/timeslot\n", app_filename);
  printf("\n\n");
}


/**
 * @brief V2X 라이브러리들을 초기화한다.
 * @retval 0: 성공
 * @retval -1: 실패
 */
static int WSM_InitV2XLibs(void)
{
  Log(kDbgMsgLevel_Init, "Initialize V2X libraries\n");

  LTEV2XHALLogLevel hal_log_level = (LTEV2XHALLogLevel ) g_mib.lib_dbg;

  // LTEV2X 접속계층 라이브러리 초기화하고 패킷수신콜백함수를 등록한다.
  int ret = LTEV2XHAL_Init(hal_log_level, g_mib.dev_name);
  if (ret < 0) {
    Log(kDbgMsgLevel_Init, "Fail to initialize ltev2x-hal library - LTEV2XHAL_Init() failed: %d\n", ret);
    return -1;
  }
  LTEV2XHAL_RegisterCallbackProcessMSDU(WSM_ProcessRxMSDUCallback);

  Log(kDbgMsgLevel_Init, "Success to initialize V2X library\n");

  return 0;
}


/**
 * @brief 어플리케이션 종료 시에 호출되는 시그널 함수
 * @param[in] signum 시그널 번호
 *
 * 종료 시에 반드시 LAL_Close()가 호출되어야 한다. (소켓 재사용을 위해)
 */
static void WSM_Terminate(int signum)
{
  (void)signum;
  g_mib.tx_running = false;
  LTEV2XHAL_Close();
  exit(0);
}


/**
 * @brief 어플리케이션 메인 함수
 * @param[in] argc 유틸리티 실행 시 입력되는 명령줄 내 파라미터들의 개수 (유틸리티 실행파일명 포함)
 * @param[in] argv 유틸리티 실행 시 입력되는 명령줄 내 파라미터들의 문자열 집합 (유틸리티 실행파일명 포함)
 * @retval 0: 성공
 * @retval -1: 실패
 */
int main(int argc, char *argv[])
{
  /*
   * 아무 파라미터 없이 실행하면 사용법을 출력한다.
   */
  if (argc < 2) {
    WSM_Usage(argv[0]);
    return 0;
  }

  printf("Running WSM trx on LTE-V2X application..\n");

  /*
   * 입력 파라미터를 파싱하여 저장한다.
   */
  memset(&g_mib, 0, sizeof(g_mib));
  int ret = WSM_ParsingInputParameters(argc, argv);
  if (ret < 0) {
    return -1;
  }

  /*
   * 종료 시에 반드시 LAL_Close()가 호출되어야 하므로, 종료 시그널 핸들러를 등록한다.
   */
  struct sigaction sig_action;
  sig_action.sa_handler = WSM_Terminate;
  sigemptyset(&sig_action.sa_mask);
  sig_action.sa_flags = 0;
  sigaction(SIGINT, &sig_action, NULL);
  sigaction(SIGHUP, &sig_action, NULL);
  sigaction(SIGTERM, &sig_action, NULL);
  sigaction(SIGSEGV, &sig_action, NULL);

  /*
   * V2X 라이브러리들을 초기화한다.
   */
  ret = WSM_InitV2XLibs();
  if (ret < 0) {
    return -1;
  }

  /*
   * 메시지 송신 루틴을 초기화한다.
   */
  if (g_mib.op == kOperationType_trx) {
    ret = WSM_InitTxOperation(g_mib.tx_interval);
    if (ret < 0) {
      goto out;
    }
  }

  /*
   * 무한 루프
   *  - (송신 동작 시) WSM 송신 타이머 처리
   *  - WSM 수신 콜백 처리
   */
  while (g_loop == true) {
    usleep(1000000);
  }

  ret = 0;

out:
  LTEV2XHAL_Close();
  return ret;
}