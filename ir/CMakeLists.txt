cmake_minimum_required(VERSION 3.10)
project(IR)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(${PROJECT_NAME} main.cpp)

# {
#     "demangle", "support", "tablegen", "core", "fuzzmutate", 
#     "filecheck", "interfacestub", "irreader", "codegen", 
#     "selectiondag", "asmprinter", "mirparser", "globalisel", 
#     "binaryformat", "bitreader", "bitwriter", "bitstreamreader", 
#     "dwarflinker", "extensions", "frontendopenmp", "transformutils", 
#     "instrumentation", "aggressiveinstcombine", "instcombine", 
#     "scalaropts", "ipo", "vectorize", "objcarcopts", "coroutines", 
#     "cfguard", "linker", "analysis", "lto", "mc", "mcparser", 
#     "mcdisassembler", "mca", "object", "objectyaml", "option", 
#     "remarks", "debuginfodwarf", "debuginfogsym", "debuginfomsf", 
#     "debuginfocodeview", "debuginfopdb", "symbolize", "executionengine", 
#     "interpreter", "jitlink", "mcjit", "orcjit", "orcshared", 
#     "orctargetprocess", "runtimedyld", "target", "asmparser", 
#     "lineeditor", "profiledata", "coverage", "passes", "textapi", 
#     "dlltooldriver", "libdriver", "xray", "windowsmanifest"
# }
llvm_map_components_to_libnames(llvm_libs support core irreader executionengine)
target_link_libraries(${PROJECT_NAME} ${llvm_libs})