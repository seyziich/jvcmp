include $(BASE_DIR)/makefile.defs

all: shared static

shared:
	$(CC) $(DEBUG) -I$(INCLUDE) -w -c *.cpp
	$(CC) $(DEBUG) -shared -Wl-soname,libraknet.so.2 -o $(BASE_DIR)/lib/linux/libraknet.so.$(VERSION) *.o $(LIBS)

static:
	$(CC) $(DEBUG) -I$(INCLUDE) -w -c *.cpp
	$(AR) rcs $(BASE_DIR)/lib/linux/libraknet.a *.o

clean:
	rm -f core
	rm -f *.o
	rm -f $(BASE_DIR)/lib/linux/libraknet.so.$(VERSION)
	rm -f $(BASE_DIR)/lib/linux/linuxraknet.a
