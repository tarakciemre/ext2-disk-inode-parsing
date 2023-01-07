.PHONY: run
.PHONY: rebuild

diskprint: $(reference)
	gcc -o diskprint $(reference) -lm

run: diskprint
	./diskprint disk1

rebuild:
	make clean
	make run

clean:
	rm diskprint

reference = diskprint.c