CC:=g++
exe:=example 
obj:=example.o

all:$(obj)
	$(CC) -o $(exe) $(obj) -pthread
%.o:%.cpp
	$(CC) -c $^ -o $@

.PHONY:clean
clean:
	rm -rf $(obj) $(exe)
