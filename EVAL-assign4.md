## CSC 360: Spring 2024

### Assignment #4 evaluation

* Student name: Jonathan Cruikshanks
* Student netlink: `jcruikshanks`
* Student number:  V00962128

---

### Virtual-memory page-replacement simulation

* `virtmem.c` submitted and compiles without errors or warnings: YES

If YES above, then the following sequence of tests were performed.

(Note: For all of the following, the sole statistic examined was `Page
faults`. It appears there was too much ambiguity with the assignment
specification of the dirty bit and the meaning of swapouts to use
these as a basis for evaluation.)

* `fifo` with `FRAMESIZE` of 10 and `NUMFRAMES` at 70, 30, and 10,
using `hello-out.txt`.  Expected behavior seen? NO

* `lru` with `FRAMESIZE` of 9 and `NUMFRAMES` at 40 and 20, using
`hello-out.txt.  Expected behavior seen? NO

* `clock` with `FRAMESIZE` of 11 and `NUMFRAMES` at 30 and 15, using
`ls-out.txt`.  Expected behavior seen? YES NO

* `fifo`, `clock`, and `lru` with `FRAMESIZE` of 10 and `NUMFRAMES` at
12, using `matrixmult-out.txt`. Expected behavior seen? YES NO

* If NO above, is expected behavior largely seen when the values `FRAMESIZE` and
`NUMFRAMES` are varied? YES NO

* Code is commented in a sensible way: NO

---

### Other evaluator comments


All cases crashed after running three-fifo.sh.

```shell
jovyan@jupyter-haohushen:~/csc360-eval/jcruikshanks/assign4$ ./three-fifo.sh 
+ BIN=./virtmem
+ FRAMESIZE=10
+ REPLACE=fifo
+ TRACEFILE=traces/hello-out.txt
+ for NUMFRAMES in 70 30 10
+ ./virtmem --framesize=10 --numframes=70 --replace=fifo --file=traces/hello-out.txt
./three-fifo.sh: line 10:  1037 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
+ for NUMFRAMES in 70 30 10
+ ./virtmem --framesize=10 --numframes=30 --replace=fifo --file=traces/hello-out.txt
./three-fifo.sh: line 10:  1038 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
+ for NUMFRAMES in 70 30 10
+ ./virtmem --framesize=10 --numframes=10 --replace=fifo --file=traces/hello-out.txt
./three-fifo.sh: line 10:  1039 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
```

All cases crashed after running two-lru.sh.

```shell
jovyan@jupyter-haohushen:~/csc360-eval/jcruikshanks/assign4$ ./two-lru.sh 
+ BIN=./virtmem
+ FRAMESIZE=9
+ REPLACE=lru
+ TRACEFILE=traces/hello-out.txt
+ for NUMFRAMES in 40 20
+ ./virtmem --framesize=9 --numframes=40 --replace=lru --file=traces/hello-out.txt
./two-lru.sh: line 10:  1062 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
+ for NUMFRAMES in 40 20
+ ./virtmem --framesize=9 --numframes=20 --replace=lru --file=traces/hello-out.txt
./two-lru.sh: line 10:  1063 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
```

All cases crashed after running full-compare.sh and full-compare.sh 9 20.

```shell
jovyan@jupyter-haohushen:~/csc360-eval/jcruikshanks/assign4$ ./full-compare.sh 
+ BIN=./virtmem
+ FRAMESIZE=10
+ REPLACE=fifo
+ NUMFRAMES=12
+ TRACEFILE=traces/matrixmult-out.txt
+ for REPLACE in "fifo" "clock" "lru"
+ ./virtmem --framesize=10 --numframes=12 --replace=fifo --file=traces/matrixmult-out.txt
./full-compare.sh: line 16:  1081 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
+ for REPLACE in "fifo" "clock" "lru"
+ ./virtmem --framesize=10 --numframes=12 --replace=clock --file=traces/matrixmult-out.txt
./full-compare.sh: line 16:  1082 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
+ for REPLACE in "fifo" "clock" "lru"
+ ./virtmem --framesize=10 --numframes=12 --replace=lru --file=traces/matrixmult-out.txt
./full-compare.sh: line 16:  1083 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
jovyan@jupyter-haohushen:~/csc360-eval/jcruikshanks/assign4$ ./full-compare.sh 9 20
+ BIN=./virtmem
+ FRAMESIZE=9
+ REPLACE=fifo
+ NUMFRAMES=20
+ TRACEFILE=traces/matrixmult-out.txt
+ for REPLACE in "fifo" "clock" "lru"
+ ./virtmem --framesize=9 --numframes=20 --replace=fifo --file=traces/matrixmult-out.txt
./full-compare.sh: line 16:  1087 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
+ for REPLACE in "fifo" "clock" "lru"
+ ./virtmem --framesize=9 --numframes=20 --replace=clock --file=traces/matrixmult-out.txt
./full-compare.sh: line 16:  1088 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
+ for REPLACE in "fifo" "clock" "lru"
+ ./virtmem --framesize=9 --numframes=20 --replace=lru --file=traces/matrixmult-out.txt
./full-compare.sh: line 16:  1089 Segmentation fault      (core dumped) ./virtmem --framesize=$FRAMESIZE --numframes=$NUMFRAMES --replace=$REPLACE --file=$TRACEFILE
```



---

### Assignment grade: 40404040
