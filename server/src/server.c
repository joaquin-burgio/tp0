#include "server.h"

int main(void) {
	logger = log_create("tp0.log", "Server", 1, LOG_LEVEL_DEBUG);
	if (logger == NULL) {
		error_show("No se pudo crear el logger");
		abort();
	}

	int fd_escucha = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	
	int fd_conexion = esperar_cliente(fd_escucha);
	log_info(logger, "Cliente conectado");

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(fd_conexion);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(fd_conexion);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_conexion);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			list_destroy_and_destroy_elements(lista, free);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			log_destroy(logger);
			close(fd_conexion);
			close(fd_escucha);
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
