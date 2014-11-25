/*
 * fivmr_rtems_juav.c
 *
 *  Created on: Jun 14, 2014
 *      Author: j3
 *
 *
 *
 *
 * Initialise the Ethernet here
 *
 * Create two outgoing sockets
 * Each outgoing socket is hard coded to OTHERS or COMMANDS
 *
 * Create Eleven incoming sockets.
 * Each incoming socket is predefined to a certain message type
 * When the socket is opened, determine if we can pull a string from it - too much work
 *
 * Store the socket information in either a struct, struct array or something
 *
 * When reading, implement a readline method that will read only a single line up until a '\n' char.
 * This can be blocking as required
 *
 * The readline method must take a char[] and the index
 * The write method must take a char[] and will stop at a \n char
 */

#include <stdio.h>
#include <bsp.h>
#include <rtems/rtems_bsdnet.h>
#include <network_interface_add.h>
#include <errno.h>

/*Network configuration*/
static struct rtems_bsdnet_ifconfig netdriver_config = {
		RTEMS_BSP_NETWORK_DRIVER_NAME, RTEMS_BSP_NETWORK_DRIVER_ATTACH, NULL,
		"10.42.0.2", /*IP address*/
		"255.255.255.0", /* IP net mask */
		NULL, 0, 0, 0, 0, 0, 0, 0, NULL };

struct rtems_bsdnet_config rtems_bsdnet_config = { &netdriver_config, /* Network interface */
NULL, /* Use fixed network configuration */
0, /* Default network task priority */
0, /* Default mbuf capacity */
0, /* Default mbuf cluster capacity */
"Leon3", /* Host name */
"Leon3", /* Domain name */
"10.42.0.0", /* Gateway */
"10.42.0.1", /* Log host */
{ "127.0.0.1" }, /* Name server(s) */
{ "127.0.0.1" }, /* NTP server(s) */
0, 0, 0, 0, 0 };

#define WRITE_SOCKETS 2
#define READ_SOCKETS 2 //When set to 15 there is errno 23 file table overflow....
//Fix by some config value but for now only open 10 sockets

struct ethClient {
	int sockfd[WRITE_SOCKETS];
	struct sockaddr_in addr;
};

struct ethServ {
	int sockfd;
	int clientfd[READ_SOCKETS];
	struct sockaddr_in addr;
};

struct ethClient ethConfig_write;
struct ethServ ethConfig_read;

/**********************************************/
//LOCAL BUFFERING
#define RBUF_SIZE    768*2

typedef struct ringBufS {
	char buf[RBUF_SIZE];
	int head;
	int tail;
	int count;
} ringBufS;

ringBufS ethBufRx[READ_SOCKETS];

void ringBufS_init(ringBufS *_this) {
	/*****
	 The following clears:
	 -> buf
	 -> head
	 -> tail
	 -> count
	 and sets head = tail
	 ***/
	memset(_this, 0, sizeof(*_this));
}

unsigned int modulo_inc(const unsigned int value, const unsigned int modulus) {
	unsigned int my_value = value + 1;
	if (my_value >= modulus) {
		my_value = 0;
	}
	return (my_value);
}

void ringBufS_put(ringBufS *_this, char c) {
	if (_this->count < RBUF_SIZE) {
		_this->buf[_this->head] = c;
		_this->head = modulo_inc(_this->head, RBUF_SIZE);
		++_this->count;
	}
}

void ringBusS_putBlock(ringBufS *_this, char* c, int quantity) {
	//Take all of the bytes and put in the circular buffer
	while (quantity--)
		ringBufS_put(_this, *c++);
}

char ringBufS_get(ringBufS *_this) {
	unsigned char c;
	if (_this->count > 0) {
		c = _this->buf[_this->tail];
		_this->tail = modulo_inc(_this->tail, RBUF_SIZE);
		--_this->count;
	} else {
		c = -1;
	}
	return (c);
}

/**********************************************/

void ethernet_write(uint8_t* ptr, int len, int skIdx);
/**
 * Initialises the Ethernet writing sockets for the OTHERS and COMMANDS data
 */
