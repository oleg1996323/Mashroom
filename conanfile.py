from conan import ConanFile
from conan.tools.cmake import cmake_layout


    
class MashroomRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    author="Oster"
    languages="C++","C"

    def requirements(self):
        self.requires("zlib/1.3.2")
        self.requires("boost/1.91.0")
        self.requires("geos/3.14.1")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.9")

    def options_build(self):
        self.options="zlib/*:shared=False"
        self.options="boost/*:shared=False"
        self.options="geos/*:shared=False"
    def layout(self):
        cmake_layout(self)