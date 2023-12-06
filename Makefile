NAME = ft_traceroute
SRCDIR = srcs/
INCDIR = include/
SRCFILES = main.c\

INCFILES = ft_traceroute.h
SRC = $(addprefix $(SRCDIR),$(SRCFILES))
INC = $(addprefix $(INCDIR),$(INCFILES))
OBJ = $(SRC:.c=.o)
FLAG = -Wall -Werror -Wextra -I include

all: $(NAME)

%.o: %.c $(INC)
	gcc -c -o $@ $< $(FLAG)

$(NAME): $(OBJ) $(INC)
	gcc -o $@ $(OBJ) $(FLAG)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all