from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext



setup(
        ext_modules=[
            Extension("pybuf",
                [
                    "pybuf.pyx",
                    "utils.cpp",
                    "ring_buffer.cpp",
                    "mapped.cpp",
                ], language="c++",
                extra_compile_args=['-fPIC', "-std=c++2a"],
            )
        ],
        cmdclass = {'build_ext': build_ext}
)
