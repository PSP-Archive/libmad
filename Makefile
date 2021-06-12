BINARY = out

OBJECTS = startup.o playmp3.o main.o misc.o

LIBRARY = libmad.a

all: $(BINARY)

$(BINARY): $(OBJECTS)
	ee-ld -s -O0 $(OBJECTS) $(LIBRARY) -M -Ttext 8804000 -q -o $@ > mp3test.map
	outpatch USERPROG
	elf2pbp outp "libmad test"

%.o : %.c
	ee-gcc -march=r4000 -O3 -fomit-frame-pointer -mgp32 -mlong32 -c $< -o $@

%.o : %.s
	ee-gcc -march=r4000 -mgp32 -c -xassembler -O -o $@ $<

clean:
	del /f *.o *.map out outp eboot.pbp
