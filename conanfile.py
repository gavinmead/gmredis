from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps


class GmredisRecipe(ConanFile):
    name = "gmredis"
    version = "0.1.0"
    package_type = "application"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources
    exports_sources = "CMakeLists.txt", "src/*", "lib/*", "tests/*", "cmake/*"

    def requirements(self):
        self.requires("gtest/1.15.0")
        self.requires("catch2/3.7.1")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
