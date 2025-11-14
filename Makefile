CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-ljpeg
MANDEL_SOURCES= mandel.c jpegrw.c 
MANDELMOVIE_SOURCES= mandelmovie.c
MANDEL_OBJECTS = $(MANDEL_SOURCES:.c=.o)
MANDELMOVIE_OBJECTS = $(MANDELMOVIE_SOURCES:.c=.o)
EXECUTABLES= mandel mandelmovie

all: $(EXECUTABLES) 

# pull in dependency info for *existing* .o files
-include $(MANDEL_OBJECTS:.o=.d)
-include $(MANDELMOVIE_OBJECTS:.o=.d)

mandel: $(MANDEL_OBJECTS)
	$(CC) $(MANDEL_OBJECTS) $(LDFLAGS) -o mandel

mandelmovie: $(MANDELMOVIE_OBJECTS)
	$(CC) $(MANDELMOVIE_OBJECTS) $(LDFLAGS) -o mandelmovie

.c.o: 
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.d
