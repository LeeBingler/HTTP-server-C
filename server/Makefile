SRC	=	server.c	\
				handle_client.c	\
				parse_request.c	\

CFLAGS	=	-g3 -g -Wall -Wextra

OBJ	=	$(SRC:.c=.o)

NAME	=	my-server

all: 	$(NAME)

$(NAME):	$(OBJ)
	gcc -o $(NAME) $(OBJ) $(CFLAGS) 

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all
