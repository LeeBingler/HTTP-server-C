SRC	=			src/server.c	\
				src/handle_client.c	\
				src/parse_request.c	\
				src/parse_arg.c	\
				src/http_headers.c	\
				src/status_log.c	\
				src/utils.c	\

CC = gcc

CFLAGS	=	-g3 -g -Wall -Wextra

NAME	=	http-server-c

OBJ	=	$(SRC:.c=.o)

all: 	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(CFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all