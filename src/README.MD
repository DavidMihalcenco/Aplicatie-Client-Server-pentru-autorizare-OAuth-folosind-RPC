## SPRC 2023-2024 - Tema 1 - Aplicatie Client/Server pentru autorizare OAuth folosind RPC

**David Mihalcenco 334C3**

## Structurile folosite

    Fisierul rpc_ge.x
        Structurile sunt facute dupa conditia primita.
        La DelegatedActionResponse am adaugat nu doar String-ul cu erroarea,
        dar si new access token si new refresh token, folosite ca sa transmit clientului
        noile token-uri pentru ca clientul sa le salveze si in baza lui de date, pentru
        a le afisa corect.
```c
    struct AccessRequest {
        string user_id<50>; // User Id 
        string authorization_token<100>; // Auth Token
        int refresh; - // 0 if clienul vrea refresh else 0
        int valabil; - // Cate comenzi mai are
    };

    struct AccessResponse {
        string response_type<100>; // String cu erroarea
        string access_token<100>;  // Acces token
        string refresh_token<100>; // Refresh token
        int validity_period; // Cate comenzi au ramas
    };

    struct DelegatedActionRequest {
        string action<100>; // Actiunea care o doreste 
        string resource<100>; // Resursa pe care face o acctiune
        string access_token<100>; // Acces token
        int autoRefresh; // Daca doreste autoRefresh
    };

    struct DelegatedActionResponse{
        string response<100>; // Un string cu erroarea
        string new_acc_token<100>; // Noul acces token
        string new_refr_token<100>; // Noul refresh token
    };
```
    - Fisierul rpc_ge.h, am adaugat doua noi structuri.
        allSour - Structura in care se afla o resursa si toate permisiunile pe care le are un client.
        userStructure - Structura unui client cu toate datele, de care am avut nevoie in server.

```c
    typedef struct allSources {
	char *name; // Numele sursei , mai jos toate permisiunile 
	int insert;
	int modify;
	int execute;
	int read;
	int delete;
    }allSour;

    typedef struct userStructure {
        char *userId; // Id-ul Clientului
        char *authToken; // Tokenul de autentificare
        char *refreshToken; // Tokenul de Refresh
        char *access_token; // Tokenul de acces
        int valability; // Valabilitatea
        int refresh; // Daca se face refresh sau nu
        allSour *sources; // Toate sursele si permisiunile care le are clientul
    }userStruct;

```
    - Fisierul rpc_ge_client.c, am adaugat o structura care reprezinta un client.
```c
    typedef struct clientStr{
        char *clientid; // id client care vine din fisier
        char *clauth; // tokenul de autentificare
        char *clrefr; // tokenul de refresh
        char *claccs; // tokenul de acces
    }clientStruct;
```

## Variabile Globale folosite
    - Fisierul rpc_ge_svc.c

```c
    char **IDs; // Lista cu toti clientii cititi din fisier
    char **Permisions; // Lista cu toate permisiunile din fisierul approvals
    char **Resouces; // Lista cu toate resursele din fisier
    int nrResources; // Numarul resurselor citit din fisier
    int nrUsers; // Numarul clientilor citit din fisier
    int nrPermisions; // Numarul permisiunilor citite din fisier
    int nrElemDB; // Numarul de clieni in userDataBase
    int curentPermision; // Permisiunea curenta la care ma aflu in Permissions
    int validity; // Valabilitatea jetonului citita de la stdin
```
    - Fisierul rpc_ge_server.c, am folosit variabilele globale de mai sus, aduse cu extern.

```c
    extern char **IDs;
    extern char **Permisions;
    extern int nrUsers; 
    extern int nrElemDB;
    extern int curentPermision;
    extern int nrResources;
    extern int nrPermisions;
    extern char **Resouces;
    extern userStruct* userDataBase;
    extern int validity;
```
    - Fisierul rpc_ge_client.c
```c
   char **Commands; // Comenzile citite in fisier
    int nrCommands; // Numarul comenzilor in fisier
    int currentclient; // Indexul la care o sa adaug un clien nou
    int nrClientsDB; // Numarul curent de clienti in database 
```

## Functionalitatea si Functiile folosite
Totul se incepe din **rpc_ge_svc.c** unde am trei functii de citire a fisierelor : 
```c
    /*
	Funtia pentru citirea si stocarea clientilor in IDS
	filename - Calea la care se afla fisierul
    */
    void storeIDsFromFile(const char *filename)

    /*
	Funtia pentru citirea si stocarea resurselor in Resources
	filename - Calea la care se afla fisierul
    */
    void storeResourcesFromFile(const char *filename)

    /*
	Funtia pentru citirea si stocarea permisiunilor in Permisions
	filename - Calea la care se afla fisierul
    */
    void storePermisionsFromFile(const char *filename)
```
In main fac alocari pentru toata baza mea de date, adica structura **userStruct**.

Dupa urmeaza fisierul **rpc_ge_client** aici am functia de citire a comenzilor :
```c
    /*
        Functia de citire din fisieul client.in si stocarea lor in Commands
        filename - Calea la care se afla fisierul
    */
    void storeCommandsFromFile(const char *filename)
```
In main aloc baza de date din Client, adica strunctura **clientStruct**.
Dupa se incepe apelarea functiilor, din server.
Daca comanda este Request atunci verific in baza mea de date din client daca clientul cu id-ul dat
nu exista, daca exista nu-l adaug, daca exista atunci il adaug la indexul **nrClientsDB** care este incrementat
dupa fiecare adaugare a unui nou client.
Dupa se apeleaza functia din server **request_authorization_1**
```c
    /*
	Functia care genereaza din id a unui client un auth token,
    clientul(id-ul lui, auth token) sunt adaugate in baza de date.
	IN
	- argp - id client
	RETURN 
	- auth token
    */
    char **request_authorization_1_svc(char **argp, struct svc_req *rqstp)
```
Workflow **request_authorization_1_svc**
```
    Se face verificarea in baza de date daca clientul exista, daca exista atunci i se genereaza auth token,
    si este adaugat in baza de date la client.
    Daca nu exista, atunci se v-a cauta daca acest client exista in UsersIDs.db, in cazul in care el exista,
    atunci se v-a genera un auth token si va fi adaugat si clienid in baza de date, si token auth.
```
Workflow **approve_token_1**
```c
    /*
        Functia care semnseaza auth token-ul daca clientul ii da permisiuni.
        Si se adauga resursele si permisiunile in baza de date a clientului.
        IN
        - argp - auth token
        RETURN 
        - auth token signed
    */
    char **approve_token_1_svc(char **argp, struct svc_req *rqstp)
```
```
    Dupa ce s-a executat functia request_authorization_1_svc cu succes, ea v-a intoarce un auth token clientului, care v-a apela
    aceasta functie, care face verificarea daca clientul are acorda permisiuni pe fisiere. Daca el acorda
    niste permisiuni unor resurse, atunci ele vor fi adaugate in baza de date la client, cu permisiunile lor,
    si se v-a semna auth-tokenul. Am ales sa-l semnez scriind la sfarsitul token-ului cuvantul VERIFIED.
    Acest token v-a fi returnat semnat in cazul in care clientul acorda permisiuni, daca nu se acorda nici o
    permisiune atunci se v-a intoarce tokenul asa cum a venit.
```
Workflow **request_token_1**
```c
    /*
	Functia care genereaza din auth token semnat, un acces token, si un refresh token 
	daca ca parametru ii vine ca trebuie sa genereze.
	IN
	- argp.authorization_token - auth token
	- argp.refresh - 1 if refresh is on , 0 off
	- argp.user_id - client id
	RETURN 
	- access token
	- refresh token (if refresh is 1)
	- response type(if auth token is not signed return REQUEST DENIED 
	else REQUEST GRANTED)
    */
    AccessResponse *request_token_1_svc(AccessRequest *argp, struct svc_req *rqstp)
```
```
    Dupa ce s-a executat functia approve_token_1, aceasta functie v-a primi auth token-ul semnat sau nu.
    Daca token-ul este semnat atunci v-a intoarce REQUEST_DENIED, daca este semnat atunci se v-a genere
    un acces token, si un refresh token(in caz ca clientul vrea sa se faca refresh) si se vor scrie in baza 
    de date a clientului.
```
Workflow **validate_action_1**
```c
    /*
        Functia care raspunde de toate errorile si generarea noilor refresh token si auth token, 
        daca refresh este pornit, si valabilitatea este 0
        IN
        - argp.access_token = access token;
        - argp.action = command;
        - argp.resource = resource;
        RETURN
        - new refresh token, new acces token (if refresh = 1, valability = 0 else NULL)
        - response
    */
    DelegatedActionResponse *validate_action_1_svc(DelegatedActionRequest *argp, struct svc_req *rqstp)
```
```
    In caz ca in client nu se citeste comanda REQUEST se v-a executa aceasta functie, care face verificarile.
    - Se verifica daca clientul exista in baza de date.
        Daca el nu exista atunci se va returna PERMISSION_DENIED
    - Se verifica daca clientul mai poate sa faca actiuni.
        Inclusiv aici se verifica si daca clientul vrea refresh automat.
        Daca el nu mai are operatiuni si vrea refresh automat atunci i se va genera un nou acces token,
        si un nou refresh token, care v-a fi trimis ca raspuns. Si actualizate in bazele de date, atat a serverului
        cat si a clientului.
        Daca nu vrea auto refresh raspunsul v-a fi TOKEN_EXPIRED. 
    - Se verifica daca resursa exista.
        Daca ea nu exista se va returna RESOURCE_NOT_FOUND
    - Se verifica daca sunt permisiuni pe acctiunea care vrea sa o faca.
        Daca nu are permisiuni atunci se intoarce OPERATION_NOT_PERMITED.
    Daca toate aceste verificare au trecut atunci se returneaza PERMISSION_GRANTED.
```
## ALTE NUANTE
    - Au fost adaugate la teste newline-uri pentru ca eu mereu scriu, cu \n. Si mereu voi avea un newline, atat 
    in client cat si in server.
    - In fisierul .x am cateva variabile in structuri care nu le folosesc, dar nu vreau sa le sterg pentru 
    a nu strica tot programul.
    - Am adaugat comanda setvbuf(stdout, NULL, _IONBF, 0) pentru a controla modul de bufferizare a serverului.
    - Pentru Validate Delegated Action am schimbat structura care a fost propusa de tema, si am returnat nu doar 
    raspunsul care este un string, dar si refresh token, si acces token, pentru a le putea afisa corect in client,
    si a le salva si in baza de date a clientului.
    - A fost cerut sa Semnez cumva auth token-ul, si am ales varianta ca sa adauga la sfarsitul lui cuvantul VERIFIED.



    