CC      ?= cc
CFLAGS  ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
TARGET  := Detcalc

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c -o $(TARGET) -lm

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@printf '2 4\n1 2 3 4\n5 6 7 8\n1 0 0 1 1 0 0 1\n0\n' | ./$(TARGET) | grep -q 'det = -16' && echo "PASS  交互 [2x4]*[4x2] det=-16" || { echo "FAIL 2x4"; exit 1; }
	@printf '2 4 3 2\n1 0 0 0\n0 1 0 0\n1 0 0\n0 1 0\n0 0 1\n0 0 0\n1 0\n0 1\n0 0\n0\n' | ./$(TARGET) | grep -q 'det = 1' && echo "PASS  交互 乘法链 det=1" || { echo "FAIL 链"; exit 1; }
	@printf '2\n1/2 1/3\n1/3 1/2\n0\n' | ./$(TARGET) | grep -q 'det = 5/36' && echo "PASS  分数精确 det=5/36" || { echo "FAIL 分数"; exit 1; }
	@printf '1/2 0\n0 2\n' | ./$(TARGET) 2 | grep -q 'det = 1' && echo "PASS  CLI 分数 det=1" || { echo "FAIL CLI 分数"; exit 1; }

clean:
	rm -f $(TARGET)
