project('cetrinet', 'cpp',
  default_options: [
    'cpp_std=c++17',
    'b_lto=true',
    'b_pie=true',
  ],
)

if get_option('buildtype').startswith('debug')
  add_project_arguments(
    '-Og',
    '-g',
    '-fsanitize=address,undefined',
    '-fno-omit-frame-pointer',
    language: 'cpp',
  )
  add_project_link_arguments(
    '-fsanitize=address',
    language: 'cpp',
  )
else
  add_project_arguments(
    '-march=x86-64',
    '-mtune=generic',
    '-pipe',
    '-fno-plt',
    '-Os',
    '-ffast-math',
    '-fno-math-errno',
    '-fno-ident',
    '-ffunction-sections',
    '-fdata-sections',
    '-fmerge-all-constants',
    '-gsplit-dwarf',
    language: 'cpp',
  )
endif

add_project_link_arguments(
#  '-static',
#  '-Wl,-Bstatic', #TODO: proper static build
#  '-Wl,--gc-sections', #incompatible on macOS
  language: 'cpp',
)

glob = run_command('meson/wildcard', 'src/**/*.cpp')
sources = glob.stdout().strip().split('\n')

dependencies = [
  dependency('threads'),
  dependency('openssl'),
  dependency('tgui'),
  dependency('fontconfig'),
]

exe = executable(
  'cetrinet',
  sources: sources,
  dependencies: dependencies,
  include_directories: include_directories('vendor/include'),
  gui_app: true,
)
