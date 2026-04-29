# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/11 16:36:16 by fmotte            #+#    #+#              #
#    Updated: 2026/04/29 05:43:32 by erpascua         ###   ########.fr        #
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
#              FILES
# =======================================
SRC_PATH = src
OBJ_PATH = obj
HEA_PATH = include

SRC_FILES = \
	$(SRC_PATH)/core/main.cpp \
	$(SRC_PATH)/core/Webserv.cpp \
	$(SRC_PATH)/core/execption.cpp \
	$(SRC_PATH)/core/struct.cpp \
	$(SRC_PATH)/config/Server.cpp \
	$(SRC_PATH)/config/Location.cpp \
	$(SRC_PATH)/config/utilsDuplicate.cpp \
	$(SRC_PATH)/config/utilsParsing.cpp \
	$(SRC_PATH)/network/Client.cpp \
	$(SRC_PATH)/network/utilsConnection.cpp \
	$(SRC_PATH)/http/HttpRequest.cpp \
	$(SRC_PATH)/request/utilsRequest.cpp

HEA_FILES = \
	$(HEA_PATH)/common/colors.hpp \
	$(HEA_PATH)/common/execption.hpp \
	$(HEA_PATH)/core/Webserv.hpp \
	$(HEA_PATH)/core/struct.hpp \
	$(HEA_PATH)/config/Server.hpp \
	$(HEA_PATH)/config/Location.hpp \
	$(HEA_PATH)/config/utilsDuplicate.hpp \
	$(HEA_PATH)/config/utilsParsing.hpp \
	$(HEA_PATH)/network/Client.hpp \
	$(HEA_PATH)/network/utilsConnection.hpp \
	$(HEA_PATH)/http/HttpRequest.hpp \
	$(HEA_PATH)/request/utilsRequest.hpp

OBJ_FILES = $(SRC_FILES:$(SRC_PATH)/%.cpp=$(OBJ_PATH)/%.o)
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
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -I $(HEA_PATH) \
		-I $(HEA_PATH)/common \
		-I $(HEA_PATH)/config \
		-I $(HEA_PATH)/core \
		-I $(HEA_PATH)/http \
		-I $(HEA_PATH)/network \
		-I $(HEA_PATH)/request \
		-O3 -c $< -o $@


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
	./$(NAME) doc/config_file

nc:
	nc -C 0.0.0.0 8080

c:
	curl localhost:8080 -H @doc/request
	
# Automatic inclusion of .d files if they exist
-include $(DEP_FILES)
