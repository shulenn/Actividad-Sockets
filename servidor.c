#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>  // Para funciones adicionales como InetPton
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Funciones de utilidad
char random_vocal() {
    char vocales[] = "aeiou";
    return vocales[rand() % 5];
}

char random_consonante() {
    char consonantes[] = "bcdfghjklmnpqrstvwxyz";
    return consonantes[rand() % 21];
}

char random_alphanumeric() {
    char alphanumeric[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    return alphanumeric[rand() % 62];
}

void generar_nombre_usuario(int longitud, char *resultado) {
    int i;
    int usar_vocal = rand() % 2;  // 0 para consonante, 1 para vocal
    for (i = 0; i < longitud; i++) {
        if (usar_vocal) {
            resultado[i] = random_vocal();
        } else {
            resultado[i] = random_consonante();
        }
        usar_vocal = !usar_vocal;  // Alterna entre vocal y consonante
    }
    resultado[longitud] = '\0';
}

void generar_contrasena(int longitud, char *resultado) {
    int i;
    for (i = 0; i < longitud; i++) {
        resultado[i] = random_alphanumeric();
    }
    resultado[longitud] = '\0';
}

void procesar_cliente(SOCKET new_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE] = {0};
    int length;

    // Obtener la dirección IP del cliente
    char cliente_ip[INET_ADDRSTRLEN];
    char cliente_port[6];
    DWORD client_addr_size = sizeof(client_addr);
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = client_addr.sin_addr.s_addr;
    addr.sin_port = client_addr.sin_port;

    // Convertir la dirección IP y el puerto del cliente a cadena
    if (WSAAddressToString((LPSOCKADDR)&addr, sizeof(addr), NULL, cliente_ip, &client_addr_size) == SOCKET_ERROR) {
        printf("Error al convertir la dirección IP: %d\n", WSAGetLastError());
        strcpy(cliente_ip, "Desconocido");
    }
    snprintf(cliente_port, sizeof(cliente_port), "%d", ntohs(client_addr.sin_port));

    // Leer datos del cliente
    recv(new_socket, buffer, BUFFER_SIZE, 0);
    char tipo = buffer[0];
    length = atoi(buffer + 1);

    char respuesta[BUFFER_SIZE];

    if (tipo == 'u') {  // Generador de nombres de usuario
        if (length < 5 || length > 15) {
            sprintf(respuesta, "Error: La longitud del nombre de usuario debe ser entre 5 y 15.");
        } else {
            generar_nombre_usuario(length, respuesta);
            printf("Cliente %s:%s solicito un nombre de usuario.\n", cliente_ip, cliente_port);
            printf("Nombre de usuario generado: %s\n", respuesta); // Muestra el nombre de usuario en el servidor
        }
    } else if (tipo == 'p') {  // Generador de contraseñas
        if (length < 8 || length > 50) {
            sprintf(respuesta, "Error: La longitud de la contraseña debe ser entre 8 y 50.");
        } else {
            generar_contrasena(length, respuesta);
            printf("Cliente %s:%s solicito una contrasena.\n", cliente_ip, cliente_port);
            printf("Contrasena generada: %s\n", respuesta); // Muestra la contraseña en el servidor
        }
    } else {
        sprintf(respuesta, "Error: Opción no válida.");
    }

    // Enviar respuesta al cliente
    send(new_socket, respuesta, strlen(respuesta), 0);
    closesocket(new_socket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address, client_addr;
    int addrlen = sizeof(address);

    // Inicializa Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Fallo en WSAStartup.\n");
        return 1;
    }

    // Crea el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Fallo en socket: %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configuración de la dirección del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind al puerto
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Error en bind: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Escucha por conexiones
    if (listen(server_fd, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Error en listen: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Servidor en espera de conexiones...\n");

    while (1) {
        int client_len = sizeof(client_addr);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&client_len)) == INVALID_SOCKET) {
            printf("Error en accept: %d\n", WSAGetLastError());
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        procesar_cliente(new_socket, client_addr);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