void ethernet_write_init() {
	//Initialise both sockets
	int s = 0;

	for (s = 0; s < WRITE_SOCKETS; s++) {

		ethConfig_write.sockfd[s] = socket(AF_INET, SOCK_STREAM, 0);
		if (ethConfig_write.sockfd[s] < 0) {
			printf("Could not connect socket %d with errno %d \n", s, errno);
			return;
		}

		memset(&ethConfig_write.addr, '0', sizeof(ethConfig_write.addr));
		ethConfig_write.addr.sin_family = AF_INET;
		ethConfig_write.addr.sin_addr.s_addr = inet_addr("10.42.0.1");
		ethConfig_write.addr.sin_port = htons(8081);

		int status = connect(ethConfig_write.sockfd[s],
				(const struct sockaddr *) &ethConfig_write.addr,
				sizeof(struct sockaddr));
		if (status < 0) {
			printf("Could not connect socket %d with errno %d \n", s, errno);
		}
		if (s == 0) {
			uint8_t cmd[] = { 'C', 'O', 'M', 'M', 'A', 'N', 'D', '\n' };
			ethernet_write(&cmd[0], 8, 0);
		}
		if (s == 1) {
			uint8_t oth[] = { 'O', 'T', 'H', 'E', 'R', 'S', '\n' };
			ethernet_write(&oth[0], 7, 1);
		}
//		uint8_t str[] = { 'T', 'E', 'S', 'T', '\n' };
//		ethernet_write(&str[0], 5, s);
		//Set the transmission socket to non-blocking
		if (fcntl(ethConfig_write.sockfd[s], F_SETFL, O_NONBLOCK) < 0) {
			printf("ERROR Setting sockfd to non-blocking");
			return;
		}

		int val = fcntl(ethConfig_write.sockfd[s], F_GETFL, 0);

		if (val >= 0) {
			printf("0x%x :: %d\n", val, val);
		}
	}

}

/**
 * Initialises the Ethernet reading sockets for all of the incoming messages
 */
void ethernet_read_init() {
	int s = 0;

	ethConfig_read.sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (ethConfig_read.sockfd < 0) {
		printf("ERROR opening socket %d\r\n", 0);
		return;
	}

	memset(&ethConfig_read.addr, '0', sizeof(ethConfig_read.addr));
	ethConfig_read.addr.sin_family = AF_INET;
	ethConfig_read.addr.sin_addr.s_addr = INADDR_ANY;
	ethConfig_read.addr.sin_port = htons(8080);

	if (bind(ethConfig_read.sockfd, (struct sockaddr *) &ethConfig_read.addr,
			sizeof(ethConfig_read.addr)) < 0) {
		return;
	}

	listen(ethConfig_read.sockfd, 5);

	for (s = 0; s < READ_SOCKETS; s++) {
		printf("Server waiting for accept .. ");

		ethConfig_read.clientfd[s] = accept(ethConfig_read.sockfd, NULL, NULL);
		if (ethConfig_read.clientfd[s] < 0) {
			printf(" %d failed to connect \n", s);
		} else
			printf("%d connected \n", s);

		//Set the reception socket to non-blocking
		if (fcntl(ethConfig_read.clientfd[s], F_SETFL, O_NONBLOCK) < 0) {
			printf("ERROR Setting clientfd to non-blocking");
			return;
		}

		//Init the ciruclar buffer
		ringBufS_init(&ethBufRx[s]);

	}

	//Set the reception socket to non-blocking
	if (fcntl(ethConfig_read.sockfd, F_SETFL, O_NONBLOCK) < 0) {
		printf("Error setting socket to non-blocking");
		return;
	}

}

/**
 * readline() - read an entire line from a file descriptor until a newline.
 * functions returns the number of characters read but not including the
 * null character.
 **/
//int readline(int fd, char *str, int maxlen) {
//	int n; /* no. of chars */
//	int readcount; /* no. characters read */
//	char c;
//	for (n = 1; n < maxlen; n++) {
/* read 1 character at a time */
//		readcount = read(fd, &c, 1); /* store result in readcount */
//		if (readcount == 1) /* 1 char read? */
//		{
//			*str = c; /* copy character to buffer */
//			str++; /* increment buffer index */
//			if (c == '\n') /* is it a newline character? */
//				break; /* then exit for loop */
//		}
//		else if (readcount == 0) /* no character read? */
//		{
//			if (n == 1) /* no character read? */
//				return (0); /* then return 0 */
//			else
//				break; /* else simply exit loop */
//		} else
//			return (-1); /* error in read() */
//	}
//	*str = 0; /* null-terminate the buffer */
//	return (n); /* return number of characters read */
//}
/*
 * ----------------------------------------------------------------------------
 * readn:
 *   read 'n' bytes or upto EOF from descriptor 'fd' into 'vptr'
 *   returns number of bytes read or -1 on error
 *   our program will be blocked waiting for n bytes to be available on fd
 *
 * fd:   the file descriptor (socket) we're reading from
 * vptr: address of memory space to put read data
 * n:    number of bytes to be read
 * ----------------------------------------------------------------------------
 */
