SHELL = /bin/bash

CXX       ?= g++-10
CFLAGS    ?= -O3 -std=c++2a -Wall -Wextra -pedantic

SRC       ?= main.cpp mapped.cpp ring_buffer.cpp utils.cpp

build:
	$(CXX) $(CFLAGS) $(SRC)

py:
	source venv/bin/activate && \
	  python setup.py build_ext --inplace
