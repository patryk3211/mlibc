.section .text
.global _start

_start:
    mov $main, %rdi
    mov %rsp, %rsi
    call __mlibc_entry

.size _start, . - _start

.section .note.GNU-stack,"",%progbits
