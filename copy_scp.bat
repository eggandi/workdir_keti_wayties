!bin/bash

if "%1" == "2" (
    scp -r ./* root@192.168.137.100:~/ws_pcap/
) else if "%1" == "1" (
    scp  ./* root@192.168.137.100:~/ws_pcap/
) else if "%1" == "3" (
    scp -r root@192.168.137.100:~/ws_pcap/f_db/pcap/* ./pcap/
) else if "%1" == "4" (
    scp -r root@192.168.137.100:~/ws_pcap/output/bin/f_db/pcap/* ./pcap/
) else (
    scp -r ./src/app/KETI_PRO/* root@192.168.137.100:~/ws_pcap/src/app/KETI_PRO/
)
