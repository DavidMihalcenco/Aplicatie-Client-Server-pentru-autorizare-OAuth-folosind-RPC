
# This is a template Makefile generated by rpcgen

# Parameters

CLIENT = rpc_ge_client
SERVER = rpc_ge_server

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = 
SOURCES.x = rpc_ge.x

TARGETS_SVC.c = rpc_ge_svc.c rpc_ge_server.c rpc_ge_xdr.c 
TARGETS_CLNT.c = rpc_ge_clnt.c rpc_ge_client.c rpc_ge_xdr.c 
TARGETS = rpc_ge.h rpc_ge_xdr.c rpc_ge_clnt.c rpc_ge_svc.c rpc_ge_client.c rpc_ge_server.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)
# Compiler flags 

CFLAGS += -g -I/usr/include/tirpc
LDLIBS += -lnsl -ltirpc
RPCGENFLAGS = 

# Targets 

all : $(CLIENT) $(SERVER)

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c) 

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c) 

$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.c) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS) 

$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.c) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

 clean:
	 $(RM) core $(OBJECTS_CLNT) 