int32_t readn(int fd, void* vptr, int32_t n) {
	int32_t nleft;
	int32_t nread;
	char* ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				nread = 0;
			} else {
				return (-1);
			}
		} else if (nread == 0) {
			break; /* EOF */
		}
		nleft -= nread;
		ptr += nread;
	}

	return (n - nleft);
}

/**
 * readline() - read an entire line from a file descriptor until a newline.
 * functions returns the number of characters read but not including the
 * null character.
 **/
//
//int readline(int fd, char *str, int maxlen) {
//
//	int readcount;
//	char c[10];
//	c[0] = 0;
//
//	readcount = read(fd,&c[0],3);
//
//	if(readcount != 3) return -1;
//
//	c[3] = '\0';
//
//	int messagelength = atoi(c);
//
//	printf("readline msgLength %d\n",messagelength);
//
//	if(messagelength <10 || messagelength > 256) return -1; //Min message size
//
//	int size = readn(fd,str,messagelength);
//	str[size] = '\0';
//
//	return size+1;
//}
int readline(int fd, char *str, int maxlen) {

	int numBytes = read(fd, str, maxlen);

	if (numBytes <= 0) {
		//This is for non-blocking Rx
		if (errno == EWOULDBLOCK) {
			return -1;
		}
		return -1;
	}
	return numBytes;
}

/**
 * Reads a line from the given input socket.
 * This is a blocking method and will return with a '\n' char in the data
 * Reads a single byte each time
 */
int ethernet_readline(char* ptr, int skIdx) {
	char temp_buffer[256];
	int i = 0;

	*ptr = '\0';

	//1: Read from the ethernet port and stuff into buffer
	/**********************************************/
	temp_buffer[0] = '\0';

	int len = readline(ethConfig_read.clientfd[skIdx], temp_buffer, 100);
	if (len > 0) {
		ringBusS_putBlock(&ethBufRx[skIdx], temp_buffer, len);
	}

	//2: Parse through the buffer for a message
	/**********************************************/

	//If the buffer is full, clear it and reset
	if (ethBufRx[skIdx].count == RBUF_SIZE) {
		ringBufS_init(&ethBufRx[skIdx]);
		return -1;
	}

	int flag = 0;
	for (i = 0; i < ethBufRx[skIdx].count; i++) {
		if (ethBufRx[skIdx].buf[(ethBufRx[skIdx].tail + i) % RBUF_SIZE]
				== '\n') {
			//BINGO We have a message, pull it out
			flag = 1;
			break;
		}
		//fixme there is an potential issue here if the \n char gets overwritten by incoming data or lost,
		//fixme two messages can go back to back and blow over the 255 array size. Will cause dropped messages
	}

	if (flag) {
		for (i = 0; i < ethBufRx[skIdx].count; i++) {
			//clamp the index to ensure we don't exceed buffer size
			if (i > 255)
				i = 255;
			//If we are over the buffer size, keep pulling data from the buffer to clear it out
			temp_buffer[i] = ringBufS_get(&ethBufRx[skIdx]);
			if (temp_buffer[i] == '\n') {
				temp_buffer[i + 1] = '\0';
				strcat(ptr, temp_buffer);
				return 1;
			}
		}
	}
	return -1;
}

/**
 * Writes from the buffer up until it encounters a '\n' char
 * Send the data to either OTHERS or COMMANDS socket
 */
void ethernet_write(uint8_t* ptr, int len, int skIdx) {
	uint8_t temp_buffer[len + 1];
	memcpy(temp_buffer, ptr, len);
	int num_bytes = send(ethConfig_write.sockfd[skIdx], temp_buffer, len, 0);
	if (num_bytes <= 0) {
		printf("wr %d\n", num_bytes);
	}
}

/**
 * Initialise all of the ethernet here. Includes the read and write sockets
 */
void ethernet_init() {
	int net_state = 0;
	printf("Initialising the Ethernet here\n");
	net_state = rtems_bsdnet_initialize_network();
	rtems_bsdnet_show_inet_routes();
	rtems_bsdnet_show_mbuf_stats();
	rtems_bsdnet_show_if_stats();
	rtems_bsdnet_show_ip_stats();
	rtems_bsdnet_show_udp_stats();

//	printf("About to init the %d reading sockets\n", READ_SOCKETS);
//	ethernet_read_init();
//	printf("Done\n");
//
//	sleep(2);
//	printf("About to init the OTHERS and COMMANDS write sockets\n");
//	ethernet_write_init();
//	printf("Done\n");
}
