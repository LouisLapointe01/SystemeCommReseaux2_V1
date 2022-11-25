/*************************************************************
 * proto_tdd_v0 -  émetteur                                   *
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
/* Programme principal - émetteur  */
/* =============================== */
int main(int argc, char *argv[])
{
    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg;                  /* taille du message */
    paquet_t paquet;                 /* paquet utilisé par le protocole */
    paquet_t ACKpaquet;              /*paquet retour*/

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    /* tant que l'émetteur a des données à envoyer */
    while (taille_msg != 0)
    {
        /*J'initialise le paquet retour en négatif*/
        ACKpaquet.type = NACK;
        /* construction paquet */
        for (int i = 0; i < taille_msg; i++)
            paquet.info[i] = message[i];

        paquet.lg_info = taille_msg;
        paquet.type = DATA;

        paquet.somme_ctrl = generer_controle(paquet);

        while (ACKpaquet.type == NACK && test_temporisateur(0) == 0)
        {
            depart_temporisateur(0, 1000);
            vers_reseau(&paquet);
            if (test_temporisateur(0) == 1)
            {
                de_reseau(&ACKpaquet);
                arreter_temporisateur(0);
            }
        }

        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);
    }

    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
