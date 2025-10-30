BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = loop1 loop2 loop3

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	mkdir -p $(BUILD_FOLDER)
	mpic++ -fopenmp $< -I$(SHARED_FOLDER) -DISIZE=5001 -DJSIZE=5001 -o $@

clean:
	rm -rf $(BUILD_FOLDER)
