BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = conveq pconveq

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp config.hpp
	mpic++ $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
