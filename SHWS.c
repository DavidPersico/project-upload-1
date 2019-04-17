/*/
 *    Heavily modified C ECHO Server example using sockets
 *    From https://www.binarytides.com/server-client-example-c-sockets-linux/
 *    David Persico
/*/

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

// Set packet size and limit for max client connect
#define MAXCLIENTS 1000
#define BUFSIZE 1024

int listenfd;
int clientConnection[MAXCLIENTS];
char *WWW;

// The fork function
void response_handler(int);

// Error, a wrapper for perror
void error(char *msg)
{
  perror(msg);
  exit(1);
}

// Function to combine to strings into one
char* concat(const char *s1, const char *s2)
{
    char *result = malloc( strlen(s1) + strlen(s2) + 1 ); // +1 for the null-terminator
    if ( result == NULL ) error ("concat function malloc()");
    strcpy(result, s1 );
    strcat(result, s2 );
    return result;
}

// Function to get size of the file from https://www.includehelp.com/c-programs/find-size-of-file.aspx*/
long int findSize(const char *file_name)
{
    struct stat st;
    // Get size with stat()
    if ( stat(file_name,&st)==0 )
        return (st.st_size);
    else
        return -1;
}

// Function to get file extension from https://stackoverflow.com/questions/5309471/getting-file-extension-in-c
const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}


int main(int argc , char **argv)
{
  struct sockaddr_in clientaddr;
  socklen_t addrlen;
  struct addrinfo hints, *res, *p;
  int spot = 0; // The specific client we are servicing out of MAXCLIENTS

  // Check if port number in args
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  // Mark each clientConnection spot as -1 for unused
	for (int i=0; i<MAXCLIENTS; i++)
		clientConnection[i] = -1;

  //Get address
  bzero((char *) &hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ( getaddrinfo(NULL, argv[1], &hints, &res) != 0 ) error("getaddrinfo() error");

  // Set socket() and bind()
  for (p = res; p != NULL; p = p -> ai_next)
  {
    listenfd = socket(p->ai_family, p -> ai_socktype, 0);
    if ( listenfd == -1 ) continue;
    if ( bind(listenfd, p -> ai_addr, p -> ai_addrlen) == 0 ) break;
  }
  if (p == NULL) error ("socket() and bind()");

  // Set directory of website we are hosting
  WWW = concat(getenv("PWD"),"/www");
  freeaddrinfo(res);

  // Begin listening for incoming connections
  if ( listen(listenfd, 1000000) != 0 ) error("listen() error");

  // Handler for accepting incoming connections
  while (1)
  {
    addrlen = sizeof(clientaddr);
    clientConnection[spot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

    if (clientConnection[spot] < 0) error("accept() error");

    else
    {
      // Handle multiple clients with new procces via fork()
      if ( fork() == 0 ) // NOTE I will switch this to pthreads
      {
        response_handler(spot);
        exit(0);
      }
    }
    while (clientConnection[spot] !=-1 ) spot = (spot + 1) % MAXCLIENTS;
  }
  return 0;
}

void response_handler(int n)
{
	char message[100000]; // Incoming message
  char *requestline[3]; // For message parsing
  char buffer[BUFSIZE]; //Holds data we send
  char path[100000]; // File path
  char strSize[100000]; //File size
  char *content;//both extension and filesize content information
	int rcvd; //Client received
  int fd; //File in directory found
  int BUFSIZE_read;
  long int size; //File size

  // Clear memory and recieve client message
  bzero(message,100000);
	rcvd = recv(clientConnection[n], message, 100000, 0);
	if (rcvd < 0)
		perror("recv failed");
	else if (rcvd == 0)
		perror("Client upexpectedly disconnected");
  // Message received
	else
	{
		printf("%s", message);
		requestline[0] = strtok(message, " \t\n");
		if ( strncmp(requestline[0], "GET\0", 4) == 0 )
		{
			requestline[1] = strtok(NULL, " \t");
			requestline[2] = strtok(NULL, " \t\n");
      // Check Request
			if (strncmp(requestline[2], "HTTP/1.0", 8) != 0 && strncmp(requestline[2], "HTTP/1.1", 8) != 0)
			{
				write(clientConnection[n], "HTTP/1.1 500 Internal Server Error\n", 35);
			}
			else
			{
				if (strncmp(requestline[1], "/\0", 2) == 0)
				{
          requestline[1] = "/index.html"; // Default to index.html as is standard
        }
				snprintf(path, sizeof(path), "%s", WWW);
				strcpy(&path[strlen(WWW)], requestline[1]);
				printf("file: %s\n", path);
        fd = open(path, O_RDONLY);
        // File found at correct path
				if (fd != -1)
				{
          // Send correct file infomation header
          size = findSize(path);
          snprintf(strSize, sizeof(strSize), "%ld", size);
          content = concat(concat(concat("HTTP/1.1 200 Document Follows\r\n Content-Type: ", get_filename_ext(path)), concat("\r\n Content-Length: ", strSize)), "\r\n\r\n");
          send(clientConnection[n], content, strlen(content), 0);
          // Send file
          BUFSIZE_read=read(fd, buffer, BUFSIZE);
          while (BUFSIZE_read > 0)
					{
          	write (clientConnection[n], buffer, BUFSIZE_read);
            BUFSIZE_read=read(fd, buffer, BUFSIZE);
          }
        }
        // File not found
				else
        {
          write(clientConnection[n], "HTTP/1.1 404 File Not Found\n", 28);
        }
      }
		}
	}
	// Close socket and shutdown
	shutdown(clientConnection[n], SHUT_RDWR);
	close(clientConnection[n]);
	clientConnection[n]=-1;
}
