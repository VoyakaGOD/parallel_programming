BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
SRC = grid.cpp
HEADERS = grid.hpp
TASKS = gol

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp $(SRC) $(HEADERS)
	mpic++ $< $(SRC) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
