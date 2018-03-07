project('cetrinet', 'cpp',
  default_options: [
    'cpp_std=c++17',
  ],
)

add_project_arguments(
  '-Os',
  language: 'cpp',
)

add_project_link_arguments(
#  '-static',
#  '-Wl,-Bstatic', #TODO: proper static build
  language: 'cpp',
)

glob = run_command('meson/wildcard', 'src/**/*.cpp')
sources = glob.stdout().strip().split('\n')

dependencies = [
  dependency('threads'),
  dependency('openssl'),
  dependency('boost', modules: ['system']),
  dependency('lcui'),
  dependency('lcuiex'),
]

# Generate assets
assets = run_command('meson/assets', 'assets')

exe = executable(
  'cetrinet',
  sources: sources,
  dependencies: dependencies,
  gui_app: true,
)
