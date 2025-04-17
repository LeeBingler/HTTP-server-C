##
## EPITECH PROJECT, 2022
## Makefile
## File description:
## built library ,
## implement the clean, fclean, re rule.
##

SRCSERVER	=	server.c	\
				handle_client.c	\
				parse_request.c	\


SRCCLIENT	=	client.c	\

CFLAGS	=	-g3 -g -Wall -Wextra -lncurses

OBJSERVER	=	$(SRCSERVER:.c=.o)

OBJCLIENT	=	$(SRCCLIENT:.c=.o)

NAME	=	my-http-server

NAMESERVER	=	my-server

NAMECLIENT	=	my-client

all: 	$(NAME)

$(NAME):	$(NAMESERVER) $(NAMECLIENT)

$(NAMESERVER):	$(OBJSERVER)
	gcc -o $(NAMESERVER) $(OBJSERVER) $(CFLAGS) 

$(NAMECLIENT):	$(OBJCLIENT)
	gcc -o $(NAMECLIENT) $(OBJCLIENT) $(CFLAGS)

clean:
	rm -f $(OBJSERVER) $(OBJCLIENT)

fclean: clean
	rm -f $(NAME)

re:	fclean all
