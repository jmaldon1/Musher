from conans import ConanFile, CMake

class MusherConan(ConanFile):
    name = "musher"
    settings = "os", "compiler", "build_type", "arch"

    generators = "cmake"

    _cmake = None

    def requirements(self):
        self.requires("gtest/[>=1.10.0]")
        # self.requires("functionalplus/v0.2.10-p0@dobiasd/stable")

    def set_version(self):
        self.version = "0.0.1"

    def _configure_cmake(self):
        if self._cmake:
            return self._cmake

        cmake = CMake(self)
        cmake.definitions["CMAKE_BUILD_TYPE"] = self.settings.build_type
        cmake.definitions["ENABLE_CONAN_INSTALL"] = False
        cmake.definitions["ENABLE_PACKAGE_BUILD"] = True
        cmake.configure()
        self._cmake = cmake
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = []

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
