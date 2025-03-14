project('iw1x-server', 'cpp',
  version : '1.0',
  default_options : ['cpp_std=c++14']
)

# Retrieve custom options (see meson_options.txt)
debug_build   = get_option('debug_build')
enable_unsafe = get_option('enable_unsafe')
compile_ssl   = get_option('compile_ssl')
compile_curl  = get_option('compile_curl')
compile_sqlite= get_option('compile_sqlite')

# Log the chosen options
message('---------------------')
message('Debug: ' + (debug_build ? 'true' : 'false'))
message('Risky features: ' + (enable_unsafe ? 'true' : 'false'))
message('SSL: ' + (compile_ssl ? 'true' : 'false'))
message('cURL: ' + (compile_curl ? 'true' : 'false'))
message('SQLite: ' + (compile_sqlite ? 'true' : 'false'))
message('---------------------')

# Add common compiler and linker flags
add_project_arguments('-m32', '-fpic', '-Wall', '-fvisibility=hidden', language : 'cpp')
add_project_link_arguments('-m32', '-L/lib32', '-L/usr/lib32', '-L/usr/lib/i386-linux-gnu', language : 'cpp')

if debug_build
  add_project_arguments('-g', '-ggdb', language : 'cpp')
endif

# Set preprocessor definitions based on options
unsafe_def = enable_unsafe ? ['-DENABLE_UNSAFE=1'] : ['-DENABLE_UNSAFE=0']
sqlite_def = compile_sqlite  ? ['-DCOMPILE_SQLITE=1']  : ['-DCOMPILE_SQLITE=0']
curl_def   = compile_curl    ? ['-DCOMPILE_CURL=1']    : ['-DCOMPILE_CURL=0']
ssl_def    = compile_ssl     ? ['-DCOMPILE_SSL=1']     : ['-DCOMPILE_SSL=0']

# Gather dependencies
deps = []
threads_dep = dependency('threads')
deps += threads_dep

if compile_sqlite
  sqlite_dep = dependency('sqlite3', required : true)
  deps += sqlite_dep
endif

if compile_curl
  curl_dep = dependency('libcurl', required : true)
  deps += curl_dep
endif

if compile_ssl
  openssl_dep = dependency('openssl', required : true)
  deps += openssl_dep
endif

# Source files list
sources = [
  'src/hook.cpp',
  'src/iw1x.cpp',
  'src/gsc.cpp',
  'src/gsc_entity.cpp',
  'src/gsc_player.cpp',
  'src/gsc_bots.cpp',
  'src/gsc_weapons.cpp',
  'src/gsc_exec.cpp',
  'src/gsc_utils.cpp',
  'src/jump.cpp',
]

if compile_sqlite
  sources += 'src/gsc_sqlite.cpp'
endif

if compile_curl
  sources += 'src/gsc_curl.cpp'
endif

# Build the shared library with a precompiled header.
# Note: The 'pch' argument (available in newer Meson versions) instructs Meson to precompile the given header.
lib = shared_library(
  'iw1x',
  sources,
  c_pch : 'src/pch.h',
  cpp_pch : 'src/pch.h',
  cpp_args : unsafe_def + sqlite_def + curl_def + ssl_def,
  dependencies : deps,
  link_args : ['-ldl'],
  name_prefix : '',  # Remove the default "lib" prefix.
  install_dir : join_paths(meson.current_source_dir(), 'bin'),
  install : true   # Change to true if you want to install.
)

