#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "comport.h"

comport Serial("/dev/ttyUSB0", 115200, 8, 'N', 1);

void *SendThread(void *data)
{
    uint32_t cnt = 0;
    while (1)
    {

        char buf[128] = {0};
        int len = sprintf(buf, "TESTMSG: %d\n", cnt);

        Serial.comwrite((uint8_t *)buf, len);

        usleep(100000);
        cnt++;
    }
    pthread_exit(NULL); // 離開子執行緒
}

void *ReceiveThread(void *data)
{

    while (1)
    {
        uint8_t read_buffer[32] = {0};
        int bytes_read = 0;

        bytes_read = Serial.comread(read_buffer, 32);
        if (bytes_read > 0)
        {
            printf("%s", read_buffer);
        }
        else
        {
            printf("Should not print this line when use blocking read (like select)\n");
        }
    }
    pthread_exit(NULL);
}


int main()
{
    pthread_t t_Send, t_Receive;
    pthread_create(&t_Send, NULL, SendThread, NULL);
    pthread_create(&t_Receive, NULL, ReceiveThread, NULL);

    pthread_join(t_Send, NULL);
    pthread_join(t_Receive, NULL);
    return 0;
}