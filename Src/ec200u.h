/*
 * ec200u.h
 *
 *  Created on: 13-Aug-2026
 *      Author: narayanamoorthy.r
 */

#ifndef EC200U_H_
#define EC200U_H_

#include <stdint.h>
#include "platform.h"

//uint8_t ec200u_check_module(volatile uint32_t*, volatile uint32_t*);

uint8_t ec200u_check_module(USART_Handle_t*);

uint8_t ec200u_reset_module(USART_Handle_t*);

uint8_t ec200u_check_sim(USART_Handle_t*);

uint8_t ec200u_check_network(USART_Handle_t*);

uint8_t ec200u_get_imei(USART_Handle_t* ,char*);

uint8_t ec200u_get_operator(USART_Handle_t*,char*);

uint8_t ec200u_get_signal(USART_Handle_t*, uint8_t*);

uint8_t ec200u_get_iccid(USART_Handle_t*, char*);

uint8_t ec200u_get_network_info(USART_Handle_t*, char*);

uint8_t ec200u_get_firmware(USART_Handle_t*, char*);

uint8_t ec200u_set_apn(USART_Handle_t*, const char*);

uint8_t ec200u_activate_pdp(USART_Handle_t*);

uint8_t ec200u_get_ip(USART_Handle_t*, char*);

uint8_t ec200u_set_mqtt_version(USART_Handle_t*);

uint8_t ec200u_mqtt_open(USART_Handle_t*, const char*, uint16_t);

uint8_t ec200u_mqtt_connect(USART_Handle_t*, const char*, const char*, const char*);

uint8_t ec200u_mqtt_publish(USART_Handle_t*, const char*, const char*);

uint8_t ec200u_mqtt_subscribe(USART_Handle_t*, const char* ,uint8_t);

uint8_t ec200u_mqtt_receive(USART_Handle_t*, char*, char*);

uint8_t ec200u_mqtt_disconnect(USART_Handle_t*);

uint8_t ec200u_mqtt_close(USART_Handle_t*);

uint8_t ec200u_send_and_wait(USART_Handle_t*, const char*, char*);

void ec200u_send_cmd(USART_Handle_t*, const char*);

void ec200u_test_with_mqtt();

uint8_t ec200u_init(USART_Handle_t*);

uint8_t ec200u_mqtt_init(USART_Handle_t*);

#endif
