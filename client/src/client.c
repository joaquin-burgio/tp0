#include "client.h"

int main(void)
{
	t_log *logger = log_create("tp0.log", "Client", 1, LOG_LEVEL_DEBUG);
	if (logger == NULL)
	{
		error_show("No se pudo crear el logger");
		abort();
	}

	t_config *config = config_create("cliente.config");
	if (config == NULL)
	{
		log_error(logger, "No se pudo abrir el archivo de configuracion");
		abort();
	}

	char *ip = config_get_string_value(config, "IP");
	char *puerto = config_get_string_value(config, "PUERTO");
	char *clave = config_get_string_value(config, "CLAVE");
	log_info(logger, "IP: %s - Puerto: %s - Valor: %s", ip, puerto, clave);

	int fd_conexion = crear_conexion(ip, puerto);
	log_info(logger, "Conexion establecida con el servidor");

	enviar_mensaje(clave, fd_conexion);

	paquete(fd_conexion, logger);

	terminar_programa(fd_conexion, logger, config);
}

void paquete(int fd_conexion, t_log *logger)
{
	char *leido;
	t_paquete *paquete = crear_paquete(); 

	leido = readline("> ");
	while (leido != NULL && strlen(leido) > 0)
	{
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> ");
	}
	free(leido);

	enviar_paquete(paquete, fd_conexion);
	
	eliminar_paquete(paquete);
}

void terminar_programa(int fd_conexion, t_log *logger, t_config *config)
{
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(fd_conexion);
}
