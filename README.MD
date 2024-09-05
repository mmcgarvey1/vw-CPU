====OVERVIEW====

toy cpu with:
* 16-bit address space
* 8 single-byte registers*
* 16-bit PC and SP
* support for interfacing with up to 256 devices
* custom assembly-esque language
* 256x256 display with 2-bit color

====INSTRUCTIONS====
```
OPCODE(')   NAME        ARG1        ARG2        SIZE (b)

0x0         NUL         -           -           1
0x0'        INT         $imm3       -           1           
0x1         MW          r           $imm8/r     2
0x2         LW          r           $imm16/r    2/3
0x3         SW          $imm16/r    r           2/3
0x4         LPC         r           -           1
0x4'        xxx         x           x           x
0x5         IN          r           r/$imm8     2
0x6         OUT         r/$imm8     r           2
0x7         _JNZ        r/$imm8     -           2
0x8         CMP         r           r/$imm8     2
0x9         ADC         r           r/$imm8     2
0xA         SBB         r           r/$imm8     2
0xB         AND         r           r/$imm8     2
0xC         XOR         r           r/$imm8     2
0xD         PUSH        r           -           1
0xD'        xxx         x           x           x
0xE         POP         -/r         -           1
0xF         HLT         -           -           1
0xF'        SHR         r           -           1
```
====MACROS====
```
JMP
JNZ
CALL
CNZ
ADD
SUB
DB
static
```
====MEMORY====
```
0x0000 -> 0x7FFF: ROM
0x8000 -> 0x9FFF: RAM
0xA000 -> 0xAFFF: VIDEO MEMORY
0xB000 -> 0xBFFF: CHARACTER DATA
0xC000 -> 0xC0FF: CHARACTER BUFFER
0xC100 -> 0xDFFF: UNUSED
0xE000 -> 0xEFFF: VIDEO VARIABLES
0xFF01 -> 0xFFFF: STACK
```
====FLAGS====
```
F: xyzwabcd

FS READ:
a - R SUCCESSFUL
b - UNUSED
c - BLOCK NOT IN USE
d - UNUSED

FS WRITE:
a - W SUCCESSFUL
b - CREATE FILE
c - BLOCK NOT IN USE
d - UNUSED
```

====INTERRUPTS====
```
CODE        NAME        DESC

000         FS_READ     READ BLOCK [A], BYTE [D], READ TO [Z]
001         FS_WRITE    WRITE BLOCK [A], BYTE [D], FROM [Z]
010         FS_CREATE   CREATE BLOCK, STORE INDEX IN [Z]
```

====WRITING PROGRAMS====

values:

    all values are prefixed by a `$`. 

    hex values are prefixed by `$0x`. 

    bin values are prefixed by `$0b`.

direct memory modification:

    define a byte in memory using keyword `DB`:

    `DB $address $value`

variables:

    define a "variable" using keyword `static`:

    `static variable $value`

    you can use this variable by typing its name in the place of a constant, and appending with @.

    no checks are done to determine if the variable is 16-bit or 8-bit. 

    you can use them for both.

    `MW A variable@` (will use lower byte as 8-bit word)

    `SW variable@ A` (will interpret as 16-bit address)

labels:

    labels can be made with a single string with no whitespace appended by a ':'.

    `label:`

    labels can be used as jump points (using `JMP` or `JNZ`) or functions (`CALL` or `CNZ`). 

    no distinction is made, but functions expect the keyword `RET`.

display:

    each byte of memory from `$0xA000` to `$0xAFFF` contains the data of four pixels, 
    top left to bottom right being MSB to LSB.






