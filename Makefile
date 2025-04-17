CFLAGS	=	-g3 -g -Wall -Wextra

NAME	=	my-http-server

NAMESERVER	=	my-server

NAMECLIENT	=	my-client

all: 	$(NAME)

$(NAME):	$(NAMESERVER) $(NAMECLIENT)

$(NAMESERVER):
	make -C server/
	mv -t ./ server/$(NAMESERVER)

$(NAMECLIENT):
	make -C client/
	mv -t ./ client/$(NAMECLIENT)

clean:
	make clean -C client/
	make clean -C server/

fclean: clean
	rm -f $(NAME)
	rm -f $(NAMECLIENT)
	rm -f $(NAMESERVER)

re:	fclean all
