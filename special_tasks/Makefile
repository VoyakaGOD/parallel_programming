BUILD_FOLDER = ../executables
SHARED_FOLDER = ../shared
SRC = exp.cpp $(SHARED_FOLDER)/big_decimal.cpp
HEADERS = $(SHARED_FOLDER)/big_decimal.hpp

.PHONY: all clean

all: $(BUILD_FOLDER)/exp $(BUILD_FOLDER)/ptattr

$(BUILD_FOLDER)/exp: $(SRC) $(HEADERS)
	mpic++ $(SRC) -I$(SHARED_FOLDER) -o$@

$(BUILD_FOLDER)/ptattr: ptattr.cpp
	g++ $< -o$@

clean:
	rm -rf $(BUILD_FOLDER)
