# Copyright © 2018, 2019 Iñigo Martínez
# Copyright © 2019 Christian Persch
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library.  If not, see <https://www.gnu.org/licenses/>.

project(
  'vte',
  ['c', 'cpp'],
  version: '0.69.0',
  license: ['LGPL-3.0-or-later', 'GPL-3.0-or-later'],
  default_options: [
    'buildtype=release',
    'warning_level=0',
    'b_ndebug=false',
  ],
  meson_version: '>= 0.50.0',
)

# Compiler requirements

c_req_std                 = 'gnu11'
cxx_req_std               = 'gnu++20'
gxx_req_version           = '10.0'
clangxx_req_version       = '11.0'

# Version requirements

gtk3_req_version          = '3.20.0'
gtk3_min_req_version      = '3.18'
gtk3_max_allowed_version  = '3.20'

gtk4_req_version          = '4.0.1'
gtk4_min_req_version      = '4.0'
gtk4_max_allowed_version  = '4.0'

fribidi_req_version       = '1.0.0'
gio_req_version           = '2.52.0'
glib_req_version          = '2.52.0'
glib_min_req_version      = '2.52'
glib_max_allowed_version  = '2.52'
gnutls_req_version        = '3.2.7'
icu_uc_req_version        = '4.8'
pango_req_version         = '1.22.0'
pcre2_req_version         = '10.21'
systemd_req_version       = '220'

# API

vte_api_major_version = 2
vte_api_minor_version = 91
vte_api_version = '@0@.@1@'.format(vte_api_major_version, vte_api_minor_version)
vte_api_name = 'vte-@0@.@1@'.format(vte_api_major_version, vte_api_minor_version)

vte_gtk3_api_name = 'vte-' + vte_api_version
vte_gtk4_api_name = 'vte-' + vte_api_version + '-gtk4'

vte_gtk3_api_path = vte_gtk3_api_name / 'vte'
vte_gtk4_api_path = vte_gtk4_api_name / 'vte'

# Library versioning

vte_version = meson.project_version()
version_array = vte_version.split('.')
vte_major_version = version_array[0].to_int()
vte_minor_version = version_array[1].to_int()
vte_micro_version = version_array[2].to_int()

libvte_soversion = 0

lt_revision = (vte_minor_version.is_odd() ? 0 : vte_micro_version)
lt_age = vte_minor_version * 100 + vte_micro_version - lt_revision
lt_current = vte_major_version + lt_age

libvte_gtk3_soversion = '@0@.@1@.@2@'.format(libvte_soversion, lt_current, lt_revision)
libvte_gtk4_soversion = libvte_soversion.to_string()

# i18n

vte_gettext_domain = vte_api_name

# Directories

vte_datadir = get_option('datadir')
vte_includedir = get_option('includedir')
vte_libexecdir = get_option('libexecdir')
vte_localedir = get_option('localedir')
vte_prefix = get_option('prefix')
vte_sysconfdir = get_option('sysconfdir')

# It is correct for this to be in ${prefix}/lib, even on systems where that
# does not match ${libdir}. This is what systemd uses on such platforms.
vte_systemduserunitdir = vte_prefix / 'lib' / 'systemd' / 'user'

# Debug

enable_debug = get_option('debugg') or get_option('debug') or get_option('buildtype').contains('debug')

# Meson modules

gnome = import('gnome')
pkg = import('pkgconfig')

# Compilers

# When using 'c_std' / 'cpp_std' in the default_options of project(),
# then later changing that requirement there, meson will *not* update
# the actual std used. E.g.
# '''
# project(..., default_options: ['cpp_std=gnu++17'])
# assert(get_option('cpp_std') == 'gnu++17')
# '''
# Now change both of the '17' to '20' and build; meson will update the
# build files and trip on the assert since it still uses cpp_std=gnu++17.
# And it allows the user to override the c/cpp_std option from the command
# line.
#
# Therefore, instead of using default_options, add the required -std=...
# options to the compiler options directly.

# C compiler

cc = meson.get_compiler('c')

c_std_opt = '-std=' + c_req_std
assert(cc.has_argument(c_std_opt), 'option ' + c_std_opt + ' not supported by ' + cc.get_id())
add_project_arguments(c_std_opt, language: 'c')

