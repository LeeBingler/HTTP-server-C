# === Configuration ===

CC 		:= 	gcc

CFLAGS	:=	-g3 -Wall -Wextra

NAME	:=	http-server-c


SRC		:=	src/core/server.c	\
			src/core/parse_arg.c	\
			src/http/handle_client.c	\
			src/http/http_headers.c	\
			src/http/parse_request.c	\
			src/http/status_log.c	\
			src/http/send_file.c	\
			src/http/mime_types.c	\
			src/request/get.c	\
			src/request/post.c	\
			src/request/head.c	\
			src/utils/utils.c	\

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