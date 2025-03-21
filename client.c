#include <stdio.h>      // Pour printf, fgets
#include <stdlib.h>     // Pour exit
#include <string.h>     // Pour strlen, memset
#include <unistd.h>     // Pour close, read, write
#include <arpa/inet.h>  // Pour les sockets (socket, connect)

#define PORT 9090          // Doit correspondre au port du serveur
#define BUFFER_SIZE 1024   // Taille du buffer

int pendu();

int main() {
    int sock;                        // Descripteur du socket client
    struct sockaddr_in server_address; // Structure pour stocker l'adresse du serveur
    char buffer[BUFFER_SIZE] = {0};    // Buffer pour recevoir la réponse du serveur
    char message[BUFFER_SIZE];         // Buffer pour stocker le message à envoyer

    // 1️⃣ Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("❌ Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // 2️⃣ Configuration de l'adresse du serveur
    server_address.sin_family = AF_INET;      // IPv4
    server_address.sin_port = htons(PORT);    // Convertir le port en format réseau

    // 3️⃣ Convertir l'adresse IP en format binaire et la stocker
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("❌ Adresse invalide");
        exit(EXIT_FAILURE);
    }

    // 4️⃣ Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("❌ Échec de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    printf("💬 Entrez un message : ");

    // 5️⃣ Lecture du message de l'utilisateur
    if (fgets(message, BUFFER_SIZE, stdin) == NULL || strlen(message) <= 1) {
        printf("🔴 Message vide, arrêt du client.\n");
        close(sock);
        return 0;
    }

    if (strcmp(message,"pendu\n")==0) {
        printf("Vous allez jouer au pendu !\n");
        pendu();
    }

    //message[strcspn(message, "\n")] = 0; // Supprimer le saut de ligne

    // 6️⃣ Envoi du message au serveur
    send(sock, message, strlen(message), 0);


    //if (bytes_received > 0) {
     //   buffer[bytes_received] = '\0'; // Ajoute un caractère de fin de chaîne
     //   printf("📩 Réponse du serveur : %s\n", buffer);
   // } else {
    //   printf("🔴 Aucune réponse reçue du serveur.\n");
   // }

    // 8️⃣ Fermeture du socket
    //close(sock);

    return 0;
}

int pendu() {
    int sock;                        // Descripteur du socket client
    struct sockaddr_in server_address; // Structure pour stocker l'adresse du serveur
    char buffer[BUFFER_SIZE] = {0};    // Buffer pour recevoir la réponse du serveur
    char message[BUFFER_SIZE];         // Buffer pour stocker le message à envoyer

    printf("Début du pendu");

    // 7️⃣ Réception de la réponse du serveur
    ssize_t bytes_received = read(sock, buffer, BUFFER_SIZE - 1);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Ajoute un caractère de fin de chaîne
        printf("📩 Réponse du serveur : %s\n", buffer);
    } else {
        printf("🔴 Aucune réponse reçue du serveur.\n");
    }
    
    return 0;
}