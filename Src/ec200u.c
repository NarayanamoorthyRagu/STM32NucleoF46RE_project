/*
 * ec200u.c
 *
 *  Created on: 13-Aug-2026
 *      Author: narayanamoorthy.r
 */
#include "ec200u.h"
#include "platform.h"
#include "uart.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char imei[128];
char operator_name[128];
uint8_t Signal;
char iccid[32];
char network[128];
char version[128];
char ip_addr[32];

uint8_t ec200u_check_module(USART_Handle_t *husart)
{
    char response[128];

    // Pass the SR and DR from the handle
    return ec200u_send_and_wait(husart, "AT", response);
}

uint8_t ec200u_reset_module(USART_Handle_t *husart)
{
	char response[128];

	return ec200u_send_and_wait(husart, "AT+CFUN=1,1", response);
}

uint8_t ec200u_check_sim(USART_Handle_t *husart)
{
    char response[128];

    if(ec200u_send_and_wait(husart, "AT+CPIN?", response))
    {
        if(strstr(response, "READY") != NULL)
        {
            return 1U;
        }
    }

    return 0U;
}

uint8_t ec200u_check_network(USART_Handle_t *husart)
{
    char response[128];

    for (uint8_t retry = 0U; retry < 30U; retry++)
    {
        memset(response, 0, sizeof(response));

//        usart_tx_str(usart2, "\r\nChecking network...\r\n");

        if (ec200u_send_and_wait(husart,
                                 "AT+CEREG?",
                                 response))
        {
//            usart_tx_str(usart2, "CEREG RESPONSE:\r\n");
//            usart_tx_str(usart2, response);
//            usart_tx_str(usart2, "\r\n");

            /* Registered on home network */
            if (strstr(response, "+CEREG: 0,1") != NULL)
            {
                usart_tx_str(usart2,
                             "[OK] Registered on home network\r\n");

                return 1U;
            }

            /* Registered while roaming */
            if (strstr(response, "+CEREG: 0,5") != NULL)
            {
                usart_tx_str(usart2,
                             "[OK] Registered while roaming\r\n");

                return 1U;
            }

            /* Searching */
            if (strstr(response, "+CEREG: 0,2") != NULL)
            {
                usart_tx_str(usart2,
                             "[WAIT] Network searching...\r\n");
            }

            /* Not registered */
            else if (strstr(response, "+CEREG: 0,0") != NULL)
            {
                usart_tx_str(usart2,
                             "[WAIT] Not registered yet...\r\n");
            }

            /* Registration denied */
            else if (strstr(response, "+CEREG: 0,3") != NULL)
            {
                usart_tx_str(usart2,
                             "[FAIL] Network registration denied\r\n");

                return 0U;
            }
        }

        /* Wait 2 seconds before checking again */
        tim2_delay(2000);
    }

    usart_tx_str(usart2,
                 "\r\n[FAIL] Network registration timeout\r\n");

    return 0U;
}

uint8_t ec200u_get_imei(USART_Handle_t *husart, char *imei)
{
    char response[128];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(husart, "AT+CGSN", response))
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

uint8_t ec200u_get_operator(USART_Handle_t *husart, char *operator_name)
{
    char response[128];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(husart, "AT+COPS?", response))
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

uint8_t ec200u_get_signal(USART_Handle_t *husart, uint8_t *rssi)
{
    char response[128];
    char *start;

    if(!ec200u_send_and_wait(husart, "AT+CSQ", response))
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

uint8_t ec200u_get_iccid(USART_Handle_t *husart, char *iccid)
{
    char response[128];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(husart, "AT+QCCID", response))
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

uint8_t ec200u_get_network_info(USART_Handle_t *husart, char *network)
{
    char response[256];
    uint32_t index = 0;
    char ch;

    char *start;
    char *end;

    memset(response, 0, sizeof(response));

    ec200u_send_cmd(husart, "AT+QNWINFO");

    /*
     * Receive response
     */
    while (index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(husart);

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
            usart_tx_str(usart2, "\r\nQNWINFO ERROR\r\n");

            return 0U;
        }
    }

    usart_tx_str(usart2, "\r\nQNWINFO RESPONSE TIMEOUT\r\n");

    return 0U;
}

