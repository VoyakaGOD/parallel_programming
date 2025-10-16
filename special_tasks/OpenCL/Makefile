BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = ocl_avg

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	mkdir -p $(BUILD_FOLDER)
	g++ $< -I$(SHARED_FOLDER) -lOpenCL -o $@

clean:
	rm -rf $(BUILD_FOLDER)
