# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/11 16:36:16 by fmotte            #+#    #+#              #
#    Updated: 2026/04/22 14:08:35 by erpascua         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# =======================================
#            MAKEFILE PROJET
# =======================================

# =======================================
#                FLAGS
# =======================================
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98 -g


# =======================================
#              FILE
# =======================================
FILE_NAMES =	main

CLASS_FILE =	Webserv \
				Server \
				Location \
				Client \
				HttpRequest \
				utils_connection \
				utils_duplicate \
				utils_parsing \
				execption \
				struct \

ABSTR_FILE = 	

TEMPL_FILE =

# =======================================
#              VARIABLE
# =======================================
SRC_PATH = src
OBJ_PATH = obj
HEA_PATH = include

SRC_FILES = $(FILE_NAMES:%=$(SRC_PATH)/%.cpp) $(CLASS_FILE:%=$(SRC_PATH)/%.cpp)
OBJ_FILES = $(FILE_NAMES:%=$(OBJ_PATH)/%.o) $(CLASS_FILE:%=$(OBJ_PATH)/%.o)
HEA_FILES = $(CLASS_FILE:%=$(HEA_PATH)/%.hpp) $(ABSTR_FILE:%=$(HEA_PATH)/%.hpp) $(TEMPL_FILE:%=$(HEA_PATH)/%.tpp) $(TEMPL_FILE:%=$(HEA_PATH)/%.hpp)
DEP_FILES = $(OBJ_FILES:.o=.d)

NAME =  Webserv


# =======================================
#              RULES
# =======================================
.PHONY: all clean fclean re git-setup

all : $(NAME)

$(OBJ_PATH) :
	@mkdir -p $(OBJ_PATH)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEA_FILES) | $(OBJ_PATH)
	@$(CXX) $(CXXFLAGS) -I $(HEA_PATH) -O3 -c $< -o $@


$(NAME): $(OBJ_FILES) 
	@$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(NAME)
	@echo "Fichier compile"
	
clean :
	@rm -f $(OBJ_FILES) $(DEP_FILES)
	@echo "Fichier object supprime"

fclean : clean	
	@rm -f $(NAME)
	@echo "Fichier executable supprime"

re : fclean all

git-setup:
	@sh doc/scripts/setup-git-ignore-42-header.sh
	@sh doc/scripts/setup-git-hooks.sh

serv: re
	./$(NAME) doc/parsing_tests/valid_minimal.conf

nc:
	nc -C 127.0.0.1 8080

c:
	curl localhost:8080 -H @doc/request
	
# Automatic inclusion of .d files if they exist
-include $(DEP_FILES)
