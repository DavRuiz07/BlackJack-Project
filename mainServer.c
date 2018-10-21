/* ~Note~ Server must start first before client for connection to be made */
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sys/time.h>                               //FD_time
#include <time.h>

#define PORT 6060                                   //port to connect to server

//------Server-side code for blackjack-------//

int main()
{
    SOCKET server;                                  //create socket
    int recvlen,opt = 1;
    char recbuffer[1000];                           //receiving msg buffer
    int cont = 1;									//sending msg buffer
    char sendbuffer[1000];

    //-----Struct address of client & Server------//
    struct sockaddr_in ServerAdd;                   //server address
    struct sockaddr_in place;                       //client address
    int serverlen = sizeof(ServerAdd);              //size of address
    int clientlen = sizeof(place);                  //size byte size of client address

    //-----Initializing Winsock-----//
    WSADATA wsa;
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
      printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Socket Initialised.\n");

    //-----(MASTER)Server Socket creation-----//
    if(( server = socket(AF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET)
    {
        printf("Cannot create socket : %d" , WSAGetLastError());
    }
    printf("Socket created...\n");

    //-----Server Internet address-----//
    ServerAdd.sin_family = AF_INET;
    ServerAdd.sin_port = htons( PORT );
    ServerAdd.sin_addr.s_addr = INADDR_ANY;

    //-----Reuse socket/Allow for Multiple Connections-----//
    if(setsockopt(server,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))<0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //-----Bind socket to port/Forcefully attach socket to port 6060------//
    if(bind(server,(struct sockaddr *)&ServerAdd, serverlen) == SOCKET_ERROR);
    {
        printf("\nBind error : %d" , WSAGetLastError());
    }
    printf("\nBind done\n");
    printf("Port 6060 is open\n");

    while(cont)
    {
    int cont1 = 1;
    int clientsum = 0;
    int serversum = 0;
    int Playerhand = 0;
    int dealerhand = 0;
    char playername[1000];
    char sendfirst[1000];

    //-----DEALING FIRST CARDS-----//
    srand(time(NULL));
    dealerhand = rand() % 11;
    Playerhand = rand() % 11;
    printf("\nBEGIN DEALING");
    printf("\nPlayer Hand = %d",Playerhand);
    printf("\nDealer Hand = %d",dealerhand);

    sendfirst[1] = (char)Playerhand;
    sendfirst[0] = (char)dealerhand;
    serversum += dealerhand;
    clientsum += Playerhand;

    //-----RECEIVE FROM FUNCTION-----//
    if((recvlen = recvfrom(server,playername,sizeof(playername),0,(struct sockaddr *)&place,&clientlen) == SOCKET_ERROR))
    {
        printf("\n\nrecvfrom() failed with error code : %d" , WSAGetLastError());
        while(1);
    }
    printf("\n\nReceived packet from %s:%d\n", inet_ntoa(place.sin_addr), ntohs(place.sin_port));
    printf("\nClient Name: ");
    printf(playername,recvlen);


    //-----SENDTO FUCNTION-----//
    if(sendto(server,sendfirst,sizeof(sendfirst),0,(struct sockaddr *)&place,clientlen) == SOCKET_ERROR)
    {
        printf("\nsendto() failed with error code : %d\n\n" , WSAGetLastError());
        while(1);
    }


    //-----WHILE LOOP FOR SENDTO() & RECV()-----//
    while(cont1)
    {
        printf("\nWaiting for client...\n");
        fflush(stdout);

        //-----RECEIVE FROM FUNCTION-----//
        if((recvlen = recvfrom(server,recbuffer,sizeof(recbuffer),0,(struct sockaddr *)&place,&clientlen) == SOCKET_ERROR))
        {
            printf("\nrecvfrom() failed with error code : %d" , WSAGetLastError());
			while(1);
        }
        printf("\n-------------------------------------------------");
        printf("Received packet from %s:%d\n", inet_ntoa(place.sin_addr), ntohs(place.sin_port));


        char code;
        code = recbuffer[0];
            switch(code) //input from receive
            {
            case '2'://HIT
                printf("\n Server Says: Card is being drawn...");
                srand(time(NULL));
                Playerhand = rand() % 12 + 1;
                sendbuffer[0] = (char)Playerhand;
                clientsum += Playerhand;
                printf("\nServer Says:Player TOTAL:%d\n",clientsum);

                if(sendto(server,sendbuffer,sizeof(sendbuffer),0,(struct sockaddr *)&place,clientlen) == SOCKET_ERROR)
                {
                    printf("\nsendto() failed with error code : %d\n\n" , WSAGetLastError());
                    while(1);
                }
                    printf("\n\n----Hit!----\n");
                break;

            case '3'://stand
                printf("\n\n----Stand!----\n");
                printf("\n Server Says: Dealer's Cards are being drawn...\n");
                while(serversum<17)
                {
                    srand(time(NULL));
                    dealerhand = rand() % 12 + 1;
                    serversum += dealerhand;
                }
                sendbuffer[0] = (char)serversum;
                printf("\nServer Says:Dealer TOTAL:%d\n",serversum);
            //Decide Win
                if(serversum>21)
                {
                    printf("Dealer Busts!\n");
                    sendbuffer[1] = 'W';
                }

                if(clientsum>21)
                {
                    printf("Player Busts!\n");
                    sendbuffer[1] = 'L';
                }

                if(clientsum>=serversum && clientsum<=21)
                {
                    printf("Player Wins!\n");
                    sendbuffer[1] = 'W';
                }

                if(serversum>clientsum && serversum<=21)
                {
                    printf("Dealer Wins\n");
                    sendbuffer[1] = 'L';
                }


            //Send Win
                if(sendto(server,sendbuffer,sizeof(sendbuffer),0,(struct sockaddr *)&place,clientlen) == SOCKET_ERROR)
                {
                    printf("\nsendto() failed with error code : %d\n\n" , WSAGetLastError());
                    while(1);
                }

            //Play again receive
                if((recvlen = recvfrom(server,recbuffer,sizeof(recbuffer),0,(struct sockaddr *)&place,&clientlen) == SOCKET_ERROR))
                {
                    printf("\nrecvfrom() failed with error code : %d" , WSAGetLastError());
                    while(1);
                }

            int ans = 0;
            ans = (int)recbuffer[0];
            printf("\n--%d--\n",ans);

                if (ans == 50)
                {
                    printf("Game Over\n\n\n");
                    return 0;
                }

                if (ans == 49)
                {
                    printf("Game Restarting\n\n\n");
                    cont1 = 0;
                }

                break;

            default:
            printf("\nno options made\n");
            printf("\n%d",code);
            }

        }
    }
    closesocket(server);
    WSACleanup();
    return 0;
    }




