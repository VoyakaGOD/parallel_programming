BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = omp_copyprivate omp_copyin

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	mkdir -p $(BUILD_FOLDER)
	g++ -fopenmp $< -I$(SHARED_FOLDER) -o $@
	g++ -fopenmp $< -I$(SHARED_FOLDER) -DCPY -o $@_cpy

clean:
	rm -rf $(BUILD_FOLDER)
