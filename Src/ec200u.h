/*
 * ec200u.h
 *
 *  Created on: 13-Aug-2026
 *      Author: narayanamoorthy.r
 */

#ifndef EC200U_H_
#define EC200U_H_

#include <stdint.h>

uint8_t ec200u_check_module(volatile uint32_t*, volatile uint32_t*);

uint8_t ec200u_reset_module(volatile uint32_t*, volatile uint32_t*);

uint8_t ec200u_check_sim(volatile uint32_t*, volatile uint32_t*);

uint8_t ec200u_check_network(volatile uint32_t*, volatile uint32_t*);

uint8_t ec200u_get_imei(volatile uint32_t*, volatile uint32_t*,char*);

uint8_t ec200u_get_operator(volatile uint32_t*, volatile uint32_t*,char*);

uint8_t ec200u_get_signal(volatile uint32_t*, volatile uint32_t*, uint8_t*);

uint8_t ec200u_get_iccid(volatile uint32_t*, volatile uint32_t*, char*);

uint8_t ec200u_get_network_info(volatile uint32_t*, volatile uint32_t*, char*);

uint8_t ec200u_get_firmware(volatile uint32_t*, volatile uint32_t*, char*);

uint8_t ec200u_set_apn(volatile uint32_t*, volatile uint32_t*, const char*);

uint8_t ec200u_activate_pdp(volatile uint32_t*, volatile uint32_t*);

uint8_t ec200u_get_ip(volatile uint32_t*, volatile uint32_t*, char*);

uint8_t ec200u_set_mqtt_version(volatile uint32_t*, volatile uint32_t*);

uint8_t ec200u_mqtt_open(volatile uint32_t*, volatile uint32_t*, const char*, uint16_t);

uint8_t ec200u_mqtt_connect(volatile uint32_t*, volatile uint32_t*, const char*);

uint8_t ec200u_send_and_wait(volatile uint32_t*, volatile uint32_t*, const char*, char*);

void ec200u_send_cmd(volatile uint32_t*, volatile uint32_t*,const char*);

#endif
