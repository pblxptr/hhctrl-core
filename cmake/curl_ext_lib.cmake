### PATHS ###
set(CURL_DIR ${CMAKE_SOURCE_DIR}/lib/curl)
set(CURL_INCLUDE_DIR ${CURL_DIR}/include)
set(CURL_LIBRARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/curl/lib/.libs)

### LIB NAMES ###
set(CURL_LIBRARY "-lcurl")

ExternalProject_Add(curl
  GIT_REPOSITORY    "https://github.com/curl/curl.git"
  GIT_TAG           master
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/curl
  BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/curl
  UPDATE_COMMAND    ""
  CONFIGURE_COMMAND autoreconf -fi ${CURL_DIR}/configure.ac && ${CURL_DIR}/configure --disable-shared --enable-static --without-ssl --without-brotli --without-zlib
  BUILD_COMMAND     make # change to: make
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
include_directories(${CURL_INCLUDE_DIR})
link_directories(${CURL_LIBRARY_DIR})

