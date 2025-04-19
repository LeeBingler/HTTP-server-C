# === Configuration ===

CC 		:= 	gcc

CFLAGS	:=	-g3 -Wall -Wextra

NAME	:=	http-server-c


SRC		:=	src/server.c	\
			src/handle_client.c	\
			src/parse_request.c	\
			src/parse_arg.c	\
			src/http_headers.c	\
			src/status_log.c	\
			src/utils.c	\

OBJ		:=	$(SRC:.c=.o)


# === Rules ===


all: 	$(NAME)

$(NAME):	$(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re