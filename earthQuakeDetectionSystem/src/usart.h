/*
 * usart.h
 *
 *  Created on: Jun 28, 2015
 *      Author: harsh
 */

#ifndef USART_H_
#define USART_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

void init_USART();
//send character when Tx is ready to send
void send_USART_byte(unsigned char u8Data);
//send string with CR and NL
void send_USART_stringRN(char *data);
void send_USART_stringR(char *data);
//receive one byte at a time
char receive_USART_byte();
//receive bytes till the expected string is received
int receive_USART_string(char *data, char *cmp_str);
//initialize wifi with AT commands
void init_wifi();
char wait_for_client();
void wait_for_close(char id);
void start_server();
void connect_client(char *ssid, char *pass);
void send_HTTP_request(float data);
//send data to client
void send_USART_data(char id, unsigned int* data);
void get_HTML_text(unsigned int* data, char* text, char *num);
char* concat(char *s1, char *s2);
void concat_c(char *s1, char c);

void init_USART() {
	//set baud rate
	UBRRH = 0x00; //baud rate -- 9600 (0.2% error for 16MHz)
	UBRRL = 0x67;
	//async mode, 8 bit data, no parity, 1 stop bit
	UCSRC |= (1<<URSEL) | (1 << UCSZ1) | (1<<UCSZ0);

	//enable transmit - receive
	UCSRB = (1<<TXEN) | (1<<RXEN);
}

//send character when Tx is ready to send
void send_USART_byte(unsigned char u8Data) {
	//wait while previous byte is completed
	while(!(UCSRA & (1<<UDRE))); //UDRE -- data buffer empty flag in UCSRA reg

	// Transmit data
	UDR = u8Data;
}

//send string with CR and NL
void send_USART_stringRN(char *data) {
	while(*data > 0) {
		send_USART_byte(*data++);
	}
	send_USART_byte(0x0D);
	send_USART_byte(0x0A);
}

//send string with CR
void send_USART_stringR(char *data) {
	while(*data > 0) {
		send_USART_byte(*data++);
	}
	send_USART_byte(0x0D); //r
}

//receive one byte at a time
char receive_USART_byte() {
	// Wait for byte to be received
	while(!(UCSRA & (1<<RXC)) );

	// Return received data
	return UDR;
}

//receive bytes till the expected string is received
int receive_USART_string(char *data, char *cmp_str) {
	uint8_t i = 0;
	char *tmp = "";
	int len = strlen(cmp_str);
	while((strstr(tmp, cmp_str) == NULL) && (strstr(tmp, "ERROR") == NULL)
			&& (strstr(tmp, "ALREADY CONNECTED") == NULL)) {
		if(i >= 99)
			i = 0;
		data[i] = receive_USART_byte();

		if(i >= len)
			tmp = data - len;
		i++;
	}
	int success = ((strstr(data, cmp_str) != NULL) ||
			(strstr(data, "ALREADY CONNECTED") != NULL)) ? 1 : 0;
	return success;
}

//initialize wifi with AT commands
void init_wifi() {
	char s[100];

	send_USART_stringRN("AT+RST");
	//receive_USART_string(s, "ready");
	_delay_ms(5000);

	send_USART_stringRN("AT+CWMODE=1");
	receive_USART_string(s, "OK");
	PORTC |= 0x04;
	_delay_ms(5000);

}

void start_server() {
	char s[100];
	send_USART_stringRN("AT+CIPSERVER=1,80");
	receive_USART_string(s, "OK");
	_delay_ms(100);
}

void connect_client(char *ssid, char *pass) {
	char s[100];
	int success = 0;
	while(!success){
		send_USART_stringRN("AT+CWJAP=\"dlink\",\"\"");
		success = receive_USART_string(s, "OK");
		_delay_ms(3000);
	}
	_delay_ms(3000);
}

void send_HTTP_request(float data) {
	char s[100];

	int success = 0;
	//while(!success){
		send_USART_stringRN("AT+CIPSTART=\"TCP\",\"192.168.0.102\",1111");
	//	success = receive_USART_string(s, "OK");
	_delay_ms(100);
	//}
	PORTC |= 0x02;

	char d[100] = "GET /?g_mag=", send[100] = "AT+CIPSEND=";
	char num[4], len_s[2];
	sprintf(num, "%i", (int)data);
	strcat(d, num);
	strcat(d, " HTTP/1.1");

	//convert int to string
	int len = strlen(d) + 4;
	sprintf(len_s, "%i", len);
	strcat(send, len_s);

	send_USART_stringRN(send);
	_delay_ms(200);
	send_USART_stringRN(d);
	send_USART_byte(0x0D);
	send_USART_byte(0x0A);
	_delay_ms(100);
}
char wait_for_client() {
	char s[100];
	receive_USART_string(s, "+IPD,");
	char id = receive_USART_byte();
	receive_USART_string(s, "GET / HTTP/1.1");
	return id;
}

void wait_for_close(char id) {
	char s[100];
	receive_USART_string(s, "CLOSED");
}

void send_USART_data(char id, uint16_t* data) {
	char text[1000], s[100], num[4];

	get_HTML_text(data, text, num);

	char cmd[100] = "AT+CIPSEND=";
	concat_c(cmd, id);
	strcat(cmd,",");
	strcat(cmd, num);
	send_USART_stringRN(cmd);
	receive_USART_string(s, ">");
	_delay_ms(100);
	send_USART_stringRN(text);
	receive_USART_string(s, "SEND OK");
	_delay_ms(1000);
	char close[100] = "AT+CIPCLOSE=5";
	concat_c(close, id);
	send_USART_stringRN("AT+CIPCLOSE=5");
}

char* concat(char *s1, char *s2){
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	char *result = (char*)malloc(len1+len2+1);//+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	memcpy(result, s1, len1);
	memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
	//printf("%i %i %s\n", len1, len2, result);
	return result;
}

void concat_c(char *s1, char c) {
	int len = strlen(s1);
	s1[len] = c;
}


void get_HTML_text(unsigned int* data, char* text, char *num) {
	char msg1[4], msg2[4];
	sprintf(msg1, "%u", data[0]);
	sprintf(msg2, "%u", data[1]);

	char *tmpStr;
	tmpStr = concat("<h1>Earthquake data</h1><h2>", msg1);
	tmpStr = concat(tmpStr, " ");
	tmpStr = concat(tmpStr, msg2);
	tmpStr = concat(tmpStr, "</h2>");
	strcpy(text, tmpStr);
	int len = strlen(text);
	sprintf(num, "%i", len);
}


#endif /* USART_H_ */
