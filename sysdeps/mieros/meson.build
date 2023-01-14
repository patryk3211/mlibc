rtdl_sources += files(
	'generic/io.cpp'
)

libc_sources += files(
	'generic/io.cpp',
	'generic/entry.cpp',
	'generic/mem.cpp',
	'generic/system.cpp',
	'generic/thread.cpp'
)

if not no_headers
	install_headers(
		'include/abi-bits/seek-whence.h',
		subdir: 'abi-bits'
	)
    install_headers(
		'include/mieros/syscall.h',
		subdir: 'mieros'
	)
endif

if not headers_only
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'x86_64/crt0.s',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif
