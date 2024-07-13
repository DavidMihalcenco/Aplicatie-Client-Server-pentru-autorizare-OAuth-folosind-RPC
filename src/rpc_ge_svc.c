/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc_ge.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif
char **IDs; // Lista cu toti clientii cititi din fisier
char **Permisions; // Lista cu toate permisiunile din fisierul approvals
char **Resouces; // Lista cu toate resursele din fisier
int nrResources; // Numarul resurselor citit din fisier
int nrUsers; // Numarul clientilor citit din fisier
int nrPermisions; // Numarul permisiunilor citite din fisier
int nrElemDB; // Numarul de clieni in userDataBase
int curentPermision; // Permisiunea curenta la care ma aflu in Permissions
int validity; // Valabilitatea jetonului citita de la stdin

userStruct* userDataBase; // Baza de date 
static void
oauthauthorization_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		char *request_authorization_1_arg;
		AccessRequest request_token_1_arg;
		DelegatedActionRequest validate_action_1_arg;
		char *approve_token_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case request_authorization:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) request_authorization_1_svc;
		break;

	case request_token:
		_xdr_argument = (xdrproc_t) xdr_AccessRequest;
		_xdr_result = (xdrproc_t) xdr_AccessResponse;
		local = (char *(*)(char *, struct svc_req *)) request_token_1_svc;
		break;

	case validate_action:
		_xdr_argument = (xdrproc_t) xdr_DelegatedActionRequest;
		_xdr_result = (xdrproc_t) xdr_DelegatedActionResponse;
		local = (char *(*)(char *, struct svc_req *)) validate_action_1_svc;
		break;

	case approve_token:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) approve_token_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}
/*
	Funtia pentru citirea si stocarea clientilor in IDS
	filename - Calea la care se afla fisierul
*/
void storeIDsFromFile(const char *filename) {
    int count = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

	fscanf(file,"%d",&nrUsers);
    IDs = (char **)malloc(nrUsers * sizeof(char *));

    for (int i = 0; i < nrUsers; ++i) {
        IDs[i] = (char *)malloc(15* sizeof(char));
        if (IDs[i] == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
    }
    while (count < nrUsers && fscanf(file, "%s", IDs[count]) == 1) {
        count++;
    }

    fclose(file);
}

/*
	Funtia pentru citirea si stocarea resurselor in Resources
	filename - Calea la care se afla fisierul
*/
void storeResourcesFromFile(const char *filename) {
    int count = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

	fscanf(file,"%d",&nrResources);
    Resouces = (char **)malloc(nrResources * sizeof(char *));

    for (int i = 0; i < nrResources; ++i) {
        Resouces[i] = (char *)malloc(15* sizeof(char));
        if (Resouces[i] == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
    }
    while (count < nrResources && fscanf(file, "%s", Resouces[count]) == 1) {
        count++;
    }

    fclose(file);
}

/*
	Funtia pentru citirea si stocarea permisiunilor in Permisions
	filename - Calea la care se afla fisierul
*/
void storePermisionsFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;

        char *temp = strdup(line);
        Permisions = realloc(Permisions, (nrPermisions + 1) * sizeof(char *));
        Permisions[nrPermisions++] = temp;
    }

    fclose(file);
}

int
main (int argc, char **argv)
{	
	setvbuf(stdout, NULL, _IONBF, 0); 
	nrElemDB = 0;
	curentPermision = 0;
	nrPermisions = 0;
	// Alocarea si citirea fisierelor
	char *filename = malloc(FILENAME_SIZE * sizeof(char));
	strcpy(filename, argv[1]);
	char *filename2 = malloc(FILENAME_SIZE * sizeof(char));
	strcpy(filename2, argv[2]);
	char *filename1 = malloc(FILENAME_SIZE * sizeof(char));
	strcpy(filename1, argv[3]);
	validity = atoi(argv[4]);
	storeIDsFromFile(filename);
	storePermisionsFromFile(filename1);
	storeResourcesFromFile(filename2);
	// Alocarea bazei de date
	userDataBase = calloc(nrUsers, sizeof(userStruct));
	for (int i = 0; i < nrUsers; ++i){
		userDataBase[i].userId = calloc(TOKENS_SIZE,sizeof(char));
		userDataBase[i].authToken = calloc(TOKENS_SIZE,sizeof(char));
		userDataBase[i].refreshToken = calloc(TOKENS_SIZE,sizeof(char));
		userDataBase[i].access_token = calloc(TOKENS_SIZE,sizeof(char));
		userDataBase[i].sources = calloc(PERMISSIONS_SIZE,sizeof(allSour));
		for (int j = 0; j < PERMISSIONS_SIZE; ++j) {
			userDataBase[i].sources[j].name = calloc(TOKENS_SIZE,sizeof(char));
		}
	}
	register SVCXPRT *transp;

	pmap_unset (OauthAuthorization, OauthVersion);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, OauthAuthorization, OauthVersion, oauthauthorization_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (OauthAuthorization, OauthVersion, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, OauthAuthorization, OauthVersion, oauthauthorization_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (OauthAuthorization, OauthVersion, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}