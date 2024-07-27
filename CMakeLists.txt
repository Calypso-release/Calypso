cmake_minimum_required(VERSION 3.9)
project(bench)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-march=native -O2)

add_executable(CardinalityMeasurement include/common/MurmurHash3.cpp include/common/utils.cpp include/baseline/Couper/Couper.cpp include/baseline/RerSkt/rerskt.cpp include/baseline/VHS/VHS.cpp experiments/CardinalityMeasurement.cpp)
add_executable(SuperSpreaderDetection include/common/MurmurHash3.cpp include/common/utils.cpp include/baseline/Couper/Couper.cpp include/baseline/RerSkt/rerskt.cpp include/baseline/VHS/VHS.cpp experiments/SuperSpreaderDetection.cpp)
