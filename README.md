# mumsh

**A developing bash in C.** [![Build Status](https://focs.ji.sjtu.edu.cn:2222/api/badges/ECE482-22/ZiningWang520370910042-p1/status.svg)](https://focs.ji.sjtu.edu.cn:2222/ECE482-22/ZiningWang520370910042-p1)

## Features

 - Redirection and Pipe support (`>` `>>` `<` `|`)
 - Detail grammar errors report (8 cases)
 - Multiple-line input support (Not complete command will ask for more in the next line)
 - background job support (use `&` as syntax)
 - quote support(`"` and `'` supported)

### `cd` command support

Absolute path and relative path are supported. 

If there is no args, the home path will be the default path.

If there are more than 1 args, the command will have effects.

Note : *`~` is not used as home path. Instead, it is viewed as a simple character.*

### `pwd` command support

Only simple `pwd` is supported.

### `jobs` command support

It will gives all the background jobs as well as state (`running` or `done`)

### errors report

case 1. trying to running not existing exec file.

case 2. trying to use input redirection of not existing file

case 3. trying to write or append to files that does not exist

case 4. trying to have more than one input redirection

case 5. trying to have more than one output redirection

case 6. trying to running commands with wrong syntax

case 7. trying to pipe to empty program (e.g. `echo test | | cat`)

case 8. trying to `cd` directory that does not exist.

## How to build
Use given `CMakeLists.txt` to compile. (If you failed to compile, use `-DCMAKE_C_COMPILER=clang`).


## How to use
This shell can be launched by `mumsh` if you use the given CMAKE file.




## Bug report

Now `mumsh` is in rapid development and there are many bugs that does not be found. If you find one bug, please use the given  `CMakeLists.txt` to compile and use `mumsh_memory_check` to reappear the bug and report if there are any issues with memory. If there is any problem with using issues, you can directly mailto:wznmickey@sjtu.edu.cn .