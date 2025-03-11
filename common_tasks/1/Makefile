BUILD_FOLDER = exe
SHARED_FOLDER = ../../shared

.PHONY: all

all: $(BUILD_FOLDER)/hello

$(BUILD_FOLDER)/hello: hello.cpp
	mkdir -p $(BUILD_FOLDER)
	mpic++ hello.cpp -I$(SHARED_FOLDER) -o$(BUILD_FOLDER)/hello

