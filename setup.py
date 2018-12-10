# http://www.benjack.io/2017/06/12/python-cpp-tests.html
import os
import re
import sys
import sysconfig
import platform
import subprocess

from distutils.version import LooseVersion
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)',
                                         out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        print(self.get_ext_fullpath(ext.name))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DBUILD_CLI=OFF',
                      '-DBUILD_TESTS=OFF',
                      '-DBUILD_PYGENEX=ON', '-DBoost_NO_BOOST_CMAKE=TRUE', '-DBoost_NO_SYSTEM_PATHS=TRUE', '-DBoost_NO_SYSTEM_PATHS=TRUE', '-DCMAKE_LIBRARY_PATH=/work/msslocal/lib', '-DBoost_INCLUDE_DIRS=/work/msslocal/include', '-DBoost_LIBRARY_DIRS=/work/msslocal/lib', '-DBOOST_ROOT=/work/msslocal/Downloads/boost_1_66_0']
        cfg = 'Debug' if self.debug else 'Release'
        build_args = []

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
                cfg.upper(),
                extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)


setup(
    name='pygenex',
    version='0.2',
    author='Cuong Nguyen',
    author_email='ctnguyendinh@wpi.edu',
    description='',
    long_description='',
    # add extension module
    ext_modules=[CMakeExtension('pygenex')],
    # add custom build_ext command
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
