TARGET = bin/practice_mode

CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -I. -O2 -w
# libraries
LDFLAGS = -lncurses

# directories
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

# finds all file in src/ and transforms them in .o
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))


all: directories $(TARGET)

# linker
$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Done, created exec in: $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# create bin and object directories
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)


clean:
	@echo "cleaning..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: $(TARGET)
	@./$(TARGET)

.PHONY: all clean directories