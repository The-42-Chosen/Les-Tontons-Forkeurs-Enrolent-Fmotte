# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/09 14:44:48 by erpascua          #+#    #+#              #
#    Updated: 2026/06/09 14:47:16 by erpascua         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -O2

OUT_DIR  = ../webPageFile/localhost
TARGET   = $(OUT_DIR)/execCgi.cgi
SRC      = execCgi.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

re: clean all

.PHONY: all clean re
