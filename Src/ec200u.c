/*
 * ec200u.c
 *
 *  Created on: 13-Aug-2026
 *      Author: narayanamoorthy.r
 */
#include "ec200u.h"
#include "platform.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t ec200u_check_module(volatile uint32_t *SR, volatile uint32_t *DR)
{
	char response[128];

	return ec200u_send_and_wait(SR, DR, "AT", response);
}

uint8_t ec200u_reset_module(volatile uint32_t *SR, volatile uint32_t *DR)
{
	char response[128];

	return ec200u_send_and_wait(SR, DR, "AT+CFUN=1,1", response);
}

uint8_t ec200u_check_sim(volatile uint32_t *SR, volatile uint32_t *DR)
{
    char response[128];

    if(ec200u_send_and_wait(SR, DR, "AT+CPIN?", response))
    {
        if(strstr(response, "READY") != NULL)
        {
            return 1U;
        }
    }

    return 0U;
}

uint8_t ec200u_check_network(volatile uint32_t *SR, volatile uint32_t *DR)
{
    char response[128];

    if(ec200u_send_and_wait(SR, DR, "AT+CREG?", response))
    {
        if((strstr(response, "+CREG: 0,1") != NULL) || (strstr(response, "+CREG: 0,5") != NULL))
        {
            return 1U;
        }
    }

    return 0U;
}

uint8_t ec200u_get_imei(volatile uint32_t *SR, volatile uint32_t *DR, char *imei)
{
    char response[128];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(SR, DR, "AT+CGSN", response))
    {
        return 0U;
    }

    start = strstr(response, "AT+CGSN");

    if(start == NULL)
    {
        return 0U;
    }

    start = strchr(start, '\n');

    if(start == NULL)
    {
        return 0U;
    }

    start++;

    while(*start == '\r' || *start == '\n')
    {
        start++;
    }

    end = strstr(start, "\r\n");

    if(end == NULL)
    {
        return 0U;
    }

    strncpy(imei, start, end - start);
    imei[end - start] = '\0';

    return 1U;
}

uint8_t ec200u_get_operator(volatile uint32_t *SR, volatile uint32_t *DR, char *operator_name)
{
    char response[128];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(SR, DR, "AT+COPS?", response))
    {
        return 0U;
    }

    start = strchr(response, '"');

    if(start == NULL)
    {
        return 0U;
    }

    start++;

    end = strchr(start, '"');

    if(end == NULL)
    {
        return 0U;
    }

    strncpy(operator_name, start, end - start);
    operator_name[end - start] = '\0';

    return 1U;
}

uint8_t ec200u_get_signal(volatile uint32_t *SR, volatile uint32_t *DR, uint8_t *rssi)
{
    char response[128];
    char *start;

    if(!ec200u_send_and_wait(SR, DR, "AT+CSQ", response))
    {
        return 0U;
    }

    start = strstr(response, "+CSQ:");

    if(start == NULL)
    {
        return 0U;
    }

    start += 6;

    while(*start == ' ')
    {
        start++;
    }

    *rssi = (uint8_t)atoi(start);

    return 1U;
}

uint8_t ec200u_get_iccid(volatile uint32_t *SR, volatile uint32_t *DR, char *iccid)
{
    char response[128];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(SR, DR, "AT+QCCID", response))
    {
        return 0U;
    }

    start = strstr(response, "+QCCID:");

    if(start == NULL)
    {
        return 0U;
    }

    start += 7;

    while(*start == ' ')
    {
        start++;
    }

    end = strstr(start, "\r\n");

    if(end == NULL)
    {
        return 0U;
    }

    strncpy(iccid, start, end - start);

    iccid[end - start] = '\0';

    return 1U;
}

uint8_t ec200u_get_network_info(volatile uint32_t *SR, volatile uint32_t *DR, char *network)
{
    char response[256];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(SR, DR, "AT+QNWINFO", response))
    {
        return 0U;
    }

    start = strchr(response, '"');

    if(start == NULL)
    {
        return 0U;
    }

    start++;

    end = strchr(start, '"');

    if(end == NULL)
    {
        return 0U;
    }

    strncpy(network, start, end - start);
    network[end - start] = '\0';

    return 1U;
}

uint8_t ec200u_get_firmware(volatile uint32_t *SR, volatile uint32_t *DR, char *version)
{
    char response[256];
    char *start;

    if(!ec200u_send_and_wait(SR, DR, "ATI", response))
    {
        return 0U;
    }

    start = strstr(response, "Revision:");

    if(start == NULL)
    {
        return 0U;
    }

    start += strlen("Revision:");

    while(*start == ' ')
    {
        start++;
    }

    strcpy(version, start);

    char *end = strstr(version, "\r\n");

    if(end != NULL)
    {
        *end = '\0';
    }

    return 1U;
}

uint8_t ec200u_set_apn(volatile uint32_t *SR, volatile uint32_t *DR, const char *apn)
{
    char cmd[128];
    char response[128];

    strcpy(cmd, "AT+QICSGP=1,1,\"");
    strcat(cmd, apn);
    strcat(cmd, "\",\"\",\"\",1");

    return ec200u_send_and_wait(SR, DR, cmd, response);
}

uint8_t ec200u_activate_pdp(volatile uint32_t *SR, volatile uint32_t *DR)
{
    char response[128];

    return ec200u_send_and_wait(SR, DR, "AT+QIACT=1", response);
}

uint8_t ec200u_get_ip(volatile uint32_t *SR, volatile uint32_t *DR, char *ip)
{
    char response[256];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(SR, DR, "AT+QIACT?", response))
    {
        return 0;
    }

    start = strchr(response, '"');

    if(start == NULL)
    {
        return 0;
    }

    start++;

    end = strchr(start, '"');

    if(end == NULL)
    {
        return 0;
    }

    strncpy(ip, start, end - start);

    ip[end - start] = '\0';

    return 1;
}

uint8_t ec200u_set_mqtt_version(volatile uint32_t *SR, volatile uint32_t *DR)
{
    char response[128];

    return ec200u_send_and_wait(SR, DR, "AT+QMTCFG=\"version\",0,3", response);
}

uint8_t ec200u_mqtt_open(volatile uint32_t *SR,
                         volatile uint32_t *DR,
                         const char *broker,
                         uint16_t port)
{
    char cmd[128];
    char response[256];
    uint32_t index = 0;
    char ch;

    sprintf(cmd,
            "AT+QMTOPEN=0,\"%s\",%u",
            broker,
            port);

    ec200u_send_cmd(SR, DR, cmd);

    memset(response, 0, sizeof(response));

    while(index < sizeof(response)-1)
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        usart_tx_ch(&USART2_SR,&USART2_DR,ch);

        if(strstr(response, "+QMTOPEN: 0,0"))
        {
            return 1U;
        }

        if(strstr(response, "+QMTOPEN:"))
        {
            return 0U;
        }
    }

    return 0U;
}
uint8_t ec200u_mqtt_connect(volatile uint32_t *SR, volatile uint32_t *DR, const char *client_id)
{
    char cmd[128];
    char response[256];
    uint32_t index = 0;
    char ch;

    sprintf(cmd,
            "AT+QMTCONN=0,\"%s\"",
            client_id);

    ec200u_send_cmd(SR, DR, cmd);

    memset(response, 0, sizeof(response));

    while(index < sizeof(response)-1)
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        usart_tx_ch(&USART2_SR,&USART2_DR,ch);

        if(strstr(response, "+QMTCONN: 0,0,0"))
        {
            return 1U;
        }

        if(strstr(response, "+QMTCONN:"))
        {
            return 0U;
        }

        if(strstr(response, "ERROR"))
        {
            return 0U;
        }
    }

    return 0U;
}
uint8_t ec200u_send_and_wait(volatile uint32_t *SR, volatile uint32_t *DR, const char *cmd, char *response)
{
    uint32_t index = 0;
    char ch;

    memset(response, 0, 128);

    ec200u_send_cmd(SR, DR, cmd);

    while(index < 127)
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        if(strstr(response, "OK") != NULL)
        {
            return 1U;
        }

        if(strstr(response, "ERROR") != NULL)
        {
            return 0U;
        }
    }

    return 0U;
}
void ec200u_send_cmd(volatile uint32_t *SR, volatile uint32_t *DR,const char *cmd)
{
    usart_tx_str(SR, DR, cmd);

    usart_tx_str(SR, DR, "\r\n");
}
