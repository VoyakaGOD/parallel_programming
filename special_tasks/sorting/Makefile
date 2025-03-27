BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = parallel_merge_sort

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	mkdir -p $(BUILD_FOLDER)
	mpic++ $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
