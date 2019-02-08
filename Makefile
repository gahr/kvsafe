BUILD_DIR?=		build_dir
CMAKE_BUILD_TYPE?=	Release
CMAKE_GENERATOR?=	Ninja
CLANG_FORMAT?=		/usr/local/bin/clang-format60
TCLSH?=			/usr/local/bin/tclsh8.7

CMAKE_ARGS=		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

.if ${CMAKE_GENERATOR} == "Ninja"
MAKE=	ninja
.else
MAKE=	make
.endif

build: ${BUILD_DIR}/kvsafe

${BUILD_DIR}/kvsafe: configure
	${MAKE} -C ${BUILD_DIR}

configure: ${BUILD_DIR} ${BUILD_DIR}/CMakeCache.txt

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

${BUILD_DIR}/CMakeCache.txt: CMakeLists.txt
	cd ${BUILD_DIR} && cmake ${CMAKE_ARGS} -G"${CMAKE_GENERATOR}" ..

clean:
	rm -rf ${BUILD_DIR}

test: ${BUILD_DIR}/kvsafe
	${TCLSH} tests/all.tcl -verbose t

indent:
	find src -type f | xargs ${CLANG_FORMAT} -i --style=file

full: clean indent build test
