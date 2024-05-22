#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
    #include <pthread.h> //for pthread_create
	void OSInit( void )
	{
		WSADATA wsaData;
		int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData ); 
		if( WSAError != 0 )
		{
			fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
			exit( -1 );
		}
	}
	void OSCleanup( void )
	{
		WSACleanup();
	}
	#define perror(string) fprintf( stderr, string ": WSA errno = %d\n", WSAGetLastError() )
#else
	#include <sys/socket.h> //for sockaddr, socket, socket
	#include <sys/types.h> //for size_t
	#include <netdb.h> //for getaddrinfo
	#include <netinet/in.h> //for sockaddr_in
	#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
	#include <errno.h> //for errno
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif

int initialization();
int connection( int internet_socket );
void execution( int client_internet_socket );
void cleanup( int internet_socket, int client_internet_socket );
int totalbytes_sent = 0;

int main( int argc, char * argv[] )
{
	//////////////////
	//Initialization//
	//////////////////
    printf("Program Start\n");
	OSInit();

	int internet_socket = initialization();

	//////////////
	//Connection//
	//////////////
	while (1){
		int client_internet_socket = connection( internet_socket );
		execution( client_internet_socket );
	}	

	/////////////
	//Execution//
	/////////////

	////////////
	//Clean up//
	////////////

	//cleanup( internet_socket, client_internet_socket );

	//OSCleanup();

	return 0;
}

int initialization()
{
	//Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	internet_address_setup.ai_flags = AI_PASSIVE;
	int getaddrinfo_return = getaddrinfo( NULL, "22", &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 1 );
	}

	int internet_socket = -1;
	struct addrinfo * internet_address_result_iterator = internet_address_result;
	while( internet_address_result_iterator != NULL )
	{
		//Step 1.2
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			perror( "socket" );
		}
		else
		{
			//Step 1.3
			int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( bind_return == -1 )
			{
				perror( "bind" );
				close( internet_socket );
			}
			else
			{
				//Step 1.4
				int listen_return = listen( internet_socket, 1 );
				if( listen_return == -1 )
				{
					close( internet_socket );
					perror( "listen" );
				}
				else
				{
					break;
				}
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo( internet_address_result );

	if( internet_socket == -1 )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		exit( 2 );
	}

	return internet_socket;
}

char ip_address[INET6_ADDRSTRLEN];

