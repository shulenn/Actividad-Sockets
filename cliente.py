import socket

def conectar_servidor(mensaje):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect(('127.0.0.1', 8080))
        client_socket.sendall(mensaje.encode())
        respuesta = client_socket.recv(1024).decode()
        print(f"Respuesta del servidor: {respuesta}")

def main():
    while True:
        print("Selecciona una opcion:")
        print("1. Generar nombre de usuario")
        print("2. Generar contrasena")
        print("3. Salir")
        opcion = input("Opcion: ")

        if opcion == '3':
            print("Saliendo del programa...")
            break  # Salir del bucle y terminar el programa

        if opcion in ['1', '2']:
            longitud = input("Ingresa la longitud deseada: ")

            if opcion == '1':
                mensaje = f"u{longitud}"  # 'u' para nombre de usuario
            else:
                mensaje = f"p{longitud}"  # 'p' para contraseña

            conectar_servidor(mensaje)
        else:
            print("Opcion no valida")

if __name__ == "__main__":
    main()
