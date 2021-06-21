SHELL = /bin/bash

CXX       ?= g++-10
CFLAGS    ?= -fPIC -O3 -std=c++2a -Wall -Wextra -pedantic

SRC       ?= libringbuf/*.cpp


all:

clean:
	rm -rf build
	rm -rf *.so

setup_venv:
	python3 -m venv venv \
	  && source ./venv/bin/activate \
	  && pip install cython pytest


build_libringbuf:
	$(CXX) -I includes/ $(CFLAGS) $(SRC) -shared -o libringbuf.so


dist_py:
	source venv/bin/activate && \
	  export LDFLAGS="-L." && \
	  python setup.py build_ext --inplace


utest:
	source venv/bin/activate && \
	  export LD_LIBRARY_PATH="." && \
	  pytest test.py
