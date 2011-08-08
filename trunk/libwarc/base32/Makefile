CFLAGS += -g -Wall -W -pedantic -std=c99

.PHONY: clean test distclean

encoder: base32.o
base32.o: base32.h

test: encoder
	[ `echo foo | ./encoder` == "MZXW6CQ=" ]
	[ `echo MZXW6CQ= | ./encoder -d` == "foo" ]
	./encoder encoder | ./encoder -d | cmp encoder
	@echo "*** All test passed successfully"

clean:
	$(RM) *.o

distclean: clean
	$(RM) encoder
