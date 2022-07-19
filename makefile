CC=gcc
SRC_DIR=src
BIN_DIR=bin
LIB_DIR=lib
INCLUDE_DIR=include
CFLAG=-m64

.PHONY: clean rebuild

all: Lab05_Client Lab05_Server

Lab05_Client:
	${CC} ${CFLAG} ${SRC_DIR}/Lab05_Client.c -o ${BIN_DIR}/Lab05_Client

Lab05_Server:
	${CC} ${CFLAG} ${SRC_DIR}/Lab05_Server.c -o ${BIN_DIR}/Lab05_Server

clean:
	-rm -rf ${BIN_DIR}/Lab05_Client
	-rm -rf ${BIN_DIR}/Lab05_Server

rebuild: clean all
