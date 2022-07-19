#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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
        printf("socket successed....\n");//成功則列印「socket。。。。」
        //若port遭占用則重複使用#include <stdio.h>
        int reuse = 1;
        int sso = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
        if(sso == -1){
            perror("setsockopt error");
            return -1;
        }
        //TCP連線
        struct sockaddr_in ser_addr;//創建結構體
        int addr_sz = sizeof(ser_addr);
        memset(&ser_addr, 0, sizeof(ser_addr));//內存清零
        ser_addr.sin_family = AF_INET;//選擇IPV4地址類型
        ser_addr.sin_port = htons(PORT);//選擇埠號
        ser_addr.sin_addr.s_addr = inet_addr(setIP);//選擇IP位址
        //綁定套接字
        if(bind(sockfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)) < 0){      
            perror("bind error");
            return -1;
        }
        printf("bind successed......\n");
        //調用listen對指定埠進行監聽
        if(listen(sockfd, 8) < 0){                                         
            perror("listen error");
            return -1;
        }
        printf("listen successed....\n");
        //使用accept從消息隊列中獲取請求
        int acptfd = accept(sockfd, (struct sockaddr*)&ser_addr, (socklen_t *)&addr_sz);                      
        if (acptfd < 0){
            perror("accept error");
            return -1;
        }
        printf("accept successed....\n");
        //關閉本身listen之socket fd
        close(sockfd);
        //建立訊息參數
        char buf[255];
        char tmp_buf[255];
        int len = 0;
        unsigned char hex1;
        unsigned char hex2;
        unsigned char msg_txt[255];
        //讀取消息隊列中的訊息
        int ser_read;
        memset(buf, 0, sizeof(buf));
        ser_read = read(acptfd, &buf, sizeof(buf));
        if(ser_read < 0){
            perror("read error");
            return -1;
        }
        memset(tmp_buf, 0, sizeof(tmp_buf));
	    memcpy(tmp_buf, &buf[2], sizeof(buf)-2);//去掉前2byte，保留單純訊息
        printf("recv: \"%s\"\n", tmp_buf);
        //統計所收到訊息長
        hex1 = buf[0];
        hex2 = buf[1];
        len = strlen(tmp_buf);
        //防止半包
	    memcpy(buf, &tmp_buf, sizeof(tmp_buf));//將單純訊息存入buf
        memset(tmp_buf, 0, sizeof(tmp_buf));
        while(1){
            //判斷訊息長度
            if(len == hex2){
                strcat(tmp_buf, buf);
                break;
            }
            else if(len < hex2){
                strcat(tmp_buf, buf);
            }
            else{
                printf("Message length is error!\n");
                exit(EXIT_FAILURE);
            }
            ser_read = read(acptfd, &buf, sizeof(buf));
            if(ser_read < 0){
                perror("read error");
                return -1;
            }
            printf("recv: \"%s\"\n", buf);
            len += strlen(buf);
        }//產出保留完整單純訊息的tmp_buf
        //拆分接收訊息
        char sender[20];
        char empl_no[10];
        int rand_num;
        sscanf(tmp_buf, "%s Hello %s %d", sender, empl_no, &rand_num);
        //處理訊息
        strcpy(sender, "Server");
        //訊息內容
        sprintf(buf, "%s Hello %s %d", sender, empl_no, rand_num);
        // 回覆
        memset(msg_txt, 0, sizeof(msg_txt));
        strcat(msg_txt, buf);
        int ser_snd;
        ser_snd = send(acptfd, &msg_txt, sizeof(msg_txt), 0);
        if(ser_snd < 0){
            perror("send error");
            exit(EXIT_FAILURE);
        }
        printf("send: \"%s\"\n", msg_txt);
        printf("-----------------------------------\n");
        //斷開連接
        close(acptfd);      
    }                                                
    return 0;
}