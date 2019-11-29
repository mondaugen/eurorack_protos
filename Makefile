CFLAGS = -DPD -O2 -funroll-loops -fomit-frame-pointer \
    -Wall -W -Wshadow -Wstrict-prototypes -Werror \
    -Wno-unused -Wno-parentheses -Wno-switch -fPIC

INCLUDE =  -I/usr/local/include/pd -Iadaptive_pitch_tracker_extracter/src/lib/datastructures/

bin/pqueue~.pd_linux : src/pqueue~.c \
    adaptive_pitch_tracker_extracter/src/lib/datastructures/fixed_heap.c \
    adaptive_pitch_tracker_extracter/src/lib/datastructures/fixed_heap_f32.c
	cc $(CFLAGS) $(INCLUDE) -c $^
	ld -shared -o $@ $(notdir $(patsubst %.c, %.o, $^)) -lc -lm
	strip --strip-unneeded $@
	rm $(notdir $(patsubst %.c, %.o, $^))
	cp -f $@ $(HOME)/Documents/Pd/externals/
