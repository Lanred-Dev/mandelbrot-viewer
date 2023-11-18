# CMake generated Testfile for 
# Source directory: C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/src/2023.02.06-e9de6e8b02.clean/test
# Build directory: C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/x64-windows-rel/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencl_icd_loader_test "C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/x64-windows-rel/icd_loader_test.exe")
set_tests_properties(opencl_icd_loader_test PROPERTIES  ENVIRONMENT "OCL_ICD_FILENAMES=C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/x64-windows-rel/OpenCLDriverStub.dll" WORKING_DIRECTORY "C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/x64-windows-rel" _BACKTRACE_TRIPLES "C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/src/2023.02.06-e9de6e8b02.clean/test/CMakeLists.txt;17;add_test;C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/src/2023.02.06-e9de6e8b02.clean/test/CMakeLists.txt;0;")
add_test(cllayerinfo_test "C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/x64-windows-rel/cllayerinfo.exe")
set_tests_properties(cllayerinfo_test PROPERTIES  ENVIRONMENT "OPENCL_LAYERS=C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/x64-windows-rel/test/layer/PrintLayer.dll" WORKING_DIRECTORY "C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/x64-windows-rel" _BACKTRACE_TRIPLES "C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/src/2023.02.06-e9de6e8b02.clean/test/CMakeLists.txt;23;add_test;C:/Users/lando/source/repos/mandelbrot/vcpkg_installed/vcpkg/blds/opencl/src/2023.02.06-e9de6e8b02.clean/test/CMakeLists.txt;0;")
subdirs("log")
subdirs("driver_stub")
subdirs("loader_test")
subdirs("layer")
