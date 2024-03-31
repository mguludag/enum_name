find_package(Doxygen REQUIRED)

configure_file("main.md.in" "main.md" @ONLY)

set(DOXYGEN_HTML_OUTPUT "${PROJECT_VERSION}")
set(DOXYGEN_OUTPUT_DIRECTORY "docs")
set(DOXYGEN_HTML_HEADER "header.html")
set(DOXYGEN_HTML_EXTRA_FILES "version_selector_handler.js")
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_BINARY_DIR}/main.md")

doxygen_add_docs(doc "${PROJECT_SOURCE_DIR}")
