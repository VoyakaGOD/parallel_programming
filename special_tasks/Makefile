BUILD_FOLDER = ../executables
SHARED_FOLDER = ../shared

.PHONY: all clean

all: $(BUILD_FOLDER)/exp

$(BUILD_FOLDER)/exp: exp.cpp
	mpic++ exp.cpp -I$(SHARED_FOLDER) -o$@

clean:
	rm -rf $(BUILD_FOLDER)
