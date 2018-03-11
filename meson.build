project('cetrinet', 'cpp',
  default_options: [
    'cpp_std=c++17',
  ],
)

add_project_arguments(
#  '-Os',
#  '-ffast-math',
#  '-fno-math-errno',
#  '-fno-ident',
#  '-ffunction-sections',
#  '-fdata-sections',
#  '-fmerge-all-constants',
  language: 'cpp',
)

add_project_link_arguments(
#  '-static',
#  '-Wl,-Bstatic', #TODO: proper static build
#  '-s',
#  '-Wl,--gc-sections',
  language: 'cpp',
)

glob = run_command('meson/wildcard', 'src/**/*.cpp')
sources = glob.stdout().strip().split('\n')

dependencies = [
  dependency('threads'),
  dependency('openssl'),
  dependency('boost', modules: ['system']),
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