# C++ compiler

cxx = meson.get_compiler('cpp')

cxx_std_opt = '-std=' + cxx_req_std
assert(cxx.has_argument(cxx_std_opt), 'option ' + cxx_std_opt + ' not supported by ' + cxx.get_id())
add_project_arguments(cxx_std_opt, language: 'cpp')

# The above only checks that the compiler supports the given -std option,
# but not that the compiler really supports that C++ standard version.
# Do a simple version check based on
# https://gcc.gnu.org/projects/cxx-status.html and
# https://clang.llvm.org/cxx_status.html
# for the C++ features that vte actually uses.

if cxx.get_id() == 'gcc'
  assert(cxx.version().version_compare('>=' + gxx_req_version), 'needs g++ >= ' + gxx_req_version + ' for ' + cxx_req_std + ' support')
endif

if cxx.get_id() == 'clang'
  assert(cxx.version().version_compare('>=' + clangxx_req_version), 'needs clang++ >= ' + clangxx_req_version + ' for ' + cxx_req_std + ' support')
endif

# Include directories

top_inc = include_directories('.')

# Start config.h

config_h = configuration_data()

config_h.set_quoted('GETTEXT_PACKAGE', vte_gettext_domain)
config_h.set_quoted('VERSION', vte_version)
config_h.set('VTE_DEBUG', enable_debug)
config_h.set('WITH_A11Y', get_option('a11y'))
config_h.set('WITH_FRIBIDI', get_option('fribidi'))
config_h.set('WITH_GNUTLS', get_option('gnutls'))
config_h.set('WITH_ICU', get_option('icu'))
config_h.set('WITH_SIXEL', get_option('sixel'))

ver = glib_min_req_version.split('.')
config_h.set('GLIB_VERSION_MIN_REQUIRED', '(G_ENCODE_VERSION(' + ver[0] + ',' + ver[1] + '))')
ver = glib_max_allowed_version.split('.')
config_h.set('GLIB_VERSION_MAX_ALLOWED', '(G_ENCODE_VERSION(' + ver[0] + ',' + ver[1] + '))')

if get_option('gtk3')
  gtk3_version_cppflags = []

  ver = gtk3_min_req_version.split('.')
  gtk3_version_cppflags += '-DGDK_VERSION_MIN_REQUIRED=(G_ENCODE_VERSION(' + ver[0] + ',' + ver[1] + '))'

  ver = gtk3_max_allowed_version.split('.')
  gtk3_version_cppflags += '-DGDK_VERSION_MAX_ALLOWED=(G_ENCODE_VERSION(' + ver[0] + ',' + ver[1] + '))'
endif

if get_option('gtk4')
  gtk4_version_cppflags = []

  ver = gtk4_min_req_version.split('.')
  gtk4_version_cppflags += '-DGDK_VERSION_MIN_REQUIRED=(G_ENCODE_VERSION(' + ver[0] + ',' + ver[1] + '))'

  ver = gtk4_max_allowed_version.split('.')
  gtk4_version_cppflags += '-DGDK_VERSION_MAX_ALLOWED=(G_ENCODE_VERSION(' + ver[0] + ',' + ver[1] + '))'
endif


# FIXME AC_USE_SYSTEM_EXTENSIONS also supported non-gnu systems
config_h.set10('_GNU_SOURCE', true)
config_h.set('_POSIX_C_SOURCE', '200809L')

# Check headers

check_headers = [
  'locale.h',
  'pty.h',
  'stropts.h',
  'sys/resource.h',
  'sys/select.h',
  'sys/stream.h',
  'sys/syslimits.h',
  'sys/termios.h',
  'sys/types.h',
  'sys/wait.h',
  'termios.h',
  'util.h',
  'wchar.h',
]

foreach header: check_headers
  config_h.set('HAVE_' + header.underscorify().to_upper(), cxx.has_header(header))
endforeach

# Check for symbols

check_symbols_required = [
  ['TIOCGWINSZ', 'sys/ioctl.h'],
]

foreach symbol: check_symbols_required
  assert(cxx.has_header_symbol(symbol[1], symbol[0]), symbol[0] + ' not found')
endforeach

# Check for functions

libm_dep = cxx.find_library('m')

# Note that we cannot use compiler.has_function() due to
# https://github.com/mesonbuild/meson/issues/8075 . So
# Hand-roll our own check functions, also checking for
# the correct function prototype.

# [func name, func type, includes, deps, required]
check_functions = [
  [
    'fork',
    'pid_t (*func)(void)',
    ['sys/types.h', 'unistd.h'],
    [],
    true,
  ],
  [
    'grantpt',
    'int (*func)(int)',
    ['stdlib.h'],
    [],
    true,
  ],
  [
    'posix_openpt',
    'int (*func)(int)',
    ['stdlib.h', 'fcntl.h'],
    [],
    true,
  ],
  [
    'ptsname',
    'char* (*func)(int)',
    ['stdlib.h'],
    [],
    true,
  ],
  [
    'tcgetattr',
    'int (*func)(int, struct termios*)',
    ['termios.h', 'unistd.h'],
    [],
    true,
  ],
  [
    'unlockpt',
    'int (*func)(int)',
    ['stdlib.h'],
    [],
    true,
  ],
  [
    'explicit_bzero',
    'void (*func)(void*, size_t)',
    ['string.h'],
    [],
    false,
  ],
  [
    'fdwalk',
    'int (*func)(int (*)(void*, int), void*)',
    ['stdlib.h'],
    [],
    false,
  ],
  [
    'pread',
    'ssize_t (*func)(int, void*, size_t off_t)',
     ['unistd.h'],
     [],
     false,
  ],
  [
    'pwrite',
    'ssize_t (*func)(int, const void*, size_t, off_t)',
     ['unistd.h'],
     [],
     false,
  ],
  [
    'strchrnul',
    'const char* (*func)(const char*, int)',
     ['string.h'],
     [],
     false,
  ],
  [
    'close_range',
    'int (*func)(unsigned int, unsigned int, int)',
     ['unistd.h'],
     [],
     false,
   ],

  # Math functions

  [
    'ceil',
    'double (*func)(double)',
    ['math.h'],
    [libm_dep],
    true,
  ],
  [
    'floor',
    'double (*func)(double)',
    ['math.h'],
    [libm_dep],
    true,
  ],
  [
    'round',
    'double (*func)(double)',
    ['math.h'],
    [libm_dep],
    false,
  ],
]

foreach func: check_functions
  __name=func[0]
  __prot=func[1]
  __incs=func[2]
  __deps=func[3]
  __reqd=func[4]

  __code = ''
  foreach __inc: __incs
    __code += '''
    #if __has_include(<@0@>)
    #include <@0@>
    #endif
    '''.format(__inc)
  endforeach

  __code += '''
    int main(void) {
      @0@ = &@1@;
      long long b = (long long)func;
      return (int)b;
    }
  '''.format(__prot, __name)

  __have = cxx.compiles(
    __code,
    args: [
      '-D_GNU_SOURCE',
      '-D_POSIX_C_SOURCE=200809L',
    ],
    dependencies: __deps,
    name: __name,
  )

  if __reqd
    assert(__have, __name + ' not found')
  else
    config_h.set('HAVE_' + __name.underscorify().to_upper(), __have)
  endif
endforeach

# Compiler

# Asserts must not be disabled

assert(get_option('b_ndebug') == 'false', 'assertions may not be disabled')

# LTO very much NOT supported

assert(get_option('b_lto') == false, 'LTO not supported')

# Compiler flags

