######################################################################
# Arquivo criado por Bruno Peres e adaptado para a disciplina de
# Redes 2015/2 por Lucas Monteiro
######################################################################

# Lista dos objetos (arquivos .c, troca-se a extensao para .o) necessarios para o programa final
OBJS_CLIENT = client.c
OBJS_SERVER = server.o user.o

# Nome do executavel que sera gerado
CLIENT = cliente
SERVER = servidor

# Especifica o compilador
CC = gcc

# Especifica as operacoes do compilador, habilita aviso sobre erros
CFLAGS = -Wall -pg -g3

# Comando terminal para limpar sem confirmacao
RM = rm -f

all: $(CLIENT) $(SERVER)
# Compilacao do programa e passos das ligacoeses de dependencias
$(CLIENT): $(OBJS_CLIENT)
	@echo ""
	@echo " --- COMPILANDO PROGRAMA ---"
	@$(CC) $(CFLAGS) $(OBJS_CLIENT) -lm -o $(CLIENT)
	@echo ""

%.o: %.c %.h
	@echo ""
	@echo " --- COMPILANDO OBJETO \"$@\""
	@$(CC) $(CFLAGS) $< -c

# Compilacao do programa e passos das ligacoeses de dependencias
$(SERVER): $(OBJS_SERVER)
	@echo ""
	@echo " --- COMPILANDO PROGRAMA ---"
	@$(CC) $(CFLAGS) $(OBJS_SERVER) -lm -o $(SERVER)
	@echo ""

%.o: %.c %.h
	@echo ""
	@echo " --- COMPILANDO OBJETO \"$@\""
	@$(CC) $(CFLAGS) $< -c
