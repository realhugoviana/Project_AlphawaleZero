#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h> 
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
// #include <MQTTClient.h>

#include "jeu.h"
#include "minimax.h"
#include "evaluation.h"
#include "mcts.h"

#define IA_MODE 2 // 0: Joueur vs Joueur, 1: Joueur vs IA, 2: IA vs IA, 3: Joueur vs IA avec MQTT

#define PROFONDEUR_IA_1 1
#define ALGO_IA_1 alphaBetaVariable
#define EVAL_IA_1 evalMinChoix

#define PROFONDEUR_IA_2 1
#define ALGO_IA_2 alphaBetaVariable
#define EVAL_IA_2 evalMinChoix

#define JOUEUR_MACHINE 1
#define DUREE_SLEEP 0

// #define MQTT_ADDRESS  "ssl://localhost:8883"
// #define MQTT_CLIENTID "IA_AWALE"
// #define TOPIC_J0 "awale/j0"
// #define TOPIC_J1 "awale/j1"
// #define TOPIC_MATCH "awale/match"
// #define MQTT_QOS 1
// #define MQTT_TIMEOUT 10000L

// MQTTClient client;
// char dernier_coup[8] = {0};

// int message_arrive(void* context, char* topicName, int topicLen, MQTTClient_message* message) {
//     char* dernier_coup = (char*)context;

//     size_t len = message->payloadlen;
//     if (len >= 8) len = 7;

//     memset(dernier_coup, 0, 8);
//     memcpy(dernier_coup, message->payload, len);
//     dernier_coup[len] = '\0';

//     printf("[MQTT] Coup reçu : %s\n", dernier_coup);

//     MQTTClient_freeMessage(&message);
//     MQTTClient_free(topicName);
//     return 1;
// }

// void publier_coup(const char* topic, Coup* coup) {
//     MQTTClient_message pubmsg = MQTTClient_message_initializer;
//     MQTTClient_deliveryToken token;

    
//     char* couleurChar;
//     if (coup->couleur == 0)
//         couleurChar = "R";
//     else if (coup->couleur == 1)
//         couleurChar = "B";
//     else if (coup->couleur == 2)
//         couleurChar = "TR";
//     else 
//         couleurChar = "TB"; 
//     char* coupChar;
//     asprintf(&coupChar, "%d%s", coup->trou + 1, couleurChar);

//     pubmsg.payload = (void*)coupChar;
//     pubmsg.payloadlen = strlen(coupChar);
//     pubmsg.qos = MQTT_QOS;
//     pubmsg.retained = 0;

//     MQTTClient_publishMessage(client, topic, &pubmsg, &token);
//     MQTTClient_waitForCompletion(client, token, MQTT_TIMEOUT);
//     printf("Coup publié : %s\n", coupChar);

//     free(coupChar);
// }

// void init_mqtt() {
//     MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//     MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;

//     ssl_opts.enableServerCertAuth = 0;
//     ssl_opts.verify = 0;

//     conn_opts.keepAliveInterval = 60;
//     conn_opts.cleansession = 1;
//     conn_opts.ssl = &ssl_opts;

//     MQTTClient_create(&client, MQTT_ADDRESS, MQTT_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
//     MQTTClient_setCallbacks(client, dernier_coup, NULL, message_arrive, NULL);

//     if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
//         fprintf(stderr, "Impossible de se connecter au broker MQTT\n");
//         exit(EXIT_FAILURE);
//     }

//     if (JOUEUR_MACHINE == 0)
//         MQTTClient_subscribe(client, TOPIC_J1, MQTT_QOS);
//     else
//         MQTTClient_subscribe(client, TOPIC_J0, MQTT_QOS);
// }

int main() {
    Jeu* jeu = initJeu(JOUEUR_MACHINE); 
    afficherJeu(jeu);

    // if (IA_MODE == 3) {
    //     init_mqtt();
    // }

    while (!estFinPartie(jeu)) {
        Coup* coup = NULL;
        double timeSpent = 0.0;
        clock_t start, end;

        // --- Mode Joueur vs Joueur ---
        if (IA_MODE == 0) {
            coup = lireCoup(jeu);
        }

        // --- Mode Joueur vs IA ---
        else if (IA_MODE == 1) {
            if (jeu->joueurActuel == jeu->joueurMachine) { 
                // IA joue
                start = clock();
                coup = choisirMeilleurCoup(jeu, PROFONDEUR_IA_1, ALGO_IA_1, EVAL_IA_1);
                end = clock();
                timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
            } else { 
                // Joueur humain
                coup = lireCoup(jeu);
            }
        }

        // --- Mode IA vs IA ---
        else if (IA_MODE == 2) {
            start = clock();
            if (jeu->joueurActuel == 0) 
                coup = choisirMeilleurCoupIteratifVariable(jeu, PROFONDEUR_IA_1, ALGO_IA_1, EVAL_IA_1);
            else 
                coup = choisirMeilleurCoupMCTS(jeu, PROFONDEUR_IA_2, ALGO_IA_2, EVAL_IA_2);
            end = clock();
            timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
            sleep(DUREE_SLEEP);
        }

        // // --- Mode Joueur vs IA avec MQTT ---
        // else if (IA_MODE == 3) {
        //     char* topic_send = (JOUEUR_MACHINE == 0) ? TOPIC_J0 : TOPIC_J1;
        //     char* topic_recv = (JOUEUR_MACHINE == 0) ? TOPIC_J1 : TOPIC_J0;

        //     if (jeu->joueurActuel == JOUEUR_MACHINE) {
        //         start = clock();
        //         coup = choisirMeilleurCoupIteratifVariable(jeu, PROFONDEUR_IA_1, ALGO_IA_1, EVAL_IA_1);
        //         end = clock();
        //         timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
        //         publier_coup(topic_send, coup);
        //     }
        //     else {
        //         while (strlen(dernier_coup) == 0) {
        //             MQTTClient_yield();
        //             usleep(100000);
        //         }

        //         int c = 0, col = 0;
        //         sscanf(dernier_coup, "%d", &c);

        //         char* p = dernier_coup;
        //         while (*p && isdigit(*p)) p++;

        //         if (*p == 'R') col = 0;
        //         else if (*p == 'B') col = 1;
        //         else if (*p == 'T') {
        //             p++;
        //             col = (*p == 'R') ? 2 : 3;
        //         }

        //         coup = creerCoup(c - 1, col);
        //         memset(dernier_coup, 0, 8);
        //     }
        // }

        jouerCoup(jeu, coup);
        afficherJeu(jeu);
        sortirCoup(coup);

        if (IA_MODE != 0) 
            printf("Temps de calcul : %.8f secondes\n", timeSpent);

        libererCoup(coup);
        printf("Coup numéro : %d\n", jeu->nbCoups);
    }
    // if (IA_MODE == 3) {
    //     MQTTClient_disconnect(client, 1000);
    //     MQTTClient_destroy(&client);
    // }

    printf("Partie terminée.\n");
    return 0;
}