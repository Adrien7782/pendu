#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9096
#define BUFFER_SIZE 1024
#define LETTRE_SIZE 1024
#define DECISION_SIZE 1024

int sock;

int pendu(int sock);

void remove_newline(char *str) {
    str[strcspn(str, "\n")] = 0;  // Supprime le '\n' à la fin de la chaîne
}

void slice(const char *source, char *dest, int start, int end) {
    int j = 0;
    for (int i = start; i < end && source[i] != '\0'; i++) {
        dest[j++] = source[i];
    }
    dest[j] = '\0'; // Null-terminate the string
}

void afficher_pendu(int vies) {
    switch (vies) {
        case 7:
            printf("\n\n");
            break;
        case 6:
            printf("6 vies restantes\n\n\n\n\n\n=========\n"); 
            break;
        case 5:
            printf("5 vies restantes\n\n    |\n    |\n    |\n    |\n=========\n"); 
            break;
        case 4:
            printf("4 vies restantes\n    +---+\n    |   |\n        |\n        |\n        |\n=========\n"); 
            break;
        case 3:
            printf("3 vies restantes\n    +---+\n    |   |\n    O   |\n        |\n        |\n=========\n"); 
            break;
        case 2:
            printf("2 vies restantes\n    +---+\n    |   |\n    O   |\n    |   |\n        |\n=========\n"); 
            break;
        case 1:
            printf("1 vie restante\n    +---+\n    |   |\n    O   |\n   /|\\  |\n        |\n=========\n"); 
            break;
        case 0:
            printf("0 vies restantes - PERDU !\n    +---+\n    |   |\n    O   |\n   /|\\  |\n   / \\  |\n=========\n Vous avez perdu !\n"); 
            break;
        default:
            printf("Erreur: nombre de vies invalide !\n");
    }
}

void traitement_message(char *str) {
    char message[strlen(str)];  

    slice(str, message, 1, strlen(str));  

    printf("\nMot mystère : %s\n", message);
    printf("Nombre de vies : %d\n\n", str[0] - '0');
    afficher_pendu(str[0] - '0');
}

int main() {
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    // Création du socket client
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Adresse invalide");
        exit(EXIT_FAILURE);
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Échec de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    // Lire un message à envoyer
    printf("A quel jeu voulez-vous jouer ? ");
    printf("\n\t-pendu\n\t-à venir...\n\nVotre choix : ");
    if (fgets(message, BUFFER_SIZE, stdin) == NULL || strlen(message) <= 1) {
        printf("Message vide, arrêt du client.\n");
        close(sock);
        return 0;
    }

    // Supprimer le '\n' à la fin du message
    remove_newline(message);

    // Envoi du message au serveur
    send(sock, message, strlen(message), 0);

    if (strcmp(message, "pendu") == 0){
        pendu(sock);
    }
    return 0;
}

int rejouer(int sock){
    char decision[LETTRE_SIZE] = {0};
    printf("\nVoulez-vous rejouer (O/n) ?\n");
    fgets(decision, DECISION_SIZE, stdin);
    remove_newline(decision);

        if (strcmp(decision, "O") == 0) {
            printf("Très bien la partie va commencer !\n");
            send(sock, decision, strlen(decision), 0);
            pendu(sock);
        } else {
            printf("OK à bientôt !\n");
            close(sock);
        }
    return 0;
}

int pendu(int sock){
    char buffer[BUFFER_SIZE] = {0};

    printf("\n\n----- Début du pendu ----- \n\n");

    // Attente de la réponse du serveur (affichage du mot caché par exemple)
    ssize_t bytes_received = read(sock, buffer, BUFFER_SIZE - 1);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Mot mystère : %s\n", buffer);
    } else {
        printf("Aucune réponse reçue du serveur.\n");
    }
    char lettre[LETTRE_SIZE] = {0};

    printf("Vous devez trouver le mot mystère !\n\n");

    int perdu;
    int gagne;

    perdu = 0;
    gagne = 0;

    while(perdu == 0 && gagne == 0) {
        printf("\nEntrez une lettre : ");
        fgets(lettre, LETTRE_SIZE, stdin);
        if (lettre == NULL ||  strlen(lettre) <= 1) {
            printf("Vous devez rentrer un caractère ! ");
        }

        //fgets(lettre, LETTRE_SIZE, stdin);

        remove_newline(lettre);
        
        send(sock, lettre, strlen(lettre), 0);
        
        bytes_received = read(sock, buffer, BUFFER_SIZE - 1);
    
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            remove_newline(buffer);
            
            if (strcmp(buffer, "gagne") == 0) {
                printf("\nVous avez gagné ! Félicitations !\n");
                rejouer(sock);
                break;
            } else if (strcmp(buffer, "perdu") == 0) {
                afficher_pendu(0);
                printf("\nVous avez perdu ! Dommage...\n");
                rejouer(sock);
                break;
            }
        
            traitement_message(buffer);
        }
    }   
    
    return 0;
}