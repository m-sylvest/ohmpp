GPP = g++-7 -g -Wall -std=c++17 -I../../PEGTL/include/tao -I. -I ./Catch2-2.0.1/single_include/
trgt = ohmpp
srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

$(trgt): $(objs)
	$(GPP) $^ -o $@

%.o: %.cpp
	$(GPP) -MMD -MP -c $< -o $@

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(objs) $(deps) $(trgt)

-include $(deps)