project('exam-clock', 'cpp', default_options: ['cpp_std=c++20'])

cpp = meson.get_compiler('cpp')

rapidcheck_dep = dependency('rapidcheck', required: false)

if not rapidcheck_dep.found()
    cmake = import('cmake')
    rapidcheck_proj = cmake.subproject('rapidcheck')
    rapidcheck_dep = rapidcheck_proj.dependency('rapidcheck')
endif

inc = include_directories('include')

clock_lib = static_library('clock', 'src/Clock.cpp', include_directories: inc)

all_tests = executable(
    'all_tests',
    'test/all_tests.cpp',
    include_directories: inc,
    link_with: clock_lib,
    dependencies: rapidcheck_dep,
)

test('all_tests', all_tests)
