CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++11

SRC_DIR = src

TARGET = my_program

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
