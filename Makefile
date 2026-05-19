# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/11 16:36:16 by fmotte            #+#    #+#              #
#    Updated: 2026/05/14 19:39:40 by fmotte           ###   ########.fr        #
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

# **************************************************************************** #
# SOURCES
# **************************************************************************** #

CORE_SRC = \
	$(SRC_PATH)/core/main.cpp \
	$(SRC_PATH)/core/Webserv.cpp \
	$(SRC_PATH)/core/execption.cpp \
	$(SRC_PATH)/core/struct.cpp

CONFIG_SRC = \
	$(SRC_PATH)/config/Server.cpp \
	$(SRC_PATH)/config/Location.cpp \
	$(SRC_PATH)/config/utilsDuplicate.cpp \
	$(SRC_PATH)/config/utilsParsing.cpp \

NETWORK_SRC = \
	$(SRC_PATH)/network/Client.cpp \
	$(SRC_PATH)/network/utilsConnection.cpp

HTTP_SRC = \
	$(SRC_PATH)/http/HttpRequest.cpp \
	$(SRC_PATH)/http/utilsRequest.cpp \
	$(SRC_PATH)/http/Header.cpp	\
	$(SRC_PATH)/http/Body.cpp

#$(SRC_PATH)/http/HttpResponse.cpp
#$(SRC_PATH)/http/utilsResponse.cpp

METHOD_SRC = \
	$(SRC_PATH)/method/AMethod.cpp \
	$(SRC_PATH)/method/GetMethod.cpp \
	$(SRC_PATH)/method/DeleteMethod.cpp \
	$(SRC_PATH)/method/PostMethod.cpp \
	$(SRC_PATH)/method/HeadMethod.cpp


SRC_FILES = \
	$(CORE_SRC) \
	$(CONFIG_SRC) \
	$(NETWORK_SRC) \
	$(HTTP_SRC) \
	$(METHOD_SRC)

# **************************************************************************** #
# HEADERS
# **************************************************************************** #

COMMON_HEA = \
	$(HEA_PATH)/common/colors.hpp \
	$(HEA_PATH)/common/execption.hpp

CORE_HEA = \
	$(HEA_PATH)/core/Webserv.hpp \
	$(HEA_PATH)/core/struct.hpp

CONFIG_HEA = \
	$(HEA_PATH)/config/Server.hpp \
	$(HEA_PATH)/config/Location.hpp \
	$(HEA_PATH)/config/utilsDuplicate.hpp \
	$(HEA_PATH)/config/utilsParsing.hpp

NETWORK_HEA = \
	$(HEA_PATH)/network/Client.hpp \
	$(HEA_PATH)/network/utilsConnection.hpp

HTTP_HEA = \
	$(HEA_PATH)/http/HttpRequest.hpp \
	$(HEA_PATH)/http/utilsRequest.hpp \
	$(HEA_PATH)/http/Header.hpp \
	$(HEA_PATH)/http/Body.hpp  

#$(HEA_PATH)/http/HttpResponse.hpp
#$(HEA_PATH)/http/utilsResponse.hpp

METHOD_HEA = \
	$(HEA_PATH)/method/AMethod.hpp \
	$(HEA_PATH)/method/GetMethod.hpp \
	$(HEA_PATH)/method/DeleteMethod.hpp \
	$(HEA_PATH)/method/PostMethod.hpp \
	$(HEA_PATH)/method/HeadMethod.hpp 


HEA_FILES = \
	$(COMMON_HEA) \
	$(CORE_HEA) \
	$(CONFIG_HEA) \
	$(NETWORK_HEA) \
	$(HTTP_HEA) \
	$(METHOD_HEA)

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
		-I $(HEA_PATH)/method \
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
