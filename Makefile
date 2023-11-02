NAME = img-search

SRC_PATH = srcs

SRC_NAME =	img-search-4-5.c	\
			get_next_line.c		\
			utils.c				\
			sig_handlers.c

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -O2 -Wpedantic
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