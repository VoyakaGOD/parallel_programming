BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = sint

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp config.hpp
	g++ $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
