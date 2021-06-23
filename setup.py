from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext


DEPS = [
    'mmap_queue/mmap_queue.pyx',
]


setup(ext_modules=[
    Extension("mmap_queue", DEPS,
        language="c++",
        extra_compile_args=["-I./includes/", "-std=c++2a", "-O3"],
        libraries=['ringbuf'])],
    cmdclass = {'build_ext': build_ext}
)
