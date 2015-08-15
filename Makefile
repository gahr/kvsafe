BUILD_DIR?=		build_dir
CMAKE_BUILD_TYPE?=	Release

CMAKE_ARGS=		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

all: build

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

configure: ${BUILD_DIR} CMakeLists.txt
	cd ${BUILD_DIR} && cmake ${CMAKE_ARGS} ..

build: configure
	cd ${BUILD_DIR} && make

clean:
	rm -rf ${BUILD_DIR}
