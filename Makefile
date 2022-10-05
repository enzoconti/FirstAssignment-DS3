.PHONY: all
all: bin/programaTrab

bin/programaTrab: obj/fileRoutines.o obj/functinalities.o obj/structCode.o main.c
	mkdir -p bin
	gcc obj/fileRoutines.o obj/functinalities.o obj/structCode.o main.c -I include -o bin/programaTrab

obj/fileRoutines.o: src/fileRoutines.c include/fileRoutines.h
	mkdir -p obj
	gcc -c src/fileRoutines.c -I include -o obj/fileRoutines.o

obj/functinalities.o: src/functinalities.c include/functinalities.h
	mkdir -p obj
	gcc -c src/functinalities.c -I include -o obj/functinalities.o

obj/structCode.o: src/structCode.c include/structCode.h
	mkdir -p obj
	gcc -c src/structCode.c -I include -o obj/structCode.o

.PHONY: run
run:
	./bin/programaTrab

.PHONY: clean
clean:
	rm -rf obj bin