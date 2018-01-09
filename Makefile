trgt = ohmpp
srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

jsonver = 3.0.1

pegtlver = 2.3.3
pegtlbase = PEGTL-$(pegtlver)
pegtltgz = ./$(pegtlbase).tgz
pegtldir = ./$(pegtlbase)/include/tao

Catch2ver = 2.0.1

GPP = g++-7 -g -Wall -std=c++17 -I . -I $(pegtldir) 


all:  json/json.hpp $(pegtldir)/pegtl.hpp catch/catch.hpp $(trgt) 
	
json/json.hpp:	
	mkdir json || true
	wget -O json/json.hpp https://github.com/nlohmann/json/releases/download/v$(jsonver)/json.hpp
	
$(pegtldir)/pegtl.hpp:
	wget -O ./$(pegtltgz) https://github.com/taocpp/PEGTL/archive/$(pegtlver).tar.gz
	tar xvf $(pegtltgz)

catch/catch.hpp:
	mkdir catch || true
	wget -O catch/catch.hpp https://github.com/catchorg/Catch2/releases/download/v$(Catch2ver)/catch.hpp

$(trgt): $(objs)
	$(GPP) $^ -o $@

%.o: %.cpp
	$(GPP) -MMD -MP -c $< -o $@

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(objs) $(deps) $(trgt)

-include $(deps)