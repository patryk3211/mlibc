.section .text
.global __mlibc_signal_restore
__mlibc_signal_restore:
	mov $25, %rax
    int $0x8F
    ud2

.size __mlibc_signal_restore, . - __mlibc_signal_restore

.section .note.GNU-stack,"",%progbits

