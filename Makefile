NAME = img-search

SRC_PATH = srcs

SRC_NAME =	img-search.c	\
			utils.c				\
			get_next_line.c

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g2
LDFLAGS += -fsanitize=address
SRC = $(addprefix $(SRC_PATH)/,$(SRC_NAME))
OBJ = $(SRC:%.c=%.o)

default:
	@make -s all

all: $(NAME)

$(NAME): $(OBJ)
	@echo "img-search compiling"
	@$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	@$(call update)

.SILENT:clean

clean:
	@echo "cleaning up img-search object files"
	@rm -rf $(OBJ)

fclean:
	@echo "cleaning up img-search object files and executable"
	@rm -rf $(OBJ)
	@rm -f $(NAME)

re: 
	@make -s fclean 
	@make -s all