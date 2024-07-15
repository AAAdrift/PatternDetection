# 定义编译器
CC=gcc

# 定义编译选项
CFLAGS=-Wall -Wextra -I/usr/include/mysql -DDEBUG=1  # 添加其他需要的编译选项

# 定义链接选项
LDFLAGS=-lpcap -lmysqlclient

# 定义目标文件
TARGET=my_program

# 默认目标
all: $(TARGET)

# 目标依赖于源文件
$(TARGET): acsm.o PatternDetection.o
	$(CC)  -o $@ $^ $(LDFLAGS)

# 从 acsm.c 生成 acsm.o
acsm.o: acsm.c acsm.h
	$(CC) $(CFLAGS) -c $< -o $@

# 从 PatternDetection.c 生成 PatternDetection.o
PatternDetection.o: PatternDetection.c acsm.h
	$(CC) $(CFLAGS) -c $< -o $@

# 清理编译生成的文件
clean:
	rm -f $(TARGET) *.o

# 声明伪目标
.PHONY: all clean
