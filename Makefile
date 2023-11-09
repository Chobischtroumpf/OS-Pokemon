
# exec name
NAME = img-search

# compiler
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g2

#folder names
SRCDIR = srcs/
INCDIR = includes/
OBJDIR = bin/

CFLAGS += -I $(INCDIR)

SRCS =\
	./srcs/child.c			\
	./srcs/get_next_line.c	\
	./srcs/handle_error.c	\
	./srcs/img-search.c		\
	./srcs/shared_mem.c		\
	./srcs/signals.c	

HEADERS =\
	./includes/img-search.h	\

LDFLAGS = -fsanitize=address

SRC 	:= $(notdir $(SRCS))
OBJ		:= $(SRC:%.c=%.o)
OBJS	:= $(addprefix $(OBJDIR), $(OBJ))

VPATH := $(SRCDIR) $(OBJDIR) $(shell find $(SRCDIR) -type d)

# RULES

all: $(NAME)

$(OBJDIR)%.o : %.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(SRCS) $(OBJS)
	@echo "img-search compiling"
	@$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

clean:
	@echo "cleaning up img-search object files"
	@cd 
	@rm -rf $(OBJDIR)

fclean: clean
	@echo "cleaning up img-search executable"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re