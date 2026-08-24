CC      ?= cc
CFLAGS  ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
TARGET  := detcalc

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c -o $(TARGET) -lm

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	@printf '2 4\n1 2 3 4\n5 6 7 8\n1 0\n0 1\n1 0\n0 1\n0\n' | ./$(TARGET) | grep -q 'det(C) = -16' && echo "PASS  [2x4]*[4x2] det=-16" || { echo "FAIL 2x4"; exit 1; }
	@printf '4 2\n1 2\n3 4\n5 6\n7 8\n1 0 0 0\n0 1 0 0\n0\n' | ./$(TARGET) | grep -q 'det(C) = 0' && echo "PASS  [4x2]*[2x4] det=0" || { echo "FAIL 4x2"; exit 1; }
	@printf '2\n1 2\n3 4\n0\n' | ./$(TARGET) | grep -q 'det = -2' && echo "PASS  2x2 det=-2" || { echo "FAIL 2x2"; exit 1; }
