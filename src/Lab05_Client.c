#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 1111
#define setIP "0.0.0.0"


int main(){
    int k;
    for(k=0; k<2; k++){
        printf("Step %d!\n", k+1);
        //創建套接字
        int sockfd;
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("socket error");
            return -1;
        }
        printf("socket successed....\n");
        //TCP連線
        struct sockaddr_in cli_addr;
        memset(&cli_addr, 0, sizeof(cli_addr));
        cli_addr.sin_family = AF_INET;
        cli_addr.sin_port = htons(PORT);
        cli_addr.sin_addr.s_addr = inet_addr(setIP);
        //等候連線，最多等候15秒
        int cnctfd;
        int i;
        for(i=0; i<15; i++){
            printf("Wait %d sec...\n", i+1);
            sleep(1);
            //為了在15秒末進行最後一次嘗試connect，因此將connect放此
            cnctfd = connect(sockfd, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
            if(cnctfd == 0){
                break;
            }
        }
        if(cnctfd < 0){
            perror("connect error");
            return -1;
        }
        printf("connect successed...\n");
        //輸入隨機數0~999
        int rand_num;
        int num_len;
        char rand_char[10];
        int flag = 1;
        while(flag){
            printf("Please enter 0~999\n");
            scanf("%s", rand_char);
            rand_num = atoi(rand_char);
            num_len = 0;
            while(rand_num != 0){
                rand_num/=10;
                num_len++;
            }
            rand_num = atoi(rand_char);
            //排除(數字+文字)經atoi會變為(數字)之部分
            if(num_len != strlen(rand_char)){
                printf("%s is NOT 0~999\n", rand_char);                             
                continue;
            }
            //1~999
            else if(rand_num>0 && rand_num<1000){
                flag = 0;
            }
            //非整數atoi值為0，需另外做判斷
            else if(strcmp(rand_char, "0") == 0){
                flag = 0;
            }
            else{
                printf("%s is NOT 0~999\n", rand_char);
                continue;
            }
        }
        //建立訊息參數
        char buf[255];
        char sender[20] = "Client";
        char empl_no[10];
        unsigned char hex1;
        unsigned char hex2;
        unsigned char msg_txt[255];
        //訊息內容
        memset(buf, 0, sizeof(buf));
        if(k == 0){
            strcpy(empl_no, "1st_time");
            sprintf(buf, "%s Hello %s %d", sender, empl_no, rand_num);
        }
        else{
            strcpy(empl_no, "2nd_time");
            sprintf(buf, "%s Hello %s %d", sender, empl_no, rand_num);
        }
        //訊息長度0x??
        hex1 = 0x00;
        hex2 = 0x00 + strlen(buf);
        //發送
        memset(msg_txt, 0, sizeof(msg_txt));
        int cli_snd;
        if(k == 0){
            strcat(msg_txt, &hex1);
            strcat((msg_txt+1), &hex2);
            strcat((msg_txt+2), buf);
            cli_snd = send(sockfd, &msg_txt, sizeof(msg_txt), 0);
            if(cli_snd < 0){
                perror("send error");
                exit(EXIT_FAILURE);
            }
            printf("send: \"%s\"\n", buf);
        }
        else{
            //傳送前半段
            sprintf(buf, "%s Hello", sender);
            strcat(msg_txt, &hex1);
            strcat((msg_txt+1), &hex2);
            strcat((msg_txt+2), buf);
            cli_snd = send(sockfd, &msg_txt, sizeof(msg_txt), 0);
            if(cli_snd < 0){
                perror("send error");
                exit(EXIT_FAILURE);
            }
            printf("send: \"%s\"\n", buf);
            //睡眠5秒
            int j;
            for(j=0; j<5; j++){
                printf("Sleep %d\n", 5-j);
                sleep(1);
            }
            //傳送後半段
            sprintf(buf, "%s %d", empl_no, rand_num);
            memset(msg_txt, 0, sizeof(msg_txt));
            strcat(msg_txt, buf);
            cli_snd = send(sockfd, &msg_txt, sizeof(msg_txt), 0);
            if(cli_snd < 0){
                perror("send error");
                exit(EXIT_FAILURE);
            }
            printf("send: \"%s\"\n",buf);
        }
        //讀取消息隊列中的訊息
        memset(buf, 0, sizeof(buf));
        int cli_read = read(sockfd, &buf, sizeof(buf));
        if(cli_read < 0){
            perror("read error");
            return -1;
        }
        printf("recv: \"%s\"\n", buf);
        printf("-----------------------------------\n");
        //關閉套接字
        close(sockfd);
    }
    return 0;
}