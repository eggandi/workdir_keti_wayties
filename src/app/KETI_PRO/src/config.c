#include "config.h"
#include "utils.h"

struct pro_config_t G_pro_config;
static void PRO_Initial_Setup_Configuration_Value_Input(char type, char *value, size_t value_len, void* out_value);
/**
 * @brief config 파일을 읽어서 V2X 핸드오버 장치의 초기설정 
 * 
 * @param  
 * @return int 0 | error code
 */
extern int PRO_Initial_Setup_Configuration_Read(struct pro_config_t *pro_config)
{
    int ret;
    // config 경로가 존재하는지 확인
    ret = access(PRO_INITAIL_SETUP_CONFIGURAION_FILE_PATH, F_OK);
    if(ret < 0)
    {
        char sys_cmd[256] = {0}; 
        // mkdir 명령어에 대한 결과값 체크
        snprintf(sys_cmd, sizeof(sys_cmd), "mkdir -m 777 -p %s", PRO_INITAIL_SETUP_CONFIGURAION_FILE_PATH);
        ret = system(sys_cmd);
        if(ret != 0)
        {
            printf("Failed to create config directory\n");
            return -1;
        }
    }
    
    // config 파일 경로 생성 (경로 + "/" + 파일명 + 널 종료 문자)
    char *config_file_name = PRO_INITAIL_SETUP_CONFIGURAION_FILE_NAME;
    size_t path_len = strlen(PRO_INITAIL_SETUP_CONFIGURAION_FILE_PATH);
    size_t name_len = strlen(config_file_name);
    size_t total_len = path_len + 1 + name_len + 1;
    char *config_file = (char*)malloc(total_len);
    if(!config_file)
    {
        printf("Memory allocation failed\n");
        return -1;
    }
    snprintf(config_file, total_len, "%s/%s", PRO_INITAIL_SETUP_CONFIGURAION_FILE_PATH, config_file_name);
    

    ret = access(config_file, F_OK);
    if(ret < 0)
    {
        // config 파일이 없으면 새로 생성
        FILE *config_fp = fopen(config_file, "w+");
        if(!config_fp)
        {
            printf("Failed to open config file for writing\n");
            free(config_file);
            return -1;
        }

        fputs("Configuration_Enable=0x00;\n", config_fp);

        fputs("----------------------------------------Utils---------------------------------------;\n", config_fp);
        fputs("----------------------------------------Mount;\n", config_fp);
        fputs("Utils_Mount_Enable=0;\t\t\t(0:Disable, 1:Enable\n", config_fp);
        fputs("Utils_Mount_Active_Mode=0;\t\t\t(0:No, 1:Auto_Mounting)\n", config_fp);
        fputs("Utils_Mount_Type=0;\t\t\t(0:SD_Card)\n", config_fp);
        fputs("Utils_Mount_Device=\"/dev/mmcblk1\";\n", config_fp);
        fputs("Utils_Mount_Path=\"/mnt/sdcard\";\n", config_fp);
        fputs("\n", config_fp);
        fputs("----------------------------------------V2X-----------------------------------------;\n", config_fp);
        fputs("V2X_Operation_Type=0;\t\t\t(0:rx only, 1:trx)\n", config_fp);
        fputs("V2X_Device_Name=\"/dev/spidev1.1\";\n", config_fp);
        fputs("V2X_Dbg_Msg_Level=0;\t\t\t(0:nothing, 1:err, 2:init, 3:event, 4:message hexdump)\n", config_fp);
        fputs("V2X_Lib_Dbg_Msg_Level=0;\t\t\t(0:nothing, 1:err, 2:init, 3:event, 4:message hexdump)\n", config_fp);
        fputs("----------------------------------------V2X-Rx\n", config_fp);
        fputs("V2X_Rx_Channel_Num=183;\n", config_fp);
        fputs("V2X_Rx_Power=23; [dBm]\n", config_fp);
        fputs("V2X_Rx_Bandwidth=20; [Hz]\n", config_fp);
        fputs("----------------------------------------V2X-Tx\n", config_fp);
        fputs("V2X_LTEV2XHAL_Tx_Type=0;\t\t\t(0:SPS, 1:Event)\n", config_fp);
        fputs("V2X_Tx_Channel_Num=183;\n", config_fp);
        fputs("V2X_Tx_DataRate=12;\n", config_fp);
        fputs("V2X_Tx_Power=10; \n", config_fp);
        fputs("V2X_Tx_Priority=7;\n", config_fp);
        fputs("V2X_PSID=32;\n", config_fp);
        fputs("V2X_Tx_WSM_Body_Len=100;\n", config_fp);
        fputs("V2X_Tx_Interval=100000;\n", config_fp);
        fputs("\n", config_fp);
#if 1
        fputs("----------------------------------------PCAP----------------------------------------;\n", config_fp);
        fputs("Pcap_File_Path=\"/mnt/sdcard/pcap/\";\n", config_fp);
        fputs("Pcap_File_Sub_Path_Enable=1;\t\t\t(YYDDMM)\n" , config_fp); 
        fputs("Pcap_File_Name_Header_0=\"KETI\";\n", config_fp); 
        fputs("Pcap_File_Name_Header_1=\"V2XnGNSS\";\n", config_fp); 
        fputs("Pcap_File_Name_YYMMDD_Enable=1;\n", config_fp);
        fputs("Pcap_File_Saving_Type=00;\t\t\t(00:Full, 01:Split, 02:Rolling) TBA\n", config_fp); 
        fputs("Pcap_File_Saving_Split_Size=0000; [MB]\n", config_fp); 
        fputs("Pcap_File_Saving_Rolling_Time=0000; [s]\n", config_fp);
#endif
        fclose(config_fp);
        free(config_file);
        printf("Fill the config_file in ./config/pro_config.conf\n");
        return 0;
    }
    else
    {
        FILE *config_fp = fopen(config_file, "r");
        if(!config_fp)
        {
            printf("Failed to open config file for reading\n");
            free(config_file);
            return -1;
        }
        char str[256] = {0};
        while(fgets(str, sizeof(str), config_fp))
        {
            // 개행 문자 제거
            str[strcspn(str, "\n")] = 0;
            char *ptr_name = strtok(str, "=");
            if(ptr_name)
            {
                char *ptr_value = strtok(NULL, ";");
                if(ptr_value && strlen(ptr_value) >= 1)
                {
                    // 값의 형식에 따라 타입 결정
                    char type;
                    size_t value_len = strlen(ptr_value);
                    if(ptr_value[0] == '"')
                    {
                        type = '"';
                    }
                    else if(value_len >= 2 && ptr_value[0]=='0' && (ptr_value[1]=='x' || ptr_value[1]=='X'))
                    {
                        type = 'x';
                    }
                    else if(value_len >= 2 && ptr_value[0]=='0' && (ptr_value[1]=='b' || ptr_value[1]=='B'))
                    {
                        type = 'b';
                    }
                    else
                    {
                        type = 'd'; // default: 정수형으로 처리
                    }
                    
                    if(strcmp(ptr_name, "Configuration_Enable") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->config_enable);
                        if(pro_config->config_enable == 0x00)
                        {
                            printf("Configuration not enable.\n");
                            fclose(config_fp);
                            free(config_file);
                            return -1 * __LINE__;
                        }
                    }else if(strcmp(ptr_name, "Utils_Mount_Enable") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->utils.mount.enable);
                    }else if(strcmp(ptr_name, "Utils_Mount_Type") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->utils.mount.type);
                    }else if(strcmp(ptr_name, "Utils_Mount_Active_Mode") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->utils.mount.active_mode);
                    }else if(strcmp(ptr_name, "Utils_Mount_Device") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)pro_config->utils.mount.device);
                    }
                    else if(strcmp(ptr_name, "Utils_Mount_Path") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)pro_config->utils.mount.path);
                    }else if(strcmp(ptr_name, "V2X_Operation_Type") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.op);
                    }else if(strcmp(ptr_name, "V2X_Device_Name") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)pro_config->v2x.dev_name);
                    }else if(strcmp(ptr_name, "V2X_Dbg_Msg_Level") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.dbg);
                    }else if(strcmp(ptr_name, "V2X_Lib_Dbg_Msg_Level") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.lib_dbg);
                    }else if(strcmp(ptr_name, "V2X_LTEV2XHAL_Tx_Type") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.tx_type);
                    }else if(strcmp(ptr_name, "V2X_Rx_Channel_Num") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.rx_chan_num);
                    }else if(strcmp(ptr_name, "V2X_Rx_Power") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.rx_power);
                    }else if(strcmp(ptr_name, "V2X_Rx_Bandwidth") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.rx_bandwidth);
                    }else if(strcmp(ptr_name, "V2X_Tx_Channel_Num") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.tx_chan_num);
                    }else if(strcmp(ptr_name, "V2X_Tx_DataRate") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.tx_datarate);
                    }else if(strcmp(ptr_name, "V2X_Tx_Power") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.tx_power);
                    }else if(strcmp(ptr_name, "V2X_Tx_Priority") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.tx_priority);
                    }else if(strcmp(ptr_name, "V2X_PSID") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.psid);
                    }else if(strcmp(ptr_name, "V2X_Tx_WSM_Body_Len") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.tx_wsm_body_len);
                    }else if(strcmp(ptr_name, "V2X_Tx_Interval") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->v2x.tx_interval);
                    }else if(strcmp(ptr_name, "Pcap_File_Path") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)pro_config->pcap.pcap_file_path);
                    }else if(strcmp(ptr_name, "Pcap_File_Sub_Path_Enable") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->pcap.pcap_file_sub_path_enable);
                    }else if(strcmp(ptr_name, "Pcap_File_Name_Header_0") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)pro_config->pcap.pcap_file_name_header_0);
                    }else if(strcmp(ptr_name, "Pcap_File_Name_Header_1") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)pro_config->pcap.pcap_file_name_header_1);
                    }else if(strcmp(ptr_name, "Pcap_File_Name_YYMMDD_Enable") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->pcap.pcap_file_name_yymmdd_enable);
                    }else if(strcmp(ptr_name, "Pcap_File_Saving_Type") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->pcap.pcap_file_saving_type);
                    }else if(strcmp(ptr_name, "Pcap_File_Saving_Split_Size") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->pcap.pcap_file_saving_split_size);  
                    }else if(strcmp(ptr_name, "Pcap_File_Saving_Rolling_Time") == 0)
                    {
                        PRO_Initial_Setup_Configuration_Value_Input(type, ptr_value, value_len, (void*)&pro_config->pcap.pcap_file_saving_rolling_time);
                    }
                }
            }
        }
        fclose(config_fp);
    }
    free(config_file);
    return 0;
}


/**
 * @brief Config 파일을 읽어서 out_value에 값 입력
 * 
 * @param type value 종류: 문자열("), 16진수(x), 2진수(b), 또는 default(정수, 'd')
 * @param value 입력 문자열
 * @param value_len 문자열 길이
 * @param out_value 출력값 (메모리 공간에 결과 복사)
 * @return void 
 */
static void PRO_Initial_Setup_Configuration_Value_Input(char type, char *value, size_t value_len, void* out_value)
{
    switch(type)
    {
        case 'b':   // 2진수 처리 ("0b" 또는 "0B"로 시작)
        {
            int decimal = 0; 
            for(size_t i = 2; i < value_len; i++)
            {
                char bin_char = value[i];
                decimal = decimal * 2 + (bin_char == '1' ? 1 : 0);
            }
            memcpy(out_value, &decimal, sizeof(int));
            break;
        }
        case 'x':   // 16진수 처리 ("0x" 또는 "0X"로 시작)
        {
            int decimal = 0; 
            for(size_t i = 2; i < value_len; i++)
            {
                char hex_char = value[i];
                int digit = 0;
                if (hex_char >= '0' && hex_char <= '9')
                { 
                    digit = hex_char - '0';
                } else if (hex_char >= 'A' && hex_char <= 'F')
                {
                    digit = hex_char - 'A' + 10;
                }else if (hex_char >= 'a' && hex_char <= 'f')  
                {
                    digit = hex_char - 'a' + 10;
                }
                decimal = decimal * 16 + digit;
            }
            memcpy(out_value, &decimal, sizeof(int));
            break;
        }
        case '"':   // 문자열 처리: 따옴표 제거 후 복사
        {
            size_t copy_len = value_len;
            if(value[0] == '"' && value[value_len - 1] == '"')
            {
                copy_len = value_len - 2;
                memcpy(out_value, value + 1, copy_len);
                ((char*)out_value)[copy_len] = '\0';
            }
            else
            {
                memcpy(out_value, value, copy_len);
                ((char*)out_value)[copy_len] = '\0';
            }
            break;
        }
        default:    // 기본: 정수값으로 처리 (한 자리 숫자 포함)
        {
            int v = atoi(value);
            memcpy(out_value, &v, sizeof(int));
            break;
        }
    }
}
