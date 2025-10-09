BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = matmul

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp matrix.hpp matrix32.hpp
	mkdir -p $(BUILD_FOLDER)
	g++ -fopenmp -mavx512vl -mavx512dq $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
