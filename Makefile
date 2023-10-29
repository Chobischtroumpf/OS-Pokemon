NAME = img-search

SRC_PATH = srcs/

SRC_NAME =	main.c	\
			utils.c

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Wpedantic
SRC = $(addprefix $(SRC_PATH)/,$(SRC_NAME))
OBJ = $(SRC:%.c=%.o)

default:
	@make -s all

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	@$(call update)

.SILENT:clean

clean:
	@rm -rf $(OBJ)

fclean:
	@rm -rf $(OBJ)
	@rm -f $(NAME)

re: 
	@echo "img-search recompiling"
	@make -s fclean 
	@make -s all