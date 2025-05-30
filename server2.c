#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9097
#define BUFFER_SIZE 1024
#define NUM_WORDS 50
#define LETTRE_SIZE 1024
#define DECISION_SIZE 1024

int static nb_life = 7;

int rejouer(int new_socket);

int pendu(int new_socket);

// Fonction pour supprimer le saut de ligne de la chaîne
void remove_newline(char *str) {
    str[strcspn(str, "\n")] = 0;  // Suppprime le '\n' à la fin de la chaîne
}

char* concatene(int vies, char* mot) {
    int len = strlen(mot);
    char* res = malloc(len + 2); // +1 pour 'vies' et +1 pour '\0'

    res[0] = vies + '0'; // transforme vies en caractere puis le concatene en premiere place de la chaine 

    for (int i = 0; i < len; i++) {
        res[i + 1] = mot[i]; // Copie chaque caractère du mot
    }

    res[len + 1] = '\0'; // Ajoute le caractère de fin de chaîne
    //printf("%s\n", res);
    return res; // Retourne la nouvelle chaîne
}

int choose_word(int size){
    srand (time(NULL));
    return rand() % size + 1;
}

char* reveal_word(char lettre [], char mot [], char devine []){
    char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
//////////////gérer répétitions ? 

    if (strstr(alphabet, lettre) != NULL){ // vérifie que la letttre fait partie de l'alphabet 
        
        for (int i = 0; i < strlen(mot); i++){
            if (mot[i] == lettre[0]){
                devine[i] = lettre[0];
            }
        }
        if(strstr(mot, lettre) == NULL) {
            printf("La lettre %s ne fait pas partie du mot mystère !\n\n", lettre);
            nb_life--;
        }
    } else {
        printf("Vous devez rentrer un caractère valide");
    }
    return devine;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_received;

    while ((bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_received] = '\0';
        remove_newline(buffer);

        printf("Message reçu : %s\n", buffer);

        if (strcmp(buffer, "pendu") == 0) {
            pendu(client_socket);
        }

        memset(buffer, 0, BUFFER_SIZE);
    }

    printf("Client déconnecté.\n");
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Création du socket serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associer le socket à l'adresse (bind)
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur lors du bind");
        exit(EXIT_FAILURE);
    }

    // Mettre le socket en mode écoute
    if (listen(server_fd, 3) < 0) {
        perror("Erreur lors de l'écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexion sur le port %i \n", PORT);

    //signal(SIGCHLD, SIG_IGN);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len);
        if (new_socket < 0) {
            perror("Erreur lors de l'acceptation de la connexion");
            continue;
        }

        printf("Nouveau client connecté !\n");
    
        if (fork() == 0) { // Création d'un processus fils
            close(server_fd); // Le processus fils n'a pas besoin du socket d'écoute
            handle_client(new_socket); // Fonction pour gérer un client
            close(new_socket);
            exit(0);
        }
    
        close(new_socket); // Le processus parent n'a pas besoin du socket client
    }

    // Lecture du message envoyé par le client
    ssize_t bytes_received;
    while ((bytes_received = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_received] = '\0';  // Ajoute un caractère de fin de chaîne

        // Supprimer le '\n' du buffer si nécessaire
        remove_newline(buffer);

        printf("Message reçu : %s\n\n", buffer);

        // Comparaison correcte du message
        if (strcmp(buffer, "pendu") == 0) {
            pendu(new_socket);
        }

        memset(buffer, 0, BUFFER_SIZE);  // Réinitialisation du buffer
    }

    // Gestion de la fin de connexion
    if (bytes_received == 0) {
        printf("Client déconnecté.\n");
    } else {
        perror("❌ Erreur de lecture");
    }

    // Fermeture des sockets
    close(new_socket);
    close(server_fd);

    return 0;
}

int pendu(int new_socket) {
    printf("\n-------- début du pendu --------\n\n");
    char *liste_de_mots[NUM_WORDS] = {
        "chien", "chat", "voiture", "maison", "ordinateur",
        "bureau", "ecole", "livre", "montagne", "mer", 
        "pomme", "banane", "avion", "arbre", "fleur", 
        "table", "chaise", "fenetre", "porte", "rue", 
        "ville", "pays", "oiseau", "poisson", "papillon", 
        "elephant", "lion", "tigre", "requin", "serpent", 
        "baleine", "dauphin", "ourson", "loutre", "mouton", 
        "vache", "cochon", "cheval", "lapin", "cerf", 
        "grenouille", "papillon", "araignee", "abeille", "fourmi", 
        "chemin", "route", "monts", "plage", "jardin"
    };

    int index = choose_word(NUM_WORDS);  // Choisir un mot aléatoire
    char *mot = liste_de_mots[index];
    size_t longueur = strlen(mot);
    char *devine = malloc(longueur + 1);

    if (devine == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return 1;
    }

    for (int i = 0; i < longueur; i++) {
        devine[i] = '_';
    }
    devine[longueur] = '\0';

    printf("Mot : %s\n", mot);

    // Envoi du mot caché au client
    ssize_t bytes_sent = send(new_socket, devine, strlen(devine), 0);
    if (bytes_sent < 0) {
        perror("Erreur lors de l'envoi du mot caché");
        free(devine);
        return 1;
    }

    // Début de la boucle de jeu
    int mot_trouve = 0;
    char lettre[LETTRE_SIZE] = {0};

    while (nb_life > 0 && !mot_trouve) {  // Tant qu'il y a des vies et que le mot n'est pas trouvé
        ssize_t bytes_received;
        bytes_received = read(new_socket, lettre, LETTRE_SIZE - 1);
        if (bytes_received > 0) {
            lettre[bytes_received] = '\0';
            remove_newline(lettre);  // Supprimer '\n'

            // Révéler la lettre et vérifier
            devine = reveal_word(lettre, mot, devine);

            // Vérifier si le mot a été trouvé
            if (strcmp(devine, mot) == 0) {
                mot_trouve = 1;
                send(new_socket, "gagne", 6, 0);  // Envoyer "gagne" au client
                printf("La partie est terminé, le client a gagné !\n");
                rejouer(new_socket);
                break;
            }

            // Vérifier si l'utilisateur a perdu
            if (nb_life <= 0) {
                send(new_socket, "perdu", 6, 0);  // Envoyer "perdu" au client
                printf("La partie est terminé, le client a perdu... \n");
                rejouer(new_socket);
                break;
            }

            // Envoyer la mise à jour du mot caché et les vies restantes
            send(new_socket, concatene(nb_life, devine), strlen(devine) + 1, 0);
            printf("Mot mystère : %s\nNombre de vies restantes : %d\n", devine, nb_life);
        }
    }
    free(devine);
    return 0;
}


int rejouer(int new_socket){
    char decision[DECISION_SIZE] = {0};
    ssize_t bytes_received;

    bytes_received = read(new_socket, decision, DECISION_SIZE - 1);
    if (bytes_received > 0) {
        decision[bytes_received] = '\0';
        remove_newline(decision);
        if (strcmp(decision, "O") == 0 ) {
            printf("Une nouvelle partie va commencer ! \n");
            pendu(new_socket);
        }
        if (strcmp(decision, "n") == 0) {
            printf("Aucune partie ne va recommencer !\n");
            close(new_socket);
        }
    }
    return 0;
}