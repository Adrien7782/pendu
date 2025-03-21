#include <stdio.h>      // Pour printf, perror
#include <stdlib.h>     // Pour exit
#include <string.h>     // Pour memset, strlen
#include <unistd.h>     // Pour close, read, write
#include <arpa/inet.h>  // Pour les sockets (socket, bind, listen, accept)
#include <ctype.h>      // Pour la fonction toupper (mettre en majuscule)

#define PORT 9090          // Port d'écoute du serveur
#define BUFFER_SIZE 1024   // Taille du buffer pour les messages

int pendu();
// Fonction qui convertit une chaîne de caractères en majuscules
void to_uppercase(char *str) {
    while (*str) {
        *str = toupper(*str);
        str++; // Avance d'un caractère
    }
}

int main() {
    int server_fd, new_socket;         // Descripteurs de socket
    struct sockaddr_in address;        // Structure pour stocker l'adresse du serveur
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};    // Buffer pour stocker le message reçu

    // 1️⃣ Création du socket serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("❌ Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // 2️⃣ Configuration de l'adresse du serveur
    address.sin_family = AF_INET;          // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Accepter les connexions sur toutes les interfaces réseau
    address.sin_port = htons(PORT);        // Convertir le port en format réseau

    // 3️⃣ Associer le socket à l'adresse (bind)
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("❌ Erreur lors du bind");
        exit(EXIT_FAILURE);
    }

    // 4️⃣ Mettre le socket en mode écoute
    if (listen(server_fd, 3) < 0) {
        perror("❌ Erreur lors de l'écoute");
        exit(EXIT_FAILURE);
    }

    printf("🔵 Serveur en attente de connexion...\n");

    // 5️⃣ Attente d'une connexion client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len)) < 0) {
        perror("❌ Erreur lors de l'acceptation de la connexion");
        exit(EXIT_FAILURE);
    }

    printf("🟢 Client connecté !\n");

    // 6️⃣ Lecture du message envoyé par le client
    ssize_t bytes_received;
    while ((bytes_received = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_received] = '\0';  // Ajoute un caractère de fin de chaîne
        printf("📩 Message reçu : %s\n", buffer);

        if (strcmp(buffer,"pendu\n") == 0){
            pendu();
        }

        memset(buffer, 0, BUFFER_SIZE);  // Réinitialisation du buffer
    }

    // 9️⃣ Gestion de la fin de connexion
    if (bytes_received == 0) {
        printf("🔴 Client déconnecté.\n");
    } else {
        perror("❌ Erreur de lecture");
    }

    // 🔟 Fermeture des sockets
    close(new_socket);
    close(server_fd);

    return 0;
}
 
int pendu(){
    int server_fd, new_socket;         // Descripteurs de socket
    //char buffer[BUFFER_SIZE] = {0};    // Buffer pour stocker le message reçu


    //faire un tableau avec plusieurs mots et un systèmes aléatoire pour en choisir un.
    char mot[] = "chien";
    char devine[strlen(mot) + 1]; // +1 pour le caractère nul '\0'

    //création du la chaine "___"
    for (int i = 0; i < strlen(mot); i++) {
        devine[i] = '_';
    }
    devine[strlen(mot)] = '\0'; // Ajout du terminateur de chaîne


    printf("Mot : %s\nMot caché : %s\n", mot, devine);

    send(new_socket, devine, strlen(devine), 0);

    printf("nous allons commencer le pendu\n");

    return 0;
}

