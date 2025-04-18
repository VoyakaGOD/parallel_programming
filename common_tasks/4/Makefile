BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = pthello

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	g++ $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
