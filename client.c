#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 1024

void remove_newline(char *str) {
    str[strcspn(str, "\n")] = 0;  // Supprime le '\n' à la fin de la chaîne
}

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    // Création du socket client
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("❌ Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("❌ Adresse invalide");
        exit(EXIT_FAILURE);
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("❌ Échec de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    // Lire un message à envoyer
    printf("💬 Entrez un message : ");
    if (fgets(message, BUFFER_SIZE, stdin) == NULL || strlen(message) <= 1) {
        printf("🔴 Message vide, arrêt du client.\n");
        close(sock);
        return 0;
    }

    // Supprimer le '\n' à la fin du message
    remove_newline(message);

    // Envoi du message au serveur
    send(sock, message, strlen(message), 0);

    // Attente de la réponse du serveur (affichage du mot caché par exemple)
    ssize_t bytes_received = read(sock, buffer, BUFFER_SIZE - 1);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("📩 Réponse du serveur : %s\n", buffer);
    } else {
        printf("🔴 Aucune réponse reçue du serveur.\n");
    }

    close(sock);
    return 0;
}
