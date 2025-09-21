# Makefile

# 编译器
CXX = g++
# 编译选项
CXXFLAGS = -std=c++11 -Wall -Wextra -g -pthread
# 源文件
SRCS = udpserver.cc udpclient.cc
# 目标文件
OBJS = $(SRCS:.cc=.o)
# 可执行文件
TARGETS = udp_server udp_client

# 默认目标
all: $(TARGETS)

# 服务器
udp_server: udpserver.o
	$(CXX) $(CXXFLAGS) -o $@ $< 

# 客户端
udp_client: udpclient.o
	$(CXX) $(CXXFLAGS) -o $@ $<

# 编译规则
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 简化编译：自动依赖头文件
DEPENDENCIES = udpserver.hpp Loger.hpp
UdpServer.o: $(DEPENDENCIES)
UdpClient.o: $(DEPENDENCIES)

# 清理
clean:
	rm -f $(OBJS) $(TARGETS) *.log

# 重新编译
rebuild: clean all

# 查看目标
list:
	@echo "Available targets: $(TARGETS)"

.PHONY: all clean rebuild list
