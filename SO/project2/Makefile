CC = gcc

# Para mais informações sobre as flags de warning, consulte a informação adicional no lab_ferramentas
CFLAGS = -g -std=c17 -D_POSIX_C_SOURCE=200809L -I. \
		 -Wall -Wextra \
		 -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wnull-dereference -Wshadow -Wsign-conversion -Wswitch-enum -Wundef -Wunreachable-code -Wunused \
		 -pthread \
		 -fsanitize=thread


ifneq ($(shell uname -s),Darwin) # if not MacOS
	CFLAGS += -fmax-errors=5
endif

all: server/ems client/client

server/ems: common/io.o common/constants.h server/main.c server/operations.o server/eventlist.o
	$(CC) $(CFLAGS) $(SLEEP) -o $@ $^

client/client: common/io.o client/main.c client/api.o client/parser.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c ${@:.o=.c} -o $@

run: 
	server/./ems server.fifo &
	sleep 1
	client/./client req.fifo resp.fifo server.fifo /home/pedroleal/SO/p2_base/jobs/a.jobs & 
	client/./client req.fifo2 resp.fifo2 server.fifo /home/pedroleal/SO/p2_base/jobs/b.jobs &
	client/./client req.fifo3 resp.fifo3 server.fifo /home/pedroleal/SO/p2_base/jobs/c.jobs &
	client/./client req.fifo4 resp.fifo4 server.fifo /home/pedroleal/SO/p2_base/jobs/e.jobs &
	client/./client req.fifo5 resp.fifo5 server.fifo /home/pedroleal/SO/p2_base/jobs/test.jobs 

clean:
	rm -f common/*.o client/*.o server/*.o server/ems client/client ems jobs/*.out

format:
	@which clang-format >/dev/null 2>&1 || echo "Please install clang-format to run this command"
	clang-format -i common/*.c common/*.h client/*.c client/*.h server/*.c server/*.h
