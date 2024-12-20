from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

__version__ = "0.0.1"

ext_modules = [
    Pybind11Extension(
        'mcts2048',
        [
            'src/game.cpp',
            'src/pybind.cpp',
        ],
        include_dirs=["include/"],
        language='c++',
        define_macros=[("NDEBUG", None)],  # Disable debugging
        extra_compile_args=[
            '-Wno-sign-compare',
            '-Wno-reorder',
            '-std=c++17',
            '-O3',                # High optimization level
            '-march=native',      # Optimize for the local CPU
            '-funroll-loops',     # Unroll loops for better performance
            '-ffast-math',        # Aggressive math optimizations
            '-flto=auto',         # Enable Link-Time Optimization with auto parallel jobs
            '-fomit-frame-pointer',
        ],
        extra_link_args=[
            '-flto=auto',         # Enable Link-Time Optimization for linking
        ],
    ),
]

setup(
    name="mcts2048",
    version=__version__,
    author="Andreas Faust",
    author_email="andreas.s.faust@gmail.com",
    url="",
    description="mcts2048 is a collection of algorithms for the 2048 game.",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)
