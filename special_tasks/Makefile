BUILD_FOLDER = ../executables
SHARED_FOLDER = ../shared
SRC = exp.cpp $(SHARED_FOLDER)/big_decimal.cpp

.PHONY: all clean

all: $(BUILD_FOLDER)/exp

$(BUILD_FOLDER)/exp: $(SRC)
	mpic++ $(SRC) -I$(SHARED_FOLDER) -o$@

clean:
	rm -rf $(BUILD_FOLDER)