compiler_flags_common = [
  '-Wall',
  '-Wextra',
  '-Wcast-align',
  '-Wcast-function-type',
  '-Wclobbered',
  '-Wempty-body',
  '-Wendif-labels',
  '-Werror=init-self',
  '-Werror=missing-include-dirs',
  '-Werror=pointer-arith',
  '-Wfloat-equal',
  '-Wignored-qualifiers',
  '-Winvalid-pch',
  '-Wlogical-op',
  '-Wmisleading-indentation',
  '-Wmissing-declarations',
  '-Wmissing-field-initializers',
  '-Wmissing-format-attribute',
  '-Wmissing-include-dirs',
  '-Wmissing-noreturn',
  '-Wno-address-of-packed-member',
  '-Wno-c99-designator',
  '-Wno-missing-field-initializers',
  '-Wno-packed',
  '-Wno-switch-enum',
  '-Wno-unused-parameter',
  '-Wno-volatile',
  '-Wshadow',
  '-Wshift-negative-value',
  '-Wsign-compare',
  '-Wstrict-aliasing=2',
  '-Wtype-limits',
  '-Wundef',
  '-Wuninitialized',
  '-Wuninitialized',
  '-Wunsafe-loop-optimizations',
  '-Wunused',
  '-Wunused-but-set-parameter',
  '-Wunused-but-set-variable',
  '-Wunused-function',
  '-Wunused-label',
  '-Wunused-local-typedefs',
  '-Wunused-value',
  '-Wunused-variable',
  '-Wvla',
  '-Wwrite-strings',
  '-fdiagnostics-show-option',
  '-fno-common',
  '-fno-semantic-interposition',
]

if enable_debug
  compiler_flags_common += [
    '-ggdb3',
  ]
endif

# These are currently needed but the code should be fixed instead
compiler_flags_common_undesirable = [
  '-fno-strict-aliasing'
]

compiler_flags_c_only = [
  '-Waggregate-return',
  '-Werror=implicit-function-declaration',
  '-Werror=missing-prototypes',
  '-Wimplicit',
  '-Wimplicit-fallthrough=3',
  '-Wmissing-parameter-type',
  '-Wnested-externs',
  '-Wold-style-declaration',
  '-Wold-style-definition',
  '-Woverride-init',
  '-Wsign-compare',
  '-Wstrict-prototypes',
]

compiler_flags_cxx_only = [
  '-Wimplicit-fallthrough=5',
  '-Wnon-virtual-dtor',
  '-Wstrict-null-sentinel',
]

compiler_flags_cxx_required = [
  '-fvisibility-inlines-hidden',
  '-fvisibility=hidden',
]

if cxx.get_id() == 'clang'
  compiler_flags_cxx_only += [
    '-Wno-unknown-attributes', # get rid of warnings about unimplemented [[(un)likely]]
  ]
endif

global_cflags = cc.get_supported_arguments(compiler_flags_common +
                                           compiler_flags_common_undesirable +
                                           compiler_flags_c_only)
global_cxxflags = cxx.get_supported_arguments(compiler_flags_common +
                                              compiler_flags_common_undesirable +
                                              compiler_flags_cxx_only +
                                              compiler_flags_cxx_required)

foreach flag: compiler_flags_cxx_required
  assert(cxx.has_argument(flag), flag + ' is required but not supported')
endforeach

# These flags have to be tested together

compiler_flags_common_multi = [
  # These only work together with -Wformat
  [
    '-Werror=format=2',
    '-Werror=format-nonliteral',
    '-Werror=format-security',
  ],
]

foreach flags : compiler_flags_common_multi
  if cc.has_multi_arguments(flags)
    global_cflags += flags
  endif
  if cxx.has_multi_arguments(flags)
    global_cxxflags += flags
  endif
endforeach

# ... and now make these flags the default

add_project_arguments(global_cflags, language: 'c')
add_project_arguments(global_cxxflags, language: 'cpp')

# Linker flags

linker_flags = [
  ['-Wl,-Bsymbolic', false,],
  ['-Wl,-Bsymbolic-functions', get_option('_b_symbolic_functions'),],

  # See issue vte#2535.
  ['-fstack-protector', false],
  ['-fstack-protector-strong', false],
]

foreach flag: linker_flags
  if cc.has_link_argument(flag[0])
    add_project_link_arguments(flag[0], language: 'c')
  elif flag[1]
    assert(false, flag[0] + ' is required but not supported')
  endif

  if cxx.has_link_argument(flag[0])
    add_project_link_arguments(flag[0], language: 'cpp')
  elif flag[1]
    assert(false, flag[0] + ' is required but not supported')
  endif
endforeach

# Dependencies

gio_dep      = dependency('gio-2.0',    version: '>=' + gio_req_version)
glib_dep     = dependency('glib-2.0',   version: '>=' + glib_req_version)
gobject_dep  = dependency('gobject-2.0')
pango_dep    = dependency('pango',      version: '>=' + pango_req_version)
pcre2_dep    = dependency('libpcre2-8', version: '>=' + pcre2_req_version)
pthreads_dep = dependency('threads')
zlib_dep     = dependency('zlib')

