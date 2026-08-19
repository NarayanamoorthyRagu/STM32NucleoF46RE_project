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

uint8_t ec200u_get_network_info(
    volatile uint32_t *SR,
    volatile uint32_t *DR,
    char *network)
{
    char response[256];
    uint32_t index = 0;
    char ch;

    char *start;
    char *end;

    memset(response, 0, sizeof(response));

    /*
     * Send AT+QNWINFO
     */
    usart_tx_str(&USART2_SR,
                 &USART2_DR,
                 "\r\nSENDING QNWINFO...\r\n");

    ec200u_send_cmd(SR, DR, "AT+QNWINFO");

    usart_tx_str(&USART2_SR,
                 &USART2_DR,
                 "QNWINFO COMMAND SENT\r\n");

    /*
     * Receive response
     */
    while (index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        /*
         * QNWINFO response received
         *
         * Example:
         *
         * +QNWINFO: "FDD LTE","404","JIO","LTE BAND 3",1650
         */
        if (strstr(response, "+QNWINFO:") != NULL)
        {
            /*
             * Wait until closing quote of first field
             *
             * +QNWINFO: "FDD LTE"
             *                      ^
             */
            start = strstr(response, "+QNWINFO:");

            if (start != NULL)
            {
                start = strchr(start, '"');

                if (start != NULL)
                {
                    start++;

                    end = strchr(start, '"');

                    if (end != NULL)
                    {
                        /*
                         * Copy network type
                         */
                        strncpy(network,
                                start,
                                end - start);

                        network[end - start] = '\0';

                        /*
                         * Print received network
                         */
                        usart_tx_str(&USART2_SR,
                                     &USART2_DR,
                                     "\r\nQNWINFO Response:\r\n");

                        usart_tx_str(&USART2_SR,
                                     &USART2_DR,
                                     response);

                        usart_tx_str(&USART2_SR,
                                     &USART2_DR,
                                     "\r\n");

                        return 1U;
                    }
                }
            }
        }

        /*
         * ERROR
         */
        if (strstr(response, "ERROR") != NULL)
        {
            usart_tx_str(&USART2_SR,
                         &USART2_DR,
                         "\r\nQNWINFO ERROR\r\n");

            return 0U;
        }
    }

    usart_tx_str(&USART2_SR,
                 &USART2_DR,
                 "\r\nQNWINFO RESPONSE TIMEOUT\r\n");

    return 0U;
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

uint8_t ec200u_mqtt_open(volatile uint32_t *SR, volatile uint32_t *DR, const char *broker, uint16_t port)
{
    char cmd[128];
    char response[256];

    uint32_t index = 0;
    char ch;

    sprintf(cmd, "AT+QMTOPEN=0,\"%s\",%u", broker, port);

    /* Send AT command */
    ec200u_send_cmd(SR, DR, cmd);

    memset(response, 0, sizeof(response));

    /* Receive response */
    while (index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(SR, DR);

        /* Store everything except CR/LF */
        if (ch != '\r' && ch != '\n')
        {
            response[index++] = ch;
            response[index] = '\0';
        }

        /*
         * Wait until complete response is received.
         *
         * Accept both:
         * +QMTOPEN:0,1
         * +QMTOPEN: 0,1
         */
        if (strstr(response, "+QMTOPEN:0,") != NULL || strstr(response, "+QMTOPEN: 0,") != NULL)
        {
            /*
             * We have reached the result.
             * Continue receiving until final digit.
             */
            if (response[index - 1] >= '0' && response[index - 1] <= '9')
            {
                break;
            }
        }
    }

    /* Find QMTOPEN response */
    char *qmtopen = strstr(response, "+QMTOPEN:");

    if (qmtopen != NULL)
    {
        /* Print only QMTOPEN response */
        usart_tx_str(&USART2_SR, &USART2_DR, "\r\n");
        usart_tx_str(&USART2_SR, &USART2_DR, qmtopen);
        usart_tx_str(&USART2_SR, &USART2_DR, "\r\n");
    }
    else
    {
        usart_tx_str(&USART2_SR, &USART2_DR, "\r\nQMTOPEN response not found\r\n");
    }

    /* Success */
    if (strstr(response, "+QMTOPEN:0,0") != NULL || strstr(response, "+QMTOPEN: 0,0") != NULL)
    {
        return 1U;
    }

    return 0U;
}

uint8_t ec200u_mqtt_connect(volatile uint32_t *SR, volatile uint32_t *DR, const char *client_id, const char *username, const char *password)
{
    char cmd[128];
    char response[128];

    uint32_t index = 0;
    char ch;

    uint8_t ok_received = 0U;
    uint8_t qmtconn_received = 0U;
    uint8_t comma_count = 0U;

    /* Create QMTCONN command */
    sprintf(cmd, "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"", client_id, username, password);

    usart_tx_str(&USART2_SR, &USART2_DR, "\r\nSENDING QMTCONN...\r\n");

    /* Send command */
    ec200u_send_cmd(SR, DR, cmd);

    usart_tx_str(&USART2_SR, &USART2_DR, "QMTCONN COMMAND SENT\r\n");

    memset(response, 0, sizeof(response));

    /*
     * Receive response
     */
    while (index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(SR, DR);

        /*
         * ------------------------------------------------
         * STEP 1
         * Wait for OK
         * ------------------------------------------------
         */
        if (ok_received == 0U)
        {
            static uint8_t ok_index = 0U;

            if (ch == 'O')
            {
                ok_index = 1U;
            }
            else if (ch == 'K' && ok_index == 1U)
            {
                ok_received = 1U;
                ok_index = 0U;

                usart_tx_str(&USART2_SR, &USART2_DR, "OK RECEIVED\r\n");
            }
            else
            {
                ok_index = 0U;
            }

            continue;
        }

        /*
         * ------------------------------------------------
         * STEP 2
         * After OK, look for +QMTCONN:
         * ------------------------------------------------
         */

        /*
         * Ignore CR and LF
         */
        if (ch == '\r' || ch == '\n')
        {
            continue;
        }

        /*
         * Store character
         */
        response[index++] = ch;
        response[index] = '\0';

        /*
         * Check +QMTCONN:
         */
        if (strstr(response, "+QMTCONN:") != NULL)
        {
            qmtconn_received = 1U;
        }

        /*
         * ------------------------------------------------
         * Count commas after QMTCONN
         *
         * Expected:
         *
         * +QMTCONN: 0,0,0
         *             ^ ^
         *             | |
         *             1 2
         * ------------------------------------------------
         */
        if (qmtconn_received == 1U)
        {
            if (ch == ',')
            {
                comma_count++;
            }

            /*
             * We need:
             *
             * +QMTCONN: 0,0,0
             *
             * After the second comma, wait for
             * the final result digit.
             */
            if (comma_count >= 2U)
            {
                /*
                 * Final character should be the
                 * third result value.
                 */
                if (ch >= '0' && ch <= '9')
                {
                    /*
                     * Do not break immediately here
                     * because this character could be
                     * the character before the final digit.
                     *
                     * Check the complete string below.
                     */
                }

                /*
                 * Check complete successful response
                 */
                if (strstr(response, "+QMTCONN:0,0,0") != NULL || strstr(response, "+QMTCONN: 0,0,0") != NULL)
                {
                    break;
                }

                /*
                 * Check other complete QMTCONN result.
                 *
                 * Examples:
                 *
                 * +QMTCONN: 0,3,0
                 * +QMTCONN: 0,5,0
                 */
                if (index >= 1U && response[index - 1] >= '0' && response[index - 1] <= '9')
                {
                    break;
                }
            }
        }
    }

    /*
     * ------------------------------------------------
     * Print received response
     * ------------------------------------------------
     */

    usart_tx_str(&USART2_SR, &USART2_DR, "\r\nQMTCONN Response:\r\n");

    usart_tx_str(&USART2_SR, &USART2_DR, response);

    usart_tx_str(&USART2_SR, &USART2_DR, "\r\n");

    /*
     * ------------------------------------------------
     * Check successful MQTT connection
     * ------------------------------------------------
     */

    if (strstr(response, "+QMTCONN:0,0,0") != NULL ||  strstr(response, "+QMTCONN: 0,0,0") != NULL)
    {
        usart_tx_str(&USART2_SR, &USART2_DR, "MQTT CONNECTION SUCCESS\r\n");

        return 1U;
    }

    /*
     * ------------------------------------------------
     * Connection failed
     * ------------------------------------------------
     */

    usart_tx_str(&USART2_SR,  &USART2_DR, "MQTT CONNECTION FAILED\r\n");

    return 0U;
}

uint8_t ec200u_mqtt_publish(volatile uint32_t *SR, volatile uint32_t *DR, const char *topic, const char *message)
{
    char cmd[256];
    char response[256];
    uint32_t index = 0;
    char ch;

    /*
     * Create QMTPUB command
     *
     * client index = 0
     * msg ID       = 0
     * QoS          = 0
     * retain       = 0
     */
    sprintf(cmd, "AT+QMTPUB=0,0,0,0,\"%s\"", topic);

    usart_tx_str(&USART2_SR, &USART2_DR, "\r\nSENDING MQTT PUBLISH...\r\n");

    /* Send command */
    ec200u_send_cmd(SR, DR, cmd);

    usart_tx_str(&USART2_SR, &USART2_DR, "MQTT PUBLISH COMMAND SENT\r\n");

    /*
     * --------------------------------------------------
     * STEP 1:
     * Wait for '>' prompt
     * --------------------------------------------------
     */

    memset(response, 0, sizeof(response));

    index = 0;

    while(index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(SR, DR);

        /*
         * Debug: print received character
         */
        usart_tx_ch(&USART2_SR, &USART2_DR, ch);

        /*
         * Store character
         */
        response[index++] = ch;
        response[index] = '\0';

        /*
         * Payload prompt received
         */
        if(ch == '>')
        {
            usart_tx_str(&USART2_SR, &USART2_DR, "\r\nPUBLISH PROMPT RECEIVED\r\n");

            break;
        }

        /*
         * Command error
         */
        if(strstr(response, "ERROR") != NULL)
        {
            usart_tx_str(&USART2_SR, &USART2_DR, "\r\nMQTT PUBLISH COMMAND ERROR\r\n");

            return 0U;
        }
    }

    /*
     * Make sure '>' was received.
     */
    if(strchr(response, '>') == NULL)
    {
        usart_tx_str(&USART2_SR, &USART2_DR, "\r\nPUBLISH PROMPT NOT RECEIVED\r\n");

        return 0U;
    }

    /*
     * --------------------------------------------------
     * STEP 2:
     * Send MQTT payload
     * --------------------------------------------------
     */

    usart_tx_str(SR, DR, message);

    /*
     * CTRL+Z
     *
     * Indicates end of MQTT payload.
     */
    usart_tx_ch(SR, DR, 0x1A);

    usart_tx_str(&USART2_SR, &USART2_DR, "\r\nMQTT MESSAGE SENT\r\n");

    /*
     * --------------------------------------------------
     * STEP 3:
     * Receive publish result
     * --------------------------------------------------
     */

    memset(response, 0, sizeof(response));

    index = 0;

    while(index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        /*
         * SUCCESS
         *
         * +QMTPUB: 0,0,0
         *
         * or
         *
         * +QMTPUB:0,0,0
         */
        if(strstr(response, "+QMTPUB: 0,0,0") != NULL || strstr(response, "+QMTPUB:0,0,0") != NULL)
        {
            usart_tx_str(&USART2_SR, &USART2_DR, "\r\nMQTT PUBLISH SUCCESS\r\n");

            return 1U;
        }

        /*
         * ERROR
         */
        if(strstr(response, "ERROR") != NULL)
        {
            break;
        }

        /*
         * IMPORTANT:
         *
         * Do NOT break just because
         * "+QMTPUB:" was received.
         *
         * The remaining characters may
         * arrive later.
         */
    }

    /*
     * --------------------------------------------------
     * STEP 4:
     * Print complete response
     * --------------------------------------------------
     */

    usart_tx_str(&USART2_SR, &USART2_DR, "\r\nMQTT PUBLISH Response:\r\n");

    usart_tx_str(&USART2_SR, &USART2_DR, response);

    usart_tx_str(&USART2_SR, &USART2_DR, "\r\n");

    /*
     * Check success one more time.
     */
    if(strstr(response, "+QMTPUB: 0,0,0") != NULL ||  strstr(response, "+QMTPUB:0,0,0") != NULL)
    {
        usart_tx_str(&USART2_SR, &USART2_DR, "MQTT PUBLISH SUCCESS\r\n");

        return 1U;
    }

    return 0U;
}

uint8_t ec200u_mqtt_subscribe(volatile uint32_t *SR, volatile uint32_t *DR, const char *topic, uint8_t qos)
{
    char cmd[128];
    char response[256];
    uint32_t index = 0;
    char ch;

    /*
     * ---------------------------------------------------------
     * 1. Build MQTT subscribe command
     *
     * AT+QMTSUB=<client_id>,<msg_id>,"<topic>",<qos>
     * ---------------------------------------------------------
     */

    snprintf(cmd, sizeof(cmd), "AT+QMTSUB=0,1,\"%s\",%u\r\n", topic, qos);

    usart_tx_str(SR, DR, cmd);

    /*
     * Debug message
     */
    usart_tx_str(&USART2_SR,&USART2_DR, "MQTT SUBSCRIBE COMMAND SENT\r\n");

    /*
     * ---------------------------------------------------------
     * 2. Receive response
     *
     * We need to wait for:
     *
     * OK
     *
     * +QMTSUB: 0,1,0
     *
     * ---------------------------------------------------------
     */

    index = 0;

    while (index < sizeof(response) - 1U)
    {
        /*
         * Wait for RX character
         */
        while (!(*SR & (1U << 5)));

        ch = (char)(*DR);

        response[index++] = ch;
        response[index] = '\0';

        /*
         * Prevent buffer overflow
         */
        if (index >= sizeof(response) - 1U)
        {
            break;
        }

        /*
         * We need the asynchronous +QMTSUB response.
         */
        if (strstr(response, "+QMTSUB:") != NULL)
        {
            /*
             * Wait until complete line is received
             */
            if (ch == '\n')
            {
                break;
            }
        }
    }

    response[index] = '\0';

    usart_tx_str(&USART2_SR,&USART2_DR,"MQTT SUBSCRIBE Response:\r\n");
    usart_tx_str(&USART2_SR,&USART2_DR, response);

    /*
     * ---------------------------------------------------------
     * 3. Check actual MQTT subscription result
     *
     * +QMTSUB: 0,1,0
     *
     *                  ^
     *                  result = 0 -> SUCCESS
     * ---------------------------------------------------------
     */

    if (strstr(response, "+QMTSUB: 0,1,0") != NULL)
    {
    	usart_tx_str(&USART2_SR,&USART2_DR,"MQTT SUBSCRIBE SUCCESS\r\n");
    	usart_tx_str(&USART2_SR,&USART2_DR,topic);
        return 1U;
    }

    /*
     * Any other result is failure.
     */
    usart_tx_str(&USART2_SR,&USART2_DR,"MQTT SUBSCRIBE FAILED\r\n");

    return 0U;
}

uint8_t ec200u_mqtt_receive(volatile uint32_t *SR, volatile uint32_t *DR, char *topic, char *message)
{
    char response[512];
    char *start;
    char *end;
    uint32_t index = 0;
    char ch;

    memset(response, 0, sizeof(response));

    /*
     * Wait for:
     *
     * +QMTRECV: 0,0,"topic","message"
     *
     */

    while (index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        /*
         * MQTT receive indication received
         */
        if (strstr(response, "+QMTRECV:") != NULL)
        {
            /*
             * Wait until end of line
             */
            if (ch == '\n')
            {
                break;
            }
        }
    }

    /*
     * Find QMTRECV
     */
    start = strstr(response, "+QMTRECV:");

    if (start == NULL)
    {
        return 0U;
    }

    /*
     * Find first quote
     *
     * +QMTRECV: 0,0,"stm32/command","LEDON"
     *                  ^
     */
    start = strchr(start, '"');

    if (start == NULL)
    {
        return 0U;
    }

    start++;

    /*
     * Find ending quote of topic
     */
    end = strchr(start, '"');

    if (end == NULL)
    {
        return 0U;
    }

    /*
     * Copy topic
     */
    strncpy(topic, start, end - start);
    topic[end - start] = '\0';

    /*
     * Move after topic closing quote
     */
    start = end + 1;

    /*
     * Find opening quote of message
     */
    start = strchr(start, '"');

    if (start == NULL)
    {
        return 0U;
    }

    start++;

    /*
     * Find closing quote of message
     */
    end = strchr(start, '"');

    if (end == NULL)
    {
        return 0U;
    }

    /*
     * Copy message
     */
    strncpy(message, start, end - start);
    message[end - start] = '\0';

    return 1U;
}

uint8_t ec200u_mqtt_disconnect(volatile uint32_t *SR, volatile uint32_t *DR)
{
    char response[128];
    uint32_t index = 0;
    char ch;

    usart_tx_str(&USART2_SR, &USART2_DR, "SENDING MQTT DISCONNECT...\r\n");

    usart_tx_str(SR, DR, "AT+QMTDISC=0\r");

    while (index < (sizeof(response) - 1U))
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        if (strstr(response, "+QMTDISC: 0,0") != NULL)
        {
            usart_tx_str(&USART2_SR, &USART2_DR, "MQTT DISCONNECT SUCCESS\r\n");

            return 1U;
        }

        if (strstr(response, "ERROR") != NULL)
        {
            break;
        }
    }

    usart_tx_str(&USART2_SR, &USART2_DR, "MQTT DISCONNECT FAILED\r\n");

    return 0U;
}

uint8_t ec200u_mqtt_close(volatile uint32_t *SR, volatile uint32_t *DR)
{
    char response[128];
    uint32_t index = 0;
    char ch;

    usart_tx_str(&USART2_SR, &USART2_DR, "SENDING MQTT CLOSE...\r\n");

    usart_tx_str(SR, DR, "AT+QMTCLOSE=0\r");

    while(index < (sizeof(response) - 1U))
    {
        ch = usart_rx_ch(SR, DR);

        response[index++] = ch;
        response[index] = '\0';

        if(strstr(response, "+QMTCLOSE: 0,0") != NULL)
        {
            usart_tx_str(&USART2_SR, &USART2_DR, "MQTT CLOSE SUCCESS\r\n");

            return 1U;
        }

        if(strstr(response, "ERROR") != NULL)
        {
            break;
        }
    }

    usart_tx_str(&USART2_SR, &USART2_DR, "MQTT CLOSE FAILED\r\n");

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

        //usart_tx_ch(&USART2_SR, &USART2_DR, ch);

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
