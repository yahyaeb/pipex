# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/16 17:10:45 by yel-bouk          #+#    #+#              #
#    Updated: 2025/02/16 17:37:38 by yel-bouk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_libft.a  # Fixed variable name (was Name)
CC = gcc           # Fixed CC (was CC, which is incorrect)
CFLAGS = -Wall -Wextra -Werror
AR = ar rcs

LIBFT_DIR = libft
PRINTF_DIR = ft_printf

LIBFT = $(LIBFT_DIR)/libft.a
PRINTF = $(PRINTF_DIR)/libftprintf.a  # Fixed file name (was libftprint.a)

all: $(LIBFT) $(PRINTF) $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(PRINTF):
	make -C $(PRINTF_DIR)

$(NAME): $(LIBFT) $(PRINTF)
	cp $(LIBFT) $(NAME)  # Copy libft.a to create ft_libft.a
	$(AR) $(NAME) $(PRINTF_DIR)/srcs/*.o  # Append ft_printf object files

clean:
	make -C $(LIBFT_DIR) clean
	make -C $(PRINTF_DIR) clean
	
fclean: clean
	make -C $(LIBFT_DIR) fclean
	make -C $(PRINTF_DIR) fclean
	rm -f $(NAME)

re: fclean all
