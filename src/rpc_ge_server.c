/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "rpc_ge.h"
#include "../token.h"
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

/*
	Functia care genereaza din id a unui client un auth token,
    clientul(id-ul lui, auth token) sunt adaugate in baza de date.
	IN
	- argp - id client
	RETURN 
	- auth token
*/
char **request_authorization_1_svc(char **argp, struct svc_req *rqstp)
{	
	printf("BEGIN %s AUTHZ\n",*argp);
	static char * result;
	result = calloc(TOKENS_SIZE, sizeof(char));
	int matchFound = 0;
	for (int j = 0; j < nrUsers; ++j) {
       	if(strcmp(userDataBase[j].userId, *argp) == 0){
			strcpy(result,generate_access_token(*argp));
			printf("  RequestToken = %s\n",result);
			strcpy(userDataBase[j].authToken,result);
			return &result;
		}
    }
    for (int i = 0; i < nrUsers; ++i) {
        if (strcmp(IDs[i], *argp) == 0) {
			strcpy(result,generate_access_token(IDs[i]));
			printf("  RequestToken = %s\n",result);
			strcpy(userDataBase[nrElemDB].userId, *argp);
			strcpy(userDataBase[nrElemDB].authToken, result);
			nrElemDB++;
			matchFound = 1;
            return &result;
        }
    }

    if (matchFound == 0) {
		strcpy(result,"Match not found\n");
        return &result;
    }
	
}

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
{
	static AccessResponse result;
	char * access_token;
	char * refresh_token;
	access_token = calloc(TOKENS_SIZE, sizeof(char));
	refresh_token = calloc(TOKENS_SIZE, sizeof(char));
	if(strstr(argp->authorization_token,"VERIFIED")!=NULL){
		size_t len = strlen(argp->authorization_token);
		static char *word = "VERIFIED";
		size_t wordLen = strlen(word);
		if (len >= wordLen && strcmp(argp->authorization_token + len - wordLen, word) == 0) {
        	argp->authorization_token[len - wordLen] = '\0';
    	}
		strcpy(access_token, generate_access_token(argp->authorization_token));
		printf("  AccessToken = %s\n",access_token);
		if(argp->refresh == 1){
			strcpy(refresh_token,generate_access_token(access_token));
			printf("  RefreshToken = %s\n",refresh_token);
		}
		for(int i = 0 ; i < nrUsers; ++i){
			if (strcmp(userDataBase[i].authToken, argp->authorization_token)==0){
				strcpy(userDataBase[i].access_token, access_token);
				strcpy(userDataBase[i].refreshToken, refresh_token);
				userDataBase[i].refresh = argp->refresh;
				userDataBase[i].valability = validity;
			}
		}
		result.access_token = calloc(TOKENS_SIZE, sizeof(char));
		result.access_token = access_token;
		result.refresh_token = calloc(TOKENS_SIZE, sizeof(char));
		result.refresh_token = refresh_token;
		result.response_type = calloc(TOKENS_SIZE, sizeof(char));
		result.validity_period = validity;
		strcpy(result.response_type,"REQUEST GRANTED");
		return &result;

	}else{
		result.access_token = calloc(TOKENS_SIZE, sizeof(char));
		result.refresh_token = calloc(TOKENS_SIZE, sizeof(char));
		result.response_type = calloc(TOKENS_SIZE, sizeof(char));
		strcpy(result.response_type,"REQUEST DENIED");
		return &result;
	}
}

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
{
	static DelegatedActionResponse result;
	char *newTRefr;
	char *newTAcc;
	result.new_acc_token = strdup("NULL");
	result.new_refr_token = strdup("NULL");
	result.response = calloc(RESULT_SIZE, sizeof(char));
	
	int found = 0;
	int rFound = 0;
	int permGrant = 0;
	int i;

	if(strcmp(argp->access_token, "NOTDB")==0){
		printf("DENY (%s,%s,,0)\n",argp->action,argp->resource);
		strcpy(result.response,"PERMISSION_DENIED");
		return &result;
	}
	for(i = 0 ; i < nrUsers ; ++i){
		if(strcmp(userDataBase[i].access_token, argp->access_token) == 0){
			found = 1;
			break;
		}
	}
	if(found == 0){
		printf("DENY (%s,%s,,%d)\n",argp->action,argp->resource,userDataBase[i].valability);
		strcpy(result.response,"PERMISSION_DENIED");
		return &result;
	}
	
	if(userDataBase[i].valability == 0){
		if(userDataBase[i].refresh == 1){
			result.new_acc_token = strdup(generate_access_token(userDataBase[i].refreshToken));
			result.new_refr_token = strdup(generate_access_token(result.new_acc_token));
			printf("BEGIN %s AUTHZ REFRESH\n",userDataBase[i].userId);
			printf("  AccessToken = %s\n",result.new_acc_token);
			printf("  RefreshToken = %s\n",result.new_refr_token);
			userDataBase[i].valability = validity;
			strcpy(userDataBase[i].access_token,result.new_acc_token);
			strcpy(userDataBase[i].refreshToken,result.new_refr_token);
		}else{
			printf("DENY (%s,%s,,%d)\n",argp->action,argp->resource,userDataBase[i].valability);
			strcpy(result.response,"TOKEN_EXPIRED");
			return &result;
		}
	}

	userDataBase[i].valability--;

	for(int j = 0; j < nrResources; ++j){
		if(strcmp(Resouces[j],argp->resource)==0){
			rFound = 1;
		}
	}
	if(rFound == 0){
		printf("DENY (%s,%s,%s,%d)\n",argp->action,argp->resource,argp->access_token,userDataBase[i].valability);
		strcpy(result.response,"RESOURCE_NOT_FOUND");
		return &result;
	}

	for(int k = 0; k < nrResources;++k){
		if(strcmp(userDataBase[i].sources[k].name,argp->resource)==0){
			if(strcmp(argp->action,"READ")==0){
				if(userDataBase[i].sources[k].read == 1){
					permGrant = 1;
				}
			}
			if(strcmp(argp->action,"DELETE")==0){
				if(userDataBase[i].sources[k].delete == 1){
					permGrant = 1;
				}
			}
			if(strcmp(argp->action,"EXECUTE")==0){
				if(userDataBase[i].sources[k].execute == 1){
					permGrant = 1;
				}
			}
			if(strcmp(argp->action,"INSERT")==0){
				if(userDataBase[i].sources[k].insert == 1){
					permGrant = 1;
				}
			}
			if(strcmp(argp->action,"MODIFY")==0){
				if(userDataBase[i].sources[k].modify == 1){
					permGrant = 1;
				}
			}
		}		
	}
	if(permGrant == 0){
		strcpy(result.response,"OPERATION_NOT_PERMITTED");
		printf("DENY (%s,%s,%s,%d)\n",argp->action,argp->resource,userDataBase[i].access_token,userDataBase[i].valability);
		return &result;
	}
	printf("PERMIT (%s,%s,%s,%d)\n",argp->action,argp->resource,userDataBase[i].access_token,userDataBase[i].valability);
	strcpy(result.response,"PERMISSION_GRANTED");
	return &result;
}

/*
	Functia care semnseaza auth token-ul daca clientul ii da permisiuni.
	Si se adauga resursele si permisiunile in baza de date a clientului.
	IN
	- argp - auth token
	RETURN 
	- auth token signed
*/
char **approve_token_1_svc(char **argp, struct svc_req *rqstp)
{
	static char *result;
	char *pError;
	pError = calloc(ERROR_SIZE, sizeof(char));
	strcpy(pError,"*,-");
	result = calloc(SIGNED_TOKEN_SIZE, sizeof(char));
	int indexPerm=0;
	if(strcmp(Permisions[curentPermision], pError) == 0){
		strcpy(result,*argp);
		curentPermision++;
		return &result;
	}else{
		char *permLine = Permisions[curentPermision];
		char *token = strtok(permLine, ",");
		for(int j = 0; j < nrUsers; ++j){
			if(strcmp(userDataBase[j].authToken, *argp)==0){
				for(int k = 0; k < nrPermisions ; ++k){
					strcpy(userDataBase[j].sources[k].name,"NoPermisions");
					userDataBase[j].sources[k].delete = 0;
					userDataBase[j].sources[k].execute = 0;
					userDataBase[j].sources[k].insert = 0;
					userDataBase[j].sources[k].modify = 0;
					userDataBase[j].sources[k].read = 0;
				}
			}
		}
		while (token != NULL){
			for(int i = 0; i < nrResources; ++i){
				if(strcmp(token, Resouces[i])==0){
					for(int j = 0; j < nrUsers; ++j){
						if(strcmp(userDataBase[j].authToken, *argp)==0){
							strcpy(userDataBase[j].sources[indexPerm].name,token);
							token = strtok(NULL, ",");
							if(strchr(token,'R')!= NULL){
								userDataBase[j].sources[indexPerm].read = 1;
							}
							if(strchr(token,'I')!= NULL){
								userDataBase[j].sources[indexPerm].insert = 1;
							}
							if(strchr(token,'M')!= NULL){
								userDataBase[j].sources[indexPerm].modify = 1;
							}
							if(strchr(token,'X')!= NULL){
								userDataBase[j].sources[indexPerm].execute = 1;
							}
							if(strchr(token,'D')!= NULL){
								userDataBase[j].sources[indexPerm].delete = 1;
							}
							indexPerm++;
						}
					}
				}
			}
			token = strtok(NULL, ",");
		}
		curentPermision++;
		char *addToAuth = "VERIFIED";
		size_t aSize = strlen(addToAuth);
		strncat(*argp,addToAuth, aSize);
		strcpy(result, *argp);
		return &result;
	}
}
