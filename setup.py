from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext


DEPS = [
    'pyrbuf/pybuf.pyx',
]


setup(ext_modules=[
    Extension("pybuf", DEPS,
        language="c++",
        extra_compile_args=["-I./includes/", "-std=c++2a", "-O3"],
        libraries=['ringbuf'])],
    cmdclass = {'build_ext': build_ext}
)
