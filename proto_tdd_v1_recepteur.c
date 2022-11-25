/*************************************************************
* proto_tdd_v0 -  récepteur                                  *
* TRANSFERT DE DONNEES  v1                                   *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* E. Lavinal - Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - récepteur */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet; /* paquet utilisé par le protocole */
    int fin = 0; /* condition d'arrêt */

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* tant que le récepteur reçoit des données */
    while ( !fin ) {

        //attendre(); /* optionnel ici car de_reseau() fct bloquante */
        de_reseau(&paquet);
        
        /*verification intégrité du paquet*/
        if (verifier_controle(paquet) == 1)
        {
            /*acquittement positif*/
            paquet.type = ACK;
        }
        else{
            /*acquittement négatif*/
            paquet.type = NACK;
        }


        /* extraction des donnees du paquet recu */
        for (int i=0; i<paquet.lg_info; i++) {
            message[i] = paquet.info[i];
        }
        if (paquet.type == ACK){
            /* remise des données à la couche application */
            fin = vers_application(message, paquet.lg_info);
        }

        vers_reseau(&paquet);
    }

    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
