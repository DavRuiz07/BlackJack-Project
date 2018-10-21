# BlackJack-Project
This Project was my attempt to creating a blackjack game using a server(host) and client(player) using the UDP Protocol.

For this solution, the server IP address must be manually inputed into the Client side code.
Then the Server must start first before the client in order for the initial message to be sent and the game to begin.
Once the game has begun, the client and server can continue to keep playing with each other so long the messages are being recieved.

Note ~ 
If the Client does not begin playing when started, then it has not been able to communicate with the server and will need to be restarted.
  *This will happen if the Server has not been started before the client or there is an issue with the IP address configuration.

If the Server and Client for any reason miss a message from each other, both programs must be restarted since there is no feature to detect or resend missed messages. 
