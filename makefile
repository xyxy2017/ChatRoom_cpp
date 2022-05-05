all: server client
LDFLAGS:= -pthread

.PHONY: clean
clean:
	rm server client
