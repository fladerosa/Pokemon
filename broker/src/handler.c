#include "handler.h"

void process_request(uint32_t cod_op, uint32_t size, uint32_t client_fd) {
	switch (cod_op) {
	case MENSAJE:
		process_message(client_fd, size);
		break;
	case 0:
		break;
	case -1:
		break;
	default:
		break;
	}
}

