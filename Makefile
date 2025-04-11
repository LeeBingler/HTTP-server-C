##
## EPITECH PROJECT, 2022
## Makefile
## File description:
## built library ,
## implement the clean, fclean, re rule.
##

SRC	=	main.c	\

CFLAGS	=	-g3 -g -Wall -Wextra -lncurses

OBJ	=	$(SRC:.c=.o)

NAME	=	my-http-server

all: 	$(NAME)

$(NAME):	$(OBJ)
	gcc -o $(NAME) $(OBJ) $(CFLAGS) 

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all
