#include "client.h"

int main(void)
{
    // Variables principales
    int conexion;
    char* ip;
    char* puerto;
    char* valor;
    t_log* logger;
    t_config* config;

    // Inicializar sistema de logging
    logger = iniciar_logger();

    // Cargar configuración desde archivo
    config = iniciar_config();
    
    // Leer valores de configuración
    valor = config_get_string_value(config, "CLAVE");
    ip = config_get_string_value(config, "IP");
    puerto = config_get_string_value(config, "PUERTO");
    
    // Mostrar configuración cargada
    log_info(logger, "%s", valor);
    log_info(logger, "IP: %s, Puerto: %s", ip, puerto);

    // Interfaz interactiva con usuario
    leer_consola(logger);

    // Establecer conexión con servidor
    conexion = crear_conexion(ip, puerto);
    
    // Enviar datos al servidor
    paquete(conexion,valor);
    
    // Cleanup y finalización
    terminar_programa(conexion, logger, config);

    return 0;
}

t_log* iniciar_logger(void)
{
    t_log* nuevo_logger;
    
    nuevo_logger = log_create(
        "tp0.log",        // Archivo de logs
        "Cliente",        // Nombre del proceso  
        true,            // Mostrar en consola
        LOG_LEVEL_INFO   // Nivel mínimo de logs
    );
    
    return nuevo_logger;
}

t_config* iniciar_config(void)
{
    t_config* nuevo_config;
    
    nuevo_config = config_create("cliente.config");
    
    // Verificar errores al leer configuración
    if (nuevo_config == NULL) {
        printf("No se pudo leer el archivo cliente.config\n");
        abort();
    }
    
    return nuevo_config;
}

void leer_consola(t_log* logger)
{
    char* leido;
    
    leido = readline("> ");
    
    // Leer líneas hasta recibir string vacío
    while (leido != NULL && strlen(leido) > 0) {
        log_info(logger, "Leí: %s", leido);
        free(leido);  // Liberar memoria de readline
        leido = readline("> ");
    }
    
    // Liberar última línea
    if (leido != NULL) {
        free(leido);
    }
    
    log_info(logger, "Fin de la lectura de consola");
}

void paquete(int conexion, char* valor)
{
    char* leido;
    t_paquete* paquete;
    
    // 1. Enviar valor de CLAVE como mensaje simple
    enviar_mensaje(valor, conexion);  // ❌ Problema: valor no está disponible aquí
    
    // 2. Crear paquete para líneas de consola
    paquete = crear_paquete();
    
    // 3. Leer líneas de consola y agregarlas al paquete
    leido = readline("> ");
    while (leido != NULL && strlen(leido) > 0) {
        agregar_a_paquete(paquete, leido, strlen(leido) + 1);
        free(leido);
        leido = readline("> ");
    }
    
    // 4. Enviar paquete
    enviar_paquete(paquete, conexion);
    
    // 5. Cleanup
    if (leido != NULL) {
        free(leido);
    }
    
    eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
    // Liberar recursos en orden inverso a la creación
    config_destroy(config);
    log_destroy(logger);
    
    if (conexion != 0) {
        close(conexion);
    }
}