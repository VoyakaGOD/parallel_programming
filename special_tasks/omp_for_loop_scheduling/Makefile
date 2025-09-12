BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = omp_for

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	mkdir -p $(BUILD_FOLDER)
	g++ -fopenmp $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
