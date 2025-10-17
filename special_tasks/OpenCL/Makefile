BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = ocl_avg ocl_sort_net

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp ocl.cpp
	mkdir -p $(BUILD_FOLDER)
	g++ $< ocl.cpp -I$(SHARED_FOLDER) -lOpenCL -o $@

clean:
	rm -rf $(BUILD_FOLDER)
