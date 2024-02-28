## CSC 360: Spring 2024

### Assignment #1 evaluation

* Student name: Jonathan Cruikshanks
* Student netlink: jcruikshanks
* Student number:  V00962128

---

### Part 1: `fetch-info.c`

* Submitted and compiles without errors or warnings: yes
* `./fetch-info` (without arguments): good
* `./fetch-info <pnum>` (process number of `conda`): good
* `./fetch-info 9999` (expect error message for invalid number): good


**Regarding code structure of solution to part 1:**

You've structured the program around a nice functional decomposition
. Such code is definitely easier to
maintain and extend. Good work.


---

### Part 2: `pipe4.c`

* Submitted and compiles without errors or warnings: yes
* Single command w/o arguments: good
* Single command w/ arguments: good
* Two commands: good
* Three commands: good
* Four commands: good

Here is the four-command pipe as it would be expressed in `bash` (but
of course broken up line-by-line for use with `pipe4`):
```
    /usr/bin/ls -1 /usr/lib | /usr/bin/sort  \
         | /usr/bin/tail -2 | /usr/bin/wc
```

**Regarding code structure of solution to part 2:**

All of the most substantial code appears to be in the `main()`
function. That is not necessarily a problem for this assignment as the
harder piece if the pipes plus loops -- but in future do make sure you
use functional decomposition (as you did with the first part).

---

### Other evaluator comments

Try to avoid the use of unsafe C string functions such as `strcat` and
`strcpy`. Use `strncat` and `strncpy` instead.

---

### Assignment grade: A


