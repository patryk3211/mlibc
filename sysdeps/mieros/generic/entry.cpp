#include <stdlib.h>
#include <mlibc/elf/startup.h>

void __mlibc_initLocale();

extern char **environ;
static mlibc::exec_stack_data __mlibc_stack_data;

struct LibraryGuard {
    LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
    __mlibc_initLocale();
}

extern "C" void __mlibc_entry(int (*main_fn)(int argc, char *argv[], char *env[])) {
    int result = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ);
    exit(result);
}
