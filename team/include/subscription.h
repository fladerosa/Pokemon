#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include "common_utils.h"
#include <commons/string.h>
#include "configuration.h"
#include <pthread.h>


void listen_to_gameboy();
void establish_broker_connection();

void send_subscription_queue_message_appeared_pokemon(char*, uint32_t, t_log*);
void send_subscription_queue_message_catch_pokemon(char*, uint32_t, t_log*);
void send_subscription_queue_message_caught_pokemon(char*, uint32_t, t_log*);
void send_subscription_queue_message_get_pokemon(char*, uint32_t, t_log*);
void send_subscription_queue_message_localized_pokemon(char*, uint32_t, t_log*);


#endif