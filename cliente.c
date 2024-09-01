#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void conectar_servidor(const char *mensaje) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Inicializa Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("\nError al inicializar Winsock\n");
        return;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("\nError al crear el socket\n");
        WSACleanup();
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Usa WSAStringToAddress para convertir la dirección IP
    struct sockaddr_in sa;
    int sa_len = sizeof(sa);
    if (WSAStringToAddress("127.0.0.1", AF_INET, NULL, (struct sockaddr *)&sa, &sa_len) != 0) {
        printf("\nDirección no válida\n");
        closesocket(sock);
        WSACleanup();
        return;
    }
    serv_addr.sin_addr = sa.sin_addr;

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("\nConexión fallida\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    send(sock, mensaje, strlen(mensaje), 0);
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Respuesta del servidor: %s\n", buffer);

    closesocket(sock);
    WSACleanup();
}

int main() {
    char opcion;
    int longitud;
    char mensaje[BUFFER_SIZE];

    while (1) {
        printf("Selecciona una opcinon:\n");
        printf("1. Generar nombre de usuario\n");
        printf("2. Generar contrasena\n");
        printf("3. Salir\n");
        printf("Opcion: ");
        scanf(" %c", &opcion);

        if (opcion == '3') {
            break;
        }

        if (opcion == '1' || opcion == '2') {
            printf("Ingresa la longitud deseada: ");
            scanf("%d", &longitud);

            if (opcion == '1') {
                sprintf(mensaje, "u%d", longitud);  // 'u' para nombre de usuario
            } else {
                sprintf(mensaje, "p%d", longitud);  // 'p' para contraseña
            }

            conectar_servidor(mensaje);
        } else {
            printf("Opcion no válida\n");
        }
    }

    return 0;
}
