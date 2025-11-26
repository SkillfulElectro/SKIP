# CMake generated Testfile for
# Source directory: /app
# Build directory: /app/build
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
add_test(python_example "/home/jules/.pyenv/shims/python" "/app/python/example.py")
set_tests_properties(python_example PROPERTIES  DEPENDS "tests" _BACKTRACE_TRIPLES "/app/CMakeLists.txt;33;add_test;/app/CMakeLists.txt;0;")
add_test(go_example "/usr/local/go/bin/go" "run" "/app/go/example.go")
set_tests_properties(go_example PROPERTIES  DEPENDS "tests" ENVIRONMENT "LD_LIBRARY_PATH=/app/build" WORKING_DIRECTORY "/app/go" _BACKTRACE_TRIPLES "/app/CMakeLists.txt;38;add_test;/app/CMakeLists.txt;0;")
add_test(java_example "/usr/share/maven/bin/mvn" "exec:java")
set_tests_properties(java_example PROPERTIES  DEPENDS "java_build;tests" ENVIRONMENT "LD_LIBRARY_PATH=/app/build" WORKING_DIRECTORY "/app/java" _BACKTRACE_TRIPLES "/app/CMakeLists.txt;54;add_test;/app/CMakeLists.txt;0;")
add_test(rust_example "/home/jules/.cargo/bin/cargo" "run" "--manifest-path" "/app/rust/Cargo.toml" "--example" "simple")
set_tests_properties(rust_example PROPERTIES  DEPENDS "tests" ENVIRONMENT "LD_LIBRARY_PATH=/app/build" WORKING_DIRECTORY "/app/build" _BACKTRACE_TRIPLES "/app/CMakeLists.txt;65;add_test;/app/CMakeLists.txt;0;")
subdirs("_deps/json-build")
subdirs("_deps/pugixml-build")
