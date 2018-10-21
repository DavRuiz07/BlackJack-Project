/* ~Note~ Server must start first before client for connection to be made */
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#define SERVER "10.129.100.202"

#define PORT 6060		//port to connect to server

//------Client-side code for  blackjack------//

int main(void)
{
	//Initialize Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("\nFailed. Error: %d",WSAGetLastError());
    }

	// Create socket
	int sock;
	if ((sock=socket(AF_INET,SOCK_DGRAM,0)) == SOCKET_ERROR){
		printf("\nSocket failed, Error: %d",WSAGetLastError());
	}

	// Fill in the socket structure
	struct sockaddr_in place;
	memset((char *) &place, 0, sizeof(place));
	place.sin_family = AF_INET;
	place.sin_port = htons(PORT);
	place.sin_addr.S_un.S_addr = inet_addr(SERVER);

	/* Code Starts Here */
	int length = sizeof(place);
    char message[1000];
    char recieve[1000];
    int recvlen;
    int opt;
    int balance = 1000;
    int bet = 0;
    int playercard;
    int dealercard;
    int playerhand[100];
    int cont = 1;


//Games Start
while(cont)
    {
    int turn = 1;
    int cont1 = 1;
//Enter Name
printf("Welcome to Black Jack\n\n");
    printf("Please Enter Name> ");
    scanf("%s",&message);

            if (sendto(sock, message,sizeof(message) , 0 , (struct sockaddr *)&place, &length) == SOCKET_ERROR)
             {
                printf("\nsendto() failed with error code : %d" , WSAGetLastError());
             }

            if((recvlen = recvfrom(sock,recieve,sizeof(recieve),0,(struct sockaddr *)&place,&length) == SOCKET_ERROR))
             {
                printf("recvfrom() failed with error %d", WSAGetLastError());
             }
//Make bet
         printf("\nMake your bet : Your Balance is %d\nHow Much would you like to bet? \n", balance);
         scanf("%d",&bet);
         while(bet>balance)
         {
             printf("Bet is to High!\nPlease Enter New Bet");
             scanf("%d",&bet);
         }
    balance -= bet;
    printf("\nYour Bet is %d and your balance is now %d\n",bet,balance);

for(int i; i<100;i++)
{
    playerhand[i] = 0;
}


printf("\n\nGame Will Now Start\n\n");

//Deals Initial Card
dealercard = (int)recieve[0];
playercard = (int)recieve[1];
playerhand[0] = playercard;
printf("Dealer Card: %d\n",dealercard);
printf("Your Card: %d\n",playercard);

//User Input
while(cont1){
printf("\n1 = Hit, 2 = Stand\n\n");
scanf("%d",&opt);

switch(opt)
{
//Hit
    case 1:
        message[0] = '2';
         if (sendto(sock, message,sizeof(message) , 0 , (struct sockaddr *) &place, &length) == SOCKET_ERROR)
             {
                printf("sendto() failed with error code : %d" , WSAGetLastError());
             }
        if((recvlen = recvfrom(sock,recieve,sizeof(recieve),0,(struct sockaddr *)&place,&length) == SOCKET_ERROR))
             {
                printf("recvfrom() failed with error %d", WSAGetLastError());
             }

        printf("Dealer Card: %d\n",dealercard);

        playercard = recieve[0];
        playerhand[turn] = playercard;
        printf("Your Cards: ");
        for(int i = 0; i <= turn;i++)
        {
            printf("%d ",playerhand[i]);
        }
        turn++;
        break;
//Stand
    case 2:
        message[0] = '3';

        if (sendto(sock, message,sizeof(message) , 0 , (struct sockaddr *) &place, &length) == SOCKET_ERROR)
             {
                printf("sendto() failed with error code : %d" , WSAGetLastError());
             }

        printf("Dealer Cards are being drawn...\n");

         if((recvlen = recvfrom(sock,recieve,sizeof(recieve),0,(struct sockaddr *)&place,&length) == SOCKET_ERROR))
             {
                printf("recvfrom() failed with error %d", WSAGetLastError());
             }
        printf("Dealer Has %d",recieve[0]);
        if(recieve[1]=='W'){
            printf("\n\nYOU WIN\n\n");
            balance += bet * 2;
            printf("You Won %d, Your Balance is now %d\n",bet*2,balance);
        }
        if(recieve[1]=='L')
        {
            printf("\n\nYOU LOSE\n\n");
            printf("You Lost %d, Your Balance is now %d\n",bet,balance);
        }

//Play Again
    int ans;
        printf("Would You Like To Play Again?  Y = 1 / N = 2  >");
        scanf("%d",&ans);

        if(ans == 1)
        {
            printf("Game Restarting\n");
            message[0] = '1';
            cont1 = 0;
        }

        if(ans == 2)
        {
            printf("Game Over\n");
            message[0] = '2';
            cont = 0;
            cont1 = 0;
        }

        if (sendto(sock, message,sizeof(message) , 0 , (struct sockaddr *) &place, &length) == SOCKET_ERROR)
             {
                printf("\nsendto() failed with error code : %d" , WSAGetLastError());
             }
        break;

default:
        printf("No option Given\n");
        }
    }
}
	// Code Ends Here 

	// Clean Up
	closesocket(sock);
	WSACleanup();
	return 0;
}

