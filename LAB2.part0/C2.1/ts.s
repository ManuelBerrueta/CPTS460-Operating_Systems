/************ ts.s file of C2.1 ***********/
.text
.global start

start:
        mov r0, #1 @ r0 = 1
        MOV R1, #2 @ r1 = 2
        ADD R1, R1, R0 // r1 = r1 + r0
        ldr r2, =result // r2 = &result
        str r1, [r2] /* result = r1 */

stop:   b stop
        .data

result: .word 0 /* a word location */

/* Notes on assembly:
        In assembly, instructions are NOT case sensitive.
        That is, an instruction can use upper, lower or mixed cases.
        However, you should choose one and stick with it, for x86 assembly
        I stick with lower case.

        The sample code above is a great example on how NOT to write assembly!

        For SYMBOLS, e.g. memory locations ARE CASE SENSITIVE.

        Also comments can be made in the following ways as show above:
            @   for inline comment
            //  for inline comment
            Also for block comments use the C style syntax
*/