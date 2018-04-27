CXX = g++

CXXFLAGS = -g -std=c++11 -I . #-lpthread -msse4.1 -lopenblas -D USE_BLAS

OBJ = fsm.o

BIN = tools/fsm-info tools/fsm-init

SRC = utils.h net.h net.cc fft.h fft.cc fbank.h \
      feature-pipeline.h feature-pipeline.cc \
      fst.h fst.cc symbol-table.h \
      keyword-spot.h kws.h kws.cc

all: $(TEST) $(BIN) $(OBJ)

check:
	for file in $(SRC); do \
		echo $$file; \
        cpplint --filter=-build/header_guard,-readability/check,-build/include_subdir $$file; \
	done

test/%: test/%.cc $(OBJ)
	$(CXX) $< $(OBJ) $(CXXFLAGS) -o $@

tools/%: tools/%.cc $(OBJ)
	$(CXX) $< $(OBJ) $(CXXFLAGS) -o $@

.PHONY: clean

clean:
	rm -rf $(OBJ); rm -rf $(TEST)

