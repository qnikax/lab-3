# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror
LDFLAGS = 

# Директории
TASK1_DIR = task1
TASK2_DIR = task2
BUILD_DIR = build

# Цели
all: task1_test task2_test

# =========== ЗАДАНИЕ 1: Тесты modAlphaCipher ===========
$(BUILD_DIR)/modAlphaCipher.o: $(TASK1_DIR)/modAlphaCipher.cpp $(TASK1_DIR)/modAlphaCipher.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/task1_test.o: $(TASK1_DIR)/test.cpp $(TASK1_DIR)/modAlphaCipher.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

task1_test: $(BUILD_DIR)/modAlphaCipher.o $(BUILD_DIR)/task1_test.o
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

# =========== ЗАДАНИЕ 2: Тесты routeCipher ===========
$(BUILD_DIR)/routeCipher.o: $(TASK2_DIR)/routeCipher.cpp $(TASK2_DIR)/routeCipher.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/task2_test.o: $(TASK2_DIR)/test.cpp $(TASK2_DIR)/routeCipher.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

task2_test: $(BUILD_DIR)/routeCipher.o $(BUILD_DIR)/task2_test.o
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

# =========== ВСПОМОГАТЕЛЬНЫЕ ЦЕЛИ ===========
clean:
	rm -rf $(BUILD_DIR)/*

clean_all:
	rm -rf $(BUILD_DIR)

run_task1: task1_test
	@echo "=== Запуск тестов для шифра Гронсфельда ==="
	./$(BUILD_DIR)/task1_test

run_task2: task2_test
	@echo "=== Запуск тестов для шифра маршрутной перестановки ==="
	./$(BUILD_DIR)/task2_test

test: run_task1 run_task2
	@echo "=== Все тесты завершены ==="

.PHONY: all clean clean_all run_task1 run_task2 test
