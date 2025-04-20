BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
SRC = grid.cpp CLI.cpp patterns_lib.cpp
HEADERS = grid.hpp patterns_lib.hpp CLI.hpp
TASKS = gol mgol

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp $(SRC) $(HEADERS)
	mpic++ $< $(SRC) -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
