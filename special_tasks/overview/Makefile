BUILD_FOLDER = ../../executables
SHARED_FOLDER = ../../shared
TASKS = spawn spawn.aux files one-sided client-server

.PHONY: all clean

all: $(addprefix $(BUILD_FOLDER)/, $(TASKS))

$(BUILD_FOLDER)/%: %.cpp
	mpic++ $< -I$(SHARED_FOLDER) -o $@

clean:
	rm -rf $(BUILD_FOLDER)