uint8_t ec200u_get_firmware(USART_Handle_t *husart, char *version)
{
    char response[256];
    char *start;

    if(!ec200u_send_and_wait(husart, "ATI", response))
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

uint8_t ec200u_set_apn(USART_Handle_t *husart, const char *apn)
{
    char cmd[128];
    char response[128];

    strcpy(cmd, "AT+QICSGP=1,1,\"");
    strcat(cmd, apn);
    strcat(cmd, "\",\"\",\"\",1");

    return ec200u_send_and_wait(husart, cmd, response);
}

uint8_t ec200u_activate_pdp(USART_Handle_t *husart)
{
    char response[128];

    return ec200u_send_and_wait(husart, "AT+QIACT=1", response);
}

uint8_t ec200u_get_ip(USART_Handle_t *husart, char *ip)
{
    char response[256];
    char *start;
    char *end;

    if(!ec200u_send_and_wait(husart, "AT+QIACT?", response))
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

uint8_t ec200u_set_mqtt_version(USART_Handle_t *husart)
{
    char response[128];

    return ec200u_send_and_wait(husart,  "AT+QMTCFG=\"version\",0,3", response);
}

uint8_t ec200u_mqtt_set_will(USART_Handle_t *husart, const char *topic, const char *message, uint8_t Qos, bool retain)
{
    char cmd[256];
    char response[256];

    snprintf(cmd, sizeof(cmd), "AT+QMTCFG=\"will\",0,1,%u,%u,\"%s\",\"%s\"",Qos, retain, topic, message);

    if (!ec200u_send_and_wait(husart, cmd, response))
    {
        usart_tx_str(usart2, "\r\n[FAIL] MQTT LWT configuration\r\n");
        return 0U;
    }

    usart_tx_str(usart2, "\r\n[OK] MQTT LWT configured\r\n");

    return 1U;
}

uint8_t ec200u_mqtt_open(USART_Handle_t *husart, const char *broker, uint16_t port)
{
    char cmd[128];
    char response[256];

    uint32_t index = 0;
    char ch;

    sprintf(cmd, "AT+QMTOPEN=0,\"%s\",%u", broker, port);

    /* Send AT command */
    ec200u_send_cmd(husart, cmd);

    memset(response, 0, sizeof(response));

    /* Receive response */
    while (index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(husart);

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

    if (qmtopen == NULL)
    {
        usart_tx_str(usart2, "\r\nQMTOPEN response not found\r\n");
    }

    /* Success */
    if (strstr(response, "+QMTOPEN:0,0") != NULL || strstr(response, "+QMTOPEN: 0,0") != NULL)
    {
        return 1U;
    }

    return 0U;
}

uint8_t ec200u_mqtt_connect(USART_Handle_t *husart, const char *client_id, const char *username, const char *password)
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

    /* Send command */
    ec200u_send_cmd(husart, cmd);

    memset(response, 0, sizeof(response));

    /*
     * Receive response
     */
    while (index < sizeof(response) - 1)
    {
        ch = usart_rx_ch(husart);

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
     * Check successful MQTT connection
     * ------------------------------------------------
     */

    if (strstr(response, "+QMTCONN:0,0,0") != NULL ||  strstr(response, "+QMTCONN: 0,0,0") != NULL)
    {

        return 1U;
    }

    /*
     * ------------------------------------------------
     * Connection failed
     * ------------------------------------------------
     */

    usart_tx_str(usart2, "MQTT CONNECTION FAILED\r\n");

    return 0U;
}

uint8_t ec200u_mqtt_publish(USART_Handle_t *husart, const char *topic, const char *message, uint8_t qos, bool retain)
{
    char cmd[256];
    char response[256];

    uint32_t index = 0;
    char ch;

    /* --------------------------------------------------
     * Create QMTPUB command
     * -------------------------------------------------- */

    snprintf(cmd, sizeof(cmd), "AT+QMTPUB=0,0,%u,%u,\"%s\"", qos, retain ? 1U : 0U, topic);
    /* --------------------------------------------------
     * Send QMTPUB command
     * -------------------------------------------------- */

    ec200u_send_cmd(husart, cmd);

    /* --------------------------------------------------
     * Wait for '>'
     * -------------------------------------------------- */

    memset(response, 0, sizeof(response));

    index = 0;

    while (index < sizeof(response) - 1U)
    {
        ch = usart_rx_ch(husart);

        /* Debug */
        //usart_tx_ch(usart2, ch);

        response[index++] = ch;
        response[index] = '\0';

        if (ch == '>')
        {
            break;
        }

        if (strstr(response, "ERROR") != NULL)
        {
            usart_tx_str(usart2, "\r\nQMTPUB COMMAND ERROR\r\n");

            return 0U;
        }
    }

    if (strchr(response, '>') == NULL)
    {
        usart_tx_str(usart2, "\r\nPUBLISH PROMPT NOT RECEIVED\r\n");

        return 0U;
    }

    /* --------------------------------------------------
     * Send payload
     * -------------------------------------------------- */

    usart_tx_str(husart, message);

    /* --------------------------------------------------
     * Send CTRL+Z
     * -------------------------------------------------- */

    usart_tx_ch(husart, 0x1A);

    /* --------------------------------------------------
     * Wait for publish response
     * -------------------------------------------------- */

    memset(response, 0, sizeof(response));

    index = 0;

    tim2_delay(100);

    while (index < sizeof(response) - 1U)
    {
        ch = usart_rx_ch(husart);

        response[index++] = ch;
        response[index] = '\0';

        /*
         * Debug received characters
         */
        //usart_tx_ch(usart2, ch);

        /*
         * Successful publish
         */
        if (strstr(response, "+QMTPUB: 0,0,0") != NULL || strstr(response, "+QMTPUB:0,0,0") != NULL)
        {
            usart_tx_str(usart2, "\r\nMQTT PUBLISH SUCCESS\r\n");

            return 1U;
        }

        /*
         * Error
         */
        if (strstr(response, "ERROR") != NULL)
        {
            usart_tx_str(usart2, "\r\nMQTT PUBLISH ERROR\r\n");

            return 0U;
        }
    }

    usart_tx_str(usart2, "\r\nMQTT PUBLISH RESPONSE TIMEOUT/INVALID\r\n");

    return 0U;
}

uint8_t ec200u_mqtt_subscribe(USART_Handle_t *husart, const char *topic, uint8_t qos)
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

    usart_tx_str(husart, cmd);

    /*
     * Debug message
     */

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
        while (!(*husart->SR & (1U << 5)));

        ch = (char)(*husart->DR);

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

//    usart_tx_str(usart2,"MQTT SUBSCRIBE Response:\r\n");
//    usart_tx_str(usart2, response);

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
        return 1U;
    }

    /*
     * Any other result is failure.
     */
    usart_tx_str(usart2,"MQTT SUBSCRIBE FAILED\r\n");

    return 0U;
}

uint8_t ec200u_mqtt_receive(USART_Handle_t *husart, char *topic, char *message)
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
        ch = usart_rx_ch(husart);

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

uint8_t ec200u_mqtt_disconnect(USART_Handle_t *husart)
{
    char response[128];
    uint32_t index = 0;
    char ch;

    usart_tx_str(husart, "AT+QMTDISC=0\r");

    while (index < (sizeof(response) - 1U))
    {
        ch = usart_rx_ch(husart);

        response[index++] = ch;
        response[index] = '\0';

        if (strstr(response, "+QMTDISC: 0,0") != NULL)
        {
            return 1U;
        }

        if (strstr(response, "ERROR") != NULL)
        {
            break;
        }
    }

    usart_tx_str(usart2, "MQTT DISCONNECT FAILED\r\n");

    return 0U;
}

uint8_t ec200u_mqtt_close(USART_Handle_t *husart)
{
    char response[128];
    uint32_t index = 0;
    char ch;

    usart_tx_str(husart, "AT+QMTCLOSE=0\r");

    while(index < (sizeof(response) - 1U))
    {
        ch = usart_rx_ch(husart);

        response[index++] = ch;
        response[index] = '\0';

        if(strstr(response, "+QMTCLOSE: 0,0") != NULL)
        {
            return 1U;
        }

        if(strstr(response, "ERROR") != NULL)
        {
            break;
        }
    }

    usart_tx_str(usart2, "MQTT CLOSE FAILED\r\n");

    return 0U;
}


uint8_t ec200u_send_and_wait(USART_Handle_t *husart, const char *cmd, char *response)
{
    uint32_t index = 0;
    char ch;

    memset(response, 0, 128);

    ec200u_send_cmd(husart, cmd);

    while(index < 127)
    {
        ch = usart_rx_ch(husart);

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

void ec200u_send_cmd(USART_Handle_t *husart ,const char *cmd)
{
    usart_tx_str(husart, cmd);

    usart_tx_str(husart, "\r\n");
}

void ec200u_test_with_mqtt(){
	while(1)
	{
		/* Reset Module */
		if(!ec200u_reset_module(usart1))
		{
			usart_tx_str(usart2, "\r\nMODEM NOT RESET - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2,  "\r\nMODEM RESET\r\n");
		tim2_delay(5000);

		/* Check AT */
		if(!ec200u_check_module(usart1))
		{
			usart_tx_str(usart2, "\r\nMODEM FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nMODEM OK\r\n");
		tim2_delay(1000);

		/* Check SIM */
		if(!ec200u_check_sim(usart1))
		{
			usart_tx_str(usart2, "\r\nSIM FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nSIM OK\r\n");
		tim2_delay(1000);

		/* Check Network */
		if(!ec200u_check_network(usart1))
		{
			usart_tx_str(usart2, "\r\nNETWORK FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nNETWORK OK\r\n");
		tim2_delay(1000);

		/* IMEI */
		if(!ec200u_get_imei(usart1, imei))
		{
			usart_tx_str(usart2, "\r\nIMEI READ FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nIMEI: ");
		usart_tx_str(usart2, imei);
		usart_tx_str(usart2, "\r\n");
		tim2_delay(1000);

		/* Operator */
		if(!ec200u_get_operator(usart1, operator_name))
		{
			usart_tx_str(usart2, "\r\nOPERATOR FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nOperator: ");
		usart_tx_str(usart2, operator_name);
		usart_tx_str(usart2, "\r\n");
		tim2_delay(1000);

		/* Signal */
		if(!ec200u_get_signal(usart1, &Signal))
		{
			usart_tx_str(usart2, "\r\nSIGNAL FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nSignal: ");
		usart_tx_uint(usart2, Signal);
		tim2_delay(1000);

		/* ICCID */
		if(!ec200u_get_iccid(usart1, iccid))
		{
		    usart_tx_str(usart2, "\r\nICCID FAIL - Retrying...\r\n");
		    continue;
		}

		usart_tx_str(usart2, "\r\nICCID: ");
		usart_tx_str(usart2, iccid);
		usart_tx_str(usart2, "\r\n");
		tim2_delay(1000);

		/* Network Info */
		if(!ec200u_get_network_info(usart1, network))
		{
			usart_tx_str(usart2, "\r\nNETWORK INFO FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nNetwork Info: ");
		usart_tx_str(usart2, network);
		usart_tx_str(usart2, "\r\n");
		tim2_delay(1000);

		/* Firmware */
		if(!ec200u_get_firmware(usart1, version))
		{
			usart_tx_str(usart2, "\r\nFIRMWARE FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nFirmware: ");
		usart_tx_str(usart2, version);
		usart_tx_str(usart2, "\r\n");
		tim2_delay(1000);

		/* APN */
		if(!ec200u_set_apn(usart1, "jionet"))
		{
			usart_tx_str(usart2, "\r\nAPN FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nAPN OK\r\n");
		tim2_delay(1000);

		/* PDP Activate */
		if(!ec200u_activate_pdp(usart1))
		{
			usart_tx_str(usart2, "\r\nPDP FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nPDP ACTIVATED\r\n");
		tim2_delay(1000);

		/* Get IP */
		if(!ec200u_get_ip(usart1, ip_addr))
		{
			usart_tx_str(usart2, "\r\nIP FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nIP Address: ");
		usart_tx_str(usart2, ip_addr);
		usart_tx_str(usart2, "\r\n");
		tim2_delay(1000);

		/* MQTT Version */
		if(!ec200u_set_mqtt_version(usart1))
		{
			usart_tx_str(usart2, "\r\nMQTT VERSION FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nMQTT VERSION SET\r\n");
		tim2_delay(1000);

		/* MQTT Open */
		if(!ec200u_mqtt_open(usart1, "broker.hivemq.com", 1883))
		{
			usart_tx_str(usart2, "\r\nMQTT OPEN FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nMQTT CONNECTION OPENED\r\n");
		tim2_delay(1000);

		char client_id[32];

		srand(12345);   // Seed once during initialization

		uint32_t random_number = 10000U + (rand() % 90000U);

		snprintf(client_id, sizeof(client_id),"stm32%lu", (unsigned long)random_number);

		/* MQTT Connect */
		if(!ec200u_mqtt_connect(usart1, client_id,"",""))
		{
			usart_tx_str(usart2, "\r\nMQTT CONNECT FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nMQTT CONNECTED\r\n");

		tim2_delay(1000);

		if(!ec200u_mqtt_subscribe(usart1, "stm32/command", 0))
		{
			usart_tx_str(usart2, "\r\nMQTT SUBSCRIBE FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nMQTT SUBSCRIBED\r\n");

		if(!ec200u_mqtt_publish(usart1, "stm32/status", "Hello from STM32",1,0))
		{
			usart_tx_str(usart2, "\r\nMQTT PUBLISH FAIL - Retrying...\r\n");
			continue;
		}

		usart_tx_str(usart2, "\r\nMQTT PUBLISHED\r\n");

		if(ec200u_mqtt_disconnect(usart1))
		{
			usart_tx_str(usart2, "\r\nMQTT DISCONNECTED\r\n");
			usart_tx_str(usart2, "\r\n");
		}

		if(ec200u_mqtt_close(usart1))
		{
			usart_tx_str(usart2, "MQTT SOCKET CLOSED\r\n");
			usart_tx_str(usart2, "\r\n");
			break;
		}
		}
}

uint8_t ec200u_init(USART_Handle_t *pUSART)
{
    usart_tx_str(usart2, "\r\n[1] Reset modem...\r\n");

    if (!ec200u_reset_module(pUSART))
    {
        usart_tx_str(usart2, "[FAIL] Reset modem\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Reset modem\r\n");

    /* Wait for modem to boot */
    tim2_delay(5000);


    /******************************************************
     * 2. CHECK AT
     ******************************************************/
    usart_tx_str(usart2, "[2] Check AT...\r\n");

    if (!ec200u_check_module(pUSART))
    {
        usart_tx_str(usart2, "[FAIL] Check AT\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Check AT\r\n");

    tim2_delay(1000);


    /******************************************************
     * 3. CHECK SIM
     ******************************************************/
    usart_tx_str(usart2, "[3] Check SIM...\r\n");

    if (!ec200u_check_sim(pUSART))
    {
        usart_tx_str(usart2, "[FAIL] Check SIM\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Check SIM\r\n");

    tim2_delay(1000);


    /******************************************************
     * 4. CHECK NETWORK
     ******************************************************/
    usart_tx_str(usart2, "[4] Check Network...\r\n");

    if (!ec200u_check_network(pUSART))
    {
        usart_tx_str(usart2, "[FAIL] Check Network\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Network\r\n");


    /******************************************************
     * 5. GET IMEI
     ******************************************************/
    usart_tx_str(usart2, "[5] Get IMEI...\r\n");

    if (!ec200u_get_imei(pUSART, imei))
    {
        usart_tx_str(usart2, "[FAIL] IMEI\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] IMEI: ");
    usart_tx_str(usart2, imei);
    usart_tx_str(usart2, "\r\n");


    /******************************************************
     * 6. GET OPERATOR
     ******************************************************/
    usart_tx_str(usart2, "[6] Get Operator...\r\n");

    if (!ec200u_get_operator(pUSART, operator_name))
    {
        usart_tx_str(usart2, "[FAIL] Operator\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Operator: ");
    usart_tx_str(usart2, operator_name);
    usart_tx_str(usart2, "\r\n");


    /******************************************************
     * 7. GET SIGNAL
     ******************************************************/
    usart_tx_str(usart2, "[7] Get Signal...\r\n");

    if (!ec200u_get_signal(pUSART, &Signal))
    {
        usart_tx_str(usart2, "[FAIL] Signal\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Signal: ");
    usart_tx_uint(usart2, Signal);

    /******************************************************
     * 8. GET ICCID
     ******************************************************/
    usart_tx_str(usart2, "[8] Get ICCID...\r\n");

    if (!ec200u_get_iccid(pUSART, iccid))
    {
        usart_tx_str(usart2, "[FAIL] ICCID\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] ICCID: ");
    usart_tx_str(usart2, iccid);
    usart_tx_str(usart2, "\r\n");

    /******************************************************
     * 9. GET NETWORK INFO
     ******************************************************/
    usart_tx_str(usart2, "[9] Get Network Info...\r\n");

    if (!ec200u_get_network_info(pUSART, network))
    {
        usart_tx_str(usart2, "[FAIL] Network Info\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Network Info: ");
    usart_tx_str(usart2, network);
    usart_tx_str(usart2, "\r\n");


    /******************************************************
     * 10. GET FIRMWARE
     ******************************************************/
    usart_tx_str(usart2, "[10] Get Firmware...\r\n");

    if (!ec200u_get_firmware(pUSART, version))
    {
        usart_tx_str(usart2, "[FAIL] Firmware\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] Firmware: ");
    usart_tx_str(usart2, version);
    usart_tx_str(usart2, "\r\n");


    /******************************************************
     * 11. SET APN
     ******************************************************/
    usart_tx_str(usart2, "[11] Set APN...\r\n");

    if (!ec200u_set_apn(pUSART, "jionet"))
    {
        usart_tx_str(usart2, "[FAIL] APN\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] APN\r\n");


    /******************************************************
     * 12. ACTIVATE PDP
     ******************************************************/
    usart_tx_str(usart2, "[12] Activate PDP...\r\n");

    if (!ec200u_activate_pdp(pUSART))
    {
        usart_tx_str(usart2, "[FAIL] PDP\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] PDP\r\n");


    /******************************************************
     * 13. GET IP
     ******************************************************/
    usart_tx_str(usart2, "[13] Get IP...\r\n");

    if (!ec200u_get_ip(pUSART, ip_addr))
    {
        usart_tx_str(usart2, "[FAIL] IP\r\n");
        return 0;
    }

    usart_tx_str(usart2, "[OK] IP: ");
    usart_tx_str(usart2, ip_addr);
    usart_tx_str(usart2, "\r\n");

    return 1;
}

uint8_t ec200u_mqtt_init(USART_Handle_t *pUSART)
{
    char client_id[256];

    snprintf(client_id, sizeof(client_id), "stm32_%s",&imei[10]);   /* Example only */

    if (!ec200u_set_mqtt_version(pUSART))
        return 0;

    if (!ec200u_mqtt_set_will(pUSART, mqtt_device_topic, "Offline", 0U, true))
        return 0U;

    if (!ec200u_mqtt_open(pUSART,	mqtt_broker,	mqtt_port))
        return 0;

    if (!ec200u_mqtt_connect(pUSART, client_id, "", ""))
        return 0;

    if (!ec200u_mqtt_publish(pUSART, mqtt_device_topic, "Online", 0U, true))
    {
        return 0;
    }

    if (!ec200u_mqtt_subscribe(pUSART,	mqtt_sub_topic, 0))
        return 0;

    return 1;
}
