.PHONY: all clean
all: 
	make -C support
	make -C grid
	make -C server
	make -C player

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C grid clean
	make -C server clean
	make -C player clean
	make -C support clean

