BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = sort_net

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	mkdir -p $(BUILD_FOLDER)
	g++ -mavx512vl -mavx512dq $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