if get_option('fribidi')
  fribidi_dep = dependency('fribidi', version: '>=' + fribidi_req_version)
else
  fribidi_dep = dependency('', required: false)
endif

if get_option('gnutls')
  gnutls_dep = dependency('gnutls', version: '>=' + gnutls_req_version)
else
  gnutls_dep = dependency('', required: false)
endif

if get_option('gtk3')
  gtk3_dep = dependency('gtk+-3.0', version: '>=' + gtk3_req_version)
else
  gtk3_dep = dependency('', required: false)
endif

if get_option('gtk4')
  gtk4_dep = dependency('gtk4', version: '>=' + gtk4_req_version)
else
  gtk4_dep = dependency('', required: false)
endif

if get_option('icu')
  icu_dep = dependency('icu-uc', version: '>=' + icu_uc_req_version)
else
  icu_dep = dependency('', required: false)
endif

if host_machine.system() == 'linux' and get_option('_systemd')
  systemd_dep = dependency('libsystemd', version: '>=' + systemd_req_version)
else
  systemd_dep = dependency('', required: false)
endif

config_h.set('WITH_SYSTEMD', systemd_dep.found())

# Write config.h

config_sources = [
  configure_file(
    output: 'config.h',
    configuration: config_h,
  ),
]

# Subdirs

subdir('src')
subdir('bindings')
subdir('po')

if get_option('docs')
  assert(meson.version().version_compare('>= 0.55.0'),
    'meson >= 0.55 is required to build docs'
  )

  subdir('doc/reference')
endif

# Dist

# Meson bug https://github.com/mesonbuild/meson/issues/8440 breaks using vte
# as subproject; see vte#345.
if meson.version().version_compare('>= 0.58.0') or not meson.is_subproject()
  meson.add_dist_script(
    find_program('meson_changelog.sh'),
  )
endif

# Simple compat Makefile

makefile_conf = configuration_data()
makefile_conf.set('srcdir', meson.current_source_dir())
makefile_conf.set('builddir', meson.current_build_dir())
makefile_conf.set('vte_api_version', vte_api_version)

configure_file(
  input: 'Makefile.meson',
  output: '@BASENAME@',
  configuration: makefile_conf,
)

# .gitignore everything in the build directory

configure_file(
  output: '.gitignore',
  command: ['echo', '**/**',],
  capture: true,
  install: false,
)

# Summary

output = '\n'
output += 'Configuration for VTE:\n\n'
output += '  Version:      ' + vte_version + '\n'
output += '\n'
output += '  C compiler:   ' + cc.get_id() + ' (version ' + cc.version() + ')\n'
output += '  C++ compiler: ' + cxx.get_id() + ' (version ' + cxx.version() + ')\n'
output += '\n'
output += '  Coverage:     ' + get_option('b_coverage').to_string() + '\n'
output += '  Debug:        ' + enable_debug.to_string() + '\n'
output += '\n'
output += '  A11y:         ' + get_option('a11y').to_string() + '\n'
output += '  Docs:         ' + get_option('docs').to_string() + '\n'
output += '  FRIBIDI:      ' + get_option('fribidi').to_string() + '\n'
output += '  GNUTLS:       ' + get_option('gnutls').to_string() + '\n'
output += '  GTK+ 3.0:     ' + get_option('gtk3').to_string() + '\n'
output += '  GTK+ 4.0:     ' + get_option('gtk4').to_string() + '\n'
output += '  ICU:          ' + get_option('icu').to_string() + '\n'
output += '  GIR:          ' + get_option('gir').to_string() + '\n'
output += '  systemd:      ' + systemd_dep.found().to_string() + '\n'
output += '  SIXEL:        ' + get_option('sixel').to_string() + '\n'
output += '  Glade:        ' + get_option('glade').to_string() + '\n'
output += '  Vala:         ' + get_option('vapi').to_string() + '\n'
output += '\n'
output += '  Prefix:       ' + get_option('prefix') + '\n'
message(output)

if get_option('gtk4')
  warning('GTK+ 4.0 support is experimental; API and ABI are subject to change without notice\n')
endif

# Done
