Gpp = g++-7 -g -Wall -std=c++17 -I../../PEGTL/include/tao -I. -Ifmt
trgt = ohmpp
srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

$(trgt): $(objs)
	$(Gpp) $^ -o $@

%.o: %.cpp
	$(Gpp) -MMD -MP -c $< -o $@

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(objs) $(deps) $(trgt)

-include $(deps)