FLAGS := -Wall -Wextra -g -std=c++11

BUILD_DIR	:= build
SOURCE_DIR	:= src

SOURCES	:= $(wildcard $(SOURCE_DIR)/*.cpp $(SOURCE_DIR)/*.c)
OBJS 	:= $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES) )


CC = g++

all: debug 	

debug: $(BUILD_DIR) $(OBJS) 
	$(CC) $(FLAGS) $(OBJS) -o simulator.out

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) simulator.out

$(BUILD_DIR):
	mkdir -p $@
