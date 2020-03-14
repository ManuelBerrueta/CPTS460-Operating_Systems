   .global entryPoint, main, syscall, getmysp, getcpsr, getmyaddress
	.text
.code 32
// upon entry, bl main0 => r0 contains pointer to the string in ustack

entryPoint:	
	bl main
	
// if main0() ever retrun: syscall to exit(0)
	
@ user process issues int syscall(a,b,c,d) ==> a,b,c,d are in r0-r3	
syscall:

   stmfd sp!, {lr}
    swi #0
   ldmfd sp!, {pc}

getmysp:
   mov r0, sp
   mov pc, lr

getcpsr:
   mrs r0, cpsr
   mov pc, lr
getmyaddress:
   ldr r0, =entryPoint
   mov pc, lr
	
	
