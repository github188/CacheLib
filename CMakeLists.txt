project(CacheLib)
cmake_minimum_required(VERSION 2.8)
#set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_COMPILER "g++") 
#set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_COMPILER "/usr/bin/cc")
set(CMAKE_CXX_COMPILER "/usr/bin/c++")
set(CMAKE_C_FLAGS "-fPIC")

#SET(CMAKE_BUILD_TYPE "Release")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -pg -g -ggdb")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
#set(CMAKE_LD_FLAGS "-pg")

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_definitions(-D_DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

execute_process( COMMAND  pwd
OUTPUT_VARIABLE  CHKPATH
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..)
string(STRIP ${CHKPATH} CHKPATH)
SET(CMAKE_INSTALL_PREFIX  ${CHKPATH})

add_definitions(-D_LL="${CMAKE_INSTALL_PREFIX}")
INCLUDE_DIRECTORIES(./redis  ./model ./serial ./rapidjson ./)

LINK_DIRECTORIES(/usr/local/lib)

SET(DATAACCESS_SRC  redis/sds.c
                                    redis/redis.c
                                    redis/net.c
                                    redis/dict.c
                                    redis/async.c
                                    Json/json_writer.cpp
                                    Json/json_value.cpp
                                    Json/json_reader.cpp
                                    DataAccess/jsonstandardcache.cpp
                                    DataAccess/cachebase.cpp
                                    DataAccess/stringbunch.cpp
)


SET(BK_SRC backup/cachebackup.cpp
    backup/cachemapnormal.cpp
    backup/cache2mapnormal.cpp
    backup/cache3mapnormal.cpp
)
SET(CIM_SRC  cim/cimmanager.cpp
   cim/cimunit.cpp
   cim/cimunitlist.cpp
   cim/normalcimunit.cpp
   cim/onlyonecimunit.cpp
)


SET(CACHE_SRC
     cache/cacheserver.cpp
     cache/cachemanager.cpp
     cache/cachemanagerobj.cpp
     cache/cachemanager3map.cpp
     cache/cachemanager2map.cpp
     cache/autoconncache.cpp
     cache/cachemanagervector2d.cpp
     cache/maptree.cpp
     cache/cachepulish.cpp
)

SET(COMM_SRC comm/norexception.cpp
                         comm/cachequeue.cpp
                         comm/define.cpp
                         comm/paratwo.cpp
                         comm/datetime.cpp
                         comm/vector2d.h
                         comm/reflexclass.h
                         comm/zini.cpp
)

SET(SERIAL_SRC serial/jsoniarchive.cpp
                          serial/jsonoarchive.cpp
                          serial/jsonserial.cpp
                          serial/jsoncontext.cpp
                          serial/jsonobj.cpp
                          serial/typeforjsonconvertor.cpp
)

SET(TESTCACHE_SRC cachemain.cpp
)
SET(TESTCIM_SRC cimmain.cpp
)

SET(TESTJSON_SRC jsonmain.cpp
)

FIND_PATH(ODBC_INCLUDE_DIR NAMES sql.h PATHS
  /usr/include
  /usr/local/include
  /usr/local/unixODBC/include
  )

FIND_LIBRARY(ODBC_LIBRARY  NAMES odbc PATHS
  /usr/lib
  /usr/local/lib
  /usr/local/unixODBC/lib
  )

if(ODBC_INCLUDE_DIR AND ODBC_LIBRARY)
  SET(ODBC_FOUND TRUE)
endif(ODBC_INCLUDE_DIR AND ODBC_LIBRARY)

SET(DATAACCESS_SRC ${DATAACCESS_SRC} ${COMM_SRC} ${SERIAL_SRC} ${CIM_SRC} ${BK_SRC} ${CACHE_SRC})
SET(DATAACCESS_LIB boost_regex)


if(ODBC_FOUND)
    INCLUDE_DIRECTORIES(${ODBC_INCLUDE_DIR})
    #MESSAGE("db include:${ODBC_INCLUDE_DIR},db lib:${ODBC_LIBRARY_DIR}")
    SET(DATAACCESS_SRC ${DATAACCESS_SRC} DB/odbcdb.cpp
                        DB/odbcpool.cpp
                        DB/autoconndb.cpp
                        DB/dbentity.cpp
                        DB/batentity.cpp
                        DB/autostmt.cpp
                        DB/sqlparameter.cpp
                        DB/odsingleentity.cpp
    )
   SET(DATAACCESS_LIB ${DATAACCESS_LIB} ${ODBC_LIBRARY})
endif(ODBC_FOUND)

ADD_LIBRARY(DatAccess SHARED ${DATAACCESS_SRC})
TARGET_LINK_LIBRARIES(DatAccess ${DATAACCESS_LIB})

SET_TARGET_PROPERTIES(DatAccess PROPERTIES VERSION 2.0 SOVERSION 2)
SET_TARGET_PROPERTIES(DatAccess PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../../lib)


ADD_EXECUTABLE(CimTest ${TESTCIM_SRC})
TARGET_LINK_LIBRARIES(CimTest DatAccess  boost_thread boost_system log4cpp)
SET_TARGET_PROPERTIES(CimTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../bin)

ADD_EXECUTABLE(CacheTest ${TESTCACHE_SRC})
TARGET_LINK_LIBRARIES(CacheTest DatAccess boost_thread boost_system log4cpp)
SET_TARGET_PROPERTIES(CacheTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../bin)


ADD_EXECUTABLE(JsonTest ${TESTJSON_SRC})
TARGET_LINK_LIBRARIES(JsonTest DatAccess boost_thread boost_system log4cpp)
SET_TARGET_PROPERTIES(JsonTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../bin)

INSTALL(TARGETS DatAccess
        LIBRARY DESTINATION lib)