int connection( int internet_socket )
{
    printf("+-----------------------------+\n");
	printf("|    Waiting for connection   |\n");
    printf("+-----------------------------+\n");
	//Step 2.1
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	int client_socket = accept( internet_socket, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
	if( client_socket == -1 )
	{
		perror( "accept" );
		close( internet_socket );
		exit( 3 );
	}
	void* addr;
	if( client_internet_address.ss_family == AF_INET )
	{
		struct sockaddr_in * s = (struct sockaddr_in *) &client_internet_address;
		addr = &(s->sin_addr);
	}
	else
	{
		struct sockaddr_in6 * s = (struct sockaddr_in6 *) &client_internet_address;
		addr = &(s->sin6_addr);
	}

    char ip_address[INET6_ADDRSTRLEN];
	inet_ntop( client_internet_address.ss_family, addr, ip_address, sizeof ip_address );

	FILE* logfile = fopen("log.txt", "a");
	if(logfile == NULL){
		perror("fopen");
		close(client_socket);
		exit(4);
	}
	fprintf(logfile, "Connection from %s\n", ip_address);

	return client_socket;
}


void http(){
	int sockfd;
	struct sockaddr_in server_addr;
	char response[1024];
	char request[100];

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Socket");
		return;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);
	server_addr.sin_addr.s_addr = inet_addr("");

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
		perror("Connect");
		return;
	}

    snprintf(request, sizeof(request), "GET /json/%s HTTP/1.0\r\nHost: ip-api.com\r\n\r\n", ip_address);

	if(send(sockfd, request, strlen(request), 0) == -1){
		perror("send");
		return;
	}

    FILE* file = fopen("log.txt", "a");
    if (file == NULL) {
        perror("fopen");
        return;
    }

	while (1)
	{
		ssize_t bytes_received = recv(sockfd, response, 1024 - 1, 0);
		if (bytes_received == -1){
			perror("recv");
			break;
		}
		else if (bytes_received == 0){
			break;
		}

		response[bytes_received] = '\0';

		fprintf(file, "Get request response: \n%s\n", response);
		printf("Get request response: \n%s\n", response);
	}
	fclose(file);
	close(sockfd);
}
void* send_message(void* arg)
{
	int client_internet_socket = *(int*) arg;
	const char * message = 
    "⠐⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠂\n"
	"⠄⠄⣰⣾⣿⣿⣿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣆⠄⠄\n"
	"⠄⠄⣿⣿⣿⡿⠋⠄⡀⣿⣿⣿⣿⣿⣿⣿⣿⠿⠛⠋⣉⣉⣉⡉⠙⠻⣿⣿⠄⠄\n"
	"⠄⠄⣿⣿⣿⣇⠔⠈⣿⣿⣿⣿⣿⡿⠛⢉⣤⣶⣾⣿⣿⣿⣿⣿⣿⣦⡀⠹⠄⠄\n"
	"⠄⠄⣿⣿⠃⠄⢠⣾⣿⣿⣿⠟⢁⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠄⠄\n"
	"⠄⠄⣿⣿⣿⣿⣿⣿⣿⠟⢁⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠄⠄\n"
	"⠄⠄⣿⣿⣿⣿⣿⡟⠁⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠄⠄\n"
	"⠄⠄⣿⣿⣿⣿⠋⢠⣾⣿⣿⣿⣿⣿⣿⡿⠿⠿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⠄⠄\n"
	"⠄⠄⣿⣿⡿⠁⣰⣿⣿⣿⣿⣿⣿⣿⣿⠗⠄⠄⠄⠄⣿⣿⣿⣿⣿⣿⣿⡟⠄⠄\n"
	"⠄⠄⣿⡿⠁⣼⣿⣿⣿⣿⣿⣿⡿⠋⠄⠄⠄⣠⣄⢰⣿⣿⣿⣿⣿⣿⣿⠃⠄⠄\n"
	"⠄⠄⡿⠁⣼⣿⣿⣿⣿⣿⣿⣿⡇⠄⢀⡴⠚⢿⣿⣿⣿⣿⣿⣿⣿⣿⡏⢠⠄⠄\n"
	"⠄⠄⠃⢰⣿⣿⣿⣿⣿⣿⡿⣿⣿⠴⠋⠄⠄⢸⣿⣿⣿⣿⣿⣿⣿⡟⢀⣾⠄⠄\n"
	"⠄⠄⢀⣿⣿⣿⣿⣿⣿⣿⠃⠈⠁⠄⠄⢀⣴⣿⣿⣿⣿⣿⣿⣿⡟⢀⣾⣿⠄⠄\n"
	"⠄⠄⢸⣿⣿⣿⣿⣿⣿⣿⠄⠄⠄⠄⢶⣿⣿⣿⣿⣿⣿⣿⣿⠏⢀⣾⣿⣿⠄⠄\n"
	"⠄⠄⣿⣿⣿⣿⣿⣿⣿⣷⣶⣶⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⠋⣠⣿⣿⣿⣿⠄⠄\n"
	"⠄⠄⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⢁⣼⣿⣿⣿⣿⣿⠄⠄\n"
	"⠄⠄⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⢁⣴⣿⣿⣿⣿⣿⣿⣿⠄⠄\n"
	"⠄⠄⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⢁⣴⣿⣿⣿⣿⠗⠄⠄⣿⣿⠄⠄\n"
	"⠄⠄⣆⠈⠻⢿⣿⣿⣿⣿⣿⣿⠿⠛⣉⣤⣾⣿⣿⣿⣿⣿⣇⠠⠺⣷⣿⣿⠄⠄\n"
	"⠄⠄⣿⣿⣦⣄⣈⣉⣉⣉⣡⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⠉⠁⣀⣼⣿⣿⣿⠄⠄\n"
	"⠄⠄⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣾⣿⣿⡿⠟⠄⠄\n"
	"⠠⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄\n";

	printf("\nATTACK!\n");
	while (1)
	{
		int bytes_sent = send( client_internet_socket, message, strlen( message ), 0 );
		if (bytes_sent == -1)
		{
			perror("send");
			break;
		}
		usleep(1000000);
		totalbytes_sent += bytes_sent;		
	}
	printf("\nATTACK END!\n");
    return NULL;
}

void execution( int client_internet_socket )
{
    printf("+-----------------------------+\n");
	printf("|    \nStarten an execution   |\n");
    printf("+-----------------------------+\n");
	http();
	char buffer[1000];

    pthread_t send_thread;
    pthread_create(&send_thread, NULL, send_message, &client_internet_socket);

	while (1)
	{
			//Step 3.1
		int number_of_bytes_received = recv( client_internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
		if( number_of_bytes_received == -1 )
		{
			perror( "recv" );
            break;
		}
		else if (number_of_bytes_received == 0)
		{
			printf("Client closed\n");
			break;
		}
			buffer[number_of_bytes_received] = '\0';
			printf( "Received : %s\n", buffer );

		FILE* logfile = fopen("log.txt", "a");
		if(logfile == NULL){
		perror("fopen");
		break;
	}
	fprintf(logfile, "client message: %s\n", buffer);
	fclose(logfile);
	}
	
	pthread_join(send_thread, NULL);

	FILE* logfile = fopen("log.txt", "a");
	if(logfile == NULL){
		perror("fopen");
		close(client_internet_socket);
		exit(4);
	}
	fprintf(logfile, "Connection from %s\n", totalbytes_sent);
	fclose(logfile);
	printf("Connection from %s\n", totalbytes_sent);
	close(client_internet_socket);
}

void cleanup( int internet_socket, int client_internet_socket )
{
	//Step 4.2
	int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}

	//Step 4.1
	close( client_internet_socket );
	close( internet_socket );
}