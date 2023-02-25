.section .text
.global _start

_start:
    lea main(%rip), %rdi
    call __mlibc_entry

.size _start, . - _start

.section .note.GNU-stack,"",%progbits

