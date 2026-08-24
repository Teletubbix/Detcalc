CC      ?= cc
CFLAGS  ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
TARGET  := Detcalc

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c -o $(TARGET) -lm

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@printf '2 4\n1 2 3 4\n5 6 7 8\n1 0 0 1 1 0 0 1\n0\n' | ./$(TARGET) | grep -q 'det = -16' && echo "PASS  交互 [2x4]*[4x2] det=-16" || { echo "FAIL 交互2x4"; exit 1; }
	@printf '2 4 3 2\n1 0 0 0\n0 1 0 0\n1 0 0\n0 1 0\n0 0 1\n0 0 0\n1 0\n0 1\n0 0\n0\n' | ./$(TARGET) | grep -q 'det = 1' && echo "PASS  交互 乘法链 det=1" || { echo "FAIL 链"; exit 1; }
	@printf '1 2 3 4\n5 6 7 8\n1 0 0 1 1 0 0 1\n' | ./$(TARGET) 2 4 | grep -q 'det = -16' && echo "PASS  CLI 2 4 det=-16" || { echo "FAIL CLI 2x4"; exit 1; }
	@printf '1 2 3\n4 5 6\n7 8 10\n' | ./$(TARGET) 3 | grep -q 'det = -3' && echo "PASS  CLI 3 det=-3" || { echo "FAIL CLI 3"; exit 1; }

clean:
	rm -f $(TARGET)
