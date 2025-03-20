/**
 * @file
 * @brief
 * @date 2024-11-29
 * @author wuppu
 */

#include "wsm.h"


/**
 * @brief ASN.1 정보구조체 내 VarLengthNumber(Psid) 필드를 채운다.
 * @param[in] from 메시지 생성을 위한 정보 파라미터 구조체 포인터 (API context)
 * @param[out] to 정보가 채워질 필드의 구조체 포인터 (ASN.1 context)
 * @retval 0: 성공
 * @retval 음수(-Cvcoctci3ResultCode): 실패
 */
int WSM_FillVarLengthNumber(int from, dot3VarLengthNumber *to)
{

  /*
   * Psid 값의 범위에 따라 p-encoded Psid 의 길이를 결정한다.
   */
  uint32_t p_encoded_psid_len;
  if (from <= 127) {
    p_encoded_psid_len = 1;
  }
  else if (from <= 16511) {
    p_encoded_psid_len = 2;
  }
  else if (from <= 2113663) {
    p_encoded_psid_len = 3;
  }
  else if (from <= 270549119) {
    p_encoded_psid_len = 4;
  }
  else {
    Log(kDbgMsgLevel_Err, "Fail to fill dot3VarLengthNumber(Psid) - invalid Psid %d\n", from);
    return -1;
  }

  /*
   * p-encoded Psid 값의 길이에 맞게 VarLengthNumber 정보구조체를 채운다.
   */
  if (p_encoded_psid_len == 1) {
    to->choice = dot3VarLengthNumber_content;
    to->u.content = (int) from;
  }
  else {
    to->choice = dot3VarLengthNumber_extension;
    if (p_encoded_psid_len == 2) {
      to->u.extension.choice = dot3Ext1_content;
      to->u.extension.u.content = (int) from;
    }
    else {
      to->u.extension.choice = dot3Ext1_extension;
      if (p_encoded_psid_len == 3) {
        to->u.extension.u.extension.choice = dot3Ext2_content;
        to->u.extension.u.extension.u.content = (int) from;
      }
      else {
        to->u.extension.u.extension.choice = dot3Ext2_extension;
        to->u.extension.u.extension.u.extension = (int) from;
      }
    }
  }

  return 0;
}


/**
 * @brief VarLengthNumber를 파싱하여 반환한다.
 * @param[in] var_len_num 파싱할 VarLengthNumber 구조체 포인터 (ASN.1 context)
 * @return 파싱된 int형 값
 * @retval 0 또는 양수: 성공
 * @retval 음수: 실패
 */
int WSM_ParseVarLengthNumber(const dot3VarLengthNumber *var_len_num)
{
  int var = 0;
  if (var_len_num->choice == dot3VarLengthNumber_content) {
    var = var_len_num->u.content;
  }
  else if (var_len_num->choice == dot3VarLengthNumber_extension) {
    const struct dot3Ext1 *ext1 = &var_len_num->u.extension;
    if (ext1->choice == dot3Ext1_content) {
      var = ext1->u.content;
    }
    else if (ext1->choice == dot3Ext1_extension) {
      const struct dot3Ext2 *ext2 = &ext1->u.extension;
      if (ext2->choice == dot3Ext2_content) {
        var = ext2->u.content;
      }
      else if (ext2->choice == dot3Ext2_extension) {
        var = ext2->u.extension;
      }
      else {
        Log(kDbgMsgLevel_Err, "Fail to parse VarLengthNumber - invalid ext2 choice %d\n", ext2->choice);
        return -1;
      }
    }
    else {
      Log(kDbgMsgLevel_Err, "Fail to parse VarLengthNumber - invalid ext1 choice %d\n", ext1->choice);
      return -1;
    }
  }
  else {
    Log(kDbgMsgLevel_Err, "Fail to parse VarLengthNumber - invalid var_len_num choice %d\n", var_len_num->choice);
    return -1;
  }
  return var;
}
