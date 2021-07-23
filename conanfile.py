from conans import ConanFile, CMake, tools
import shutil

class Matrix2dConan(ConanFile):
    name = "matrix2d"
    version = "0.2.3"
    license = "MIT"
    author = "Justin Riddell - arghnews@hotmail.co.uk"
    url = "https://github.com/Arghnews/matrix2d"
    description = "Simple 2d row major contiguous matrix"
    topics = ("c++", "matrix")
    settings = "os", "compiler", "build_type", "arch"

    exports_sources = ("include/*", "CMakeLists.txt", "tests/*", "cmake/*",
            "main.cpp")
    no_copy_source = False

    generators = "cmake_find_package", "cmake"

    requires = ("fmt/[>=7.0.2]", "range-v3/[>=0.11.0]",
            "span-lite/[>=0.9.2]",
            "boost/[>=1.76.0]",
            )

    # Not sure if this is correct, regarding cmake add_subdirectory tests
    build_requires = "catch2/[>=2.13.6]"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test(output_on_failure=True)

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.info.header_only()
