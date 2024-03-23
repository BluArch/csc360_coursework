## CSC 360: Spring 2024

### Assignment #2 evaluation

* Student name: Jonathan Cruikshanks
* Student netlink: `jcruikshanks`
* Student number:  V00962128

---

### Part 1: `kosmos-sem.c`

* Submitted and compiles without errors or warnings: YES

If YES above, **program is executed using seed 122 and 80 atoms.**

* Program runs without crashing: YES

* Program creates at least one valid radical: YES

* Program create several valid radicals: YES

* Program creates all valid radicals: YES

* Only semaphores used in the solution: YES

* More than one semaphore appears to form the solution: YES

* Code appears to "clean up" atoms not forming a radical: YES

* Code is commented: YES


---

### Part 2: `kosmos-mcv.c`

* Submitted and compiles without errors or warnings: YES

If YES above, **program is executed using seed 122 and 80 atoms.**

* Program runs without crashing: YES

* Program creates at least one valid radical: YES

* Program create several valid radicals: YES

* Program creates all valid radicals: YES

* Only Pthread condition variables and mutexes used: YES

* More than one CV appears to form the solution: NO. Is there a reason
you chose this approach? Normally one would expect individual CVs
for the kinds of atoms (one for carbons, one for hydrogens, one for
oxygens). It isn't possible to guarantee that newly arriving atoms
won't be reawakened with a `broadcast`.

* Code is commented: YES


---

### Other evaluator comments

---

### Assignment grade: B+
