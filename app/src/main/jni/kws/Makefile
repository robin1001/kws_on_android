CXX = g++

CXXFLAGS = -g -std=c++11 -I . #-lpthread -msse4.1 -lopenblas -D USE_BLAS

OBJ = fsm.o

BIN = tools/fsm-info tools/fsm-init

TEST = 

all: $(TEST) $(BIN) $(OBJ)

test/%: test/%.cc $(OBJ)
	$(CXX) $< $(OBJ) $(CXXFLAGS) -o $@

tools/%: tools/%.cc $(OBJ)
	$(CXX) $< $(OBJ) $(CXXFLAGS) -o $@

.PHONY: clean

clean:
	rm -rf $(OBJ); rm -rf $(TEST)

