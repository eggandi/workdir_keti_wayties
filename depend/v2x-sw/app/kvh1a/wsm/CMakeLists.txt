
## wsm 유틸리티를 빌드하기 위한 CMakeLists.txt 파일


#
# 빌드 출력 설정
#
set(TARGET_APP wsm)


#
# x64 플랫폼용으로 빌드 시와 단위테스트 빌드 시에는 본 유틸리티를 빌드하지 않는다.
#  - 본 유틸리티는 실제 통신플랫폼에서만 사용된다.
#
if (${TARGET_PLATFORM} STREQUAL "x64" OR BUILD_UNIT_TEST)
  message("[${TARGET_PLATFORM}][${TARGET_APP}] NOT BUILD")
  return()
endif ()


#
# 유틸리티 빌드 - 현재 condor5/condor5v/condor6 플랫폼용으로만 빌드된다 (=LTE-V2X 지원 플랫폼)
#
if (${TARGET_PLATFORM} STREQUAL "condor5" OR ${TARGET_PLATFORM} STREQUAL "condor5v" OR ${TARGET_PLATFORM} STREQUAL "condor6")
  message("[${TARGET_PLATFORM}][${TARGET_APP}] BUILD")
  # 빌드
  add_executable(${TARGET_APP})
  target_include_directories(${TARGET_APP} PRIVATE ${PROJECT_ROOT}/src/apps/depend/include)
  target_include_directories(${TARGET_APP} PRIVATE src)
  target_link_directories(${TARGET_APP} PRIVATE ${PROJECT_ROOT}/src/apps/depend/lib/${TARGET_BUILD})
  target_link_libraries(${TARGET_APP} PRIVATE pthread rt)
  target_link_libraries(${TARGET_APP} PRIVATE lteaccess cv2x-log v2x_radio telux_cv2x telux_qmi telux_common
          qmi_common_so qmiservices qmi_cci qmi_client_helper dsutils
          qmipowermanager qmi_encdec qmi_client_qmux qmi qmiidl)
  target_link_libraries(${TARGET_APP} PRIVATE dot3-2016)
  if (${ASN1_LIB_VENDOR} STREQUAL "ffasn1c")
    target_link_libraries(${TARGET_APP} PRIVATE ffasn1-dot3-2016 ffasn1-base)
  elseif (${ASN1_LIB_VENDOR} STREQUAL "objasn1c")
    target_link_libraries(${TARGET_APP} PRIVATE objasn1-dot3-2016 asn1per asn1oer asn1rt)
  endif ()
  target_link_options(${TARGET_APP} PRIVATE -Wl,--hash-style=gnu -Wl,--as-needed -Wl,--allow-shlib-undefined)
  target_sources(${TARGET_APP} PRIVATE
          src/wsm-lte-v2x.c
          src/wsm-lte-v2x-input-params.c
          src/wsm-lte-v2x-rx.c
          src/wsm-lte-v2x-tx.c)

  # 빌드 결과물을 output 디렉토리로 복사
  set(OUTPUT_DIR ${PROJECT_ROOT}/src/apps/output/${TARGET_BUILD})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          remove ${OUTPUT_DIR}/${TARGET_APP})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          remove ${OUTPUT_DIR}/${TARGET_APP}-${VERSION})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          copy ${TARGET_APP} ${OUTPUT_DIR}/${TARGET_APP})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          copy ${TARGET_APP} ${OUTPUT_DIR}/${TARGET_APP}-${VERSION})
elseif (${TARGET_PLATFORM} STREQUAL "kvh1a")
  message("[${TARGET_PLATFORM}][${TARGET_APP}] BUILD")
  # 빌드
  add_executable(${TARGET_APP})
  target_include_directories(${TARGET_APP} PRIVATE ${PROJECT_ROOT}/src/apps/depend/include)
  target_include_directories(${TARGET_APP} PRIVATE src)
  target_link_directories(${TARGET_APP} PRIVATE ${PROJECT_ROOT}/src/apps/depend/lib/${TARGET_BUILD})
  target_link_libraries(${TARGET_APP} PRIVATE pthread rt)
  #  target_link_libraries(${TARGET_APP} PRIVATE dot3-2016)
  target_link_libraries(${TARGET_APP} PRIVATE ltev2x-hal atlkremote_linux_u atlkcv2x atlkcli atlkpoti atlksmx atlktest)
  if (${ASN1_LIB_VENDOR} STREQUAL "ffasn1c")
    target_link_libraries(${TARGET_APP} PRIVATE ffasn1-dot3-2016 ffasn1-base)
  elseif (${ASN1_LIB_VENDOR} STREQUAL "objasn1c")
    target_link_libraries(${TARGET_APP} PRIVATE objasn1-dot3-2016 asn1per asn1oer asn1rt)
  endif ()
  target_link_options(${TARGET_APP} PRIVATE -Wl,--hash-style=gnu -Wl,--as-needed)
  target_sources(${TARGET_APP} PRIVATE
          src/wsm.c
          src/wsm-input-params.c
          src/wsm-rx.c
          src/wsm-tx.c
          src/wsm-log.c
          src/wsm-asn.c
  )

  # 빌드 결과물을 output 디렉토리로 복사
  set(OUTPUT_DIR ${PROJECT_ROOT}/src/apps/output/${TARGET_BUILD})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          remove ${OUTPUT_DIR}/${TARGET_APP})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          remove ${OUTPUT_DIR}/${TARGET_APP}-${VERSION})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          copy ${TARGET_APP} ${OUTPUT_DIR}/${TARGET_APP})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          copy ${TARGET_APP} ${OUTPUT_DIR}/${TARGET_APP}-${VERSION})
  add_custom_command(TARGET ${TARGET_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
          remove ${TARGET_APP})
else ()
  message("[${TARGET_PLATFORM}][${TARGET_APP}] NOT BUILD")
endif ()
