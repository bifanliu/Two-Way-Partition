# define compiler
CXX = g++

# define compile tool
CXXFLAGS = -Wall -Wextra -std=c++11

# define target file
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

# define main
main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# define clean file
clean:
	rm -f $(OBJS) main Output.txt

# Verifier
Ver:
	./verifier benchmarks/case1.txt output.txt

EvalOne:
	grep -o '1' output.txt | wc -l
