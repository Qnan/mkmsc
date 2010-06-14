#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/common/base_cpp/os_thread_wrapper.o \
	${OBJECTDIR}/tnirps/tnirps_script.o \
	${OBJECTDIR}/tnirps/tnirps_monomial.o \
	${OBJECTDIR}/common/base_cpp/output.o \
	${OBJECTDIR}/common/base_cpp/profiling.o \
	${OBJECTDIR}/common/base_cpp/os_sync_wrapper.o \
	${OBJECTDIR}/tnirps/tnirps_var_map.o \
	${OBJECTDIR}/tnirps/tnirps_scheme_simple.o \
	${OBJECTDIR}/common/base_cpp/chunk_storage.o \
	${OBJECTDIR}/common/base_cpp/bitinworker.o \
	${OBJECTDIR}/common/base_c/nano_posix.o \
	${OBJECTDIR}/common/base_c/os_thread_posix.o \
	${OBJECTDIR}/tnirps/tnirps_monopool.o \
	${OBJECTDIR}/common/base_cpp/scanner.o \
	${OBJECTDIR}/common/base_cpp/exception.o \
	${OBJECTDIR}/tnirps/tnirps_scheme_gorner.o \
	${OBJECTDIR}/common/base_cpp/bitoutworker.o \
	${OBJECTDIR}/common/base_cpp/gray_codes.o \
	${OBJECTDIR}/common/base_cpp/tlscont.o \
	${OBJECTDIR}/tnirps/tnirps_scheme.o \
	${OBJECTDIR}/tnirps/tnirps_ring.o \
	${OBJECTDIR}/common/base_cpp/crc32.o \
	${OBJECTDIR}/common/base_c/os_sync_posix.o \
	${OBJECTDIR}/tnirps/main.o \
	${OBJECTDIR}/tnirps/tnirps_polynomial.o \
	${OBJECTDIR}/common/base_c/os_dir_posix.o \
	${OBJECTDIR}/tnirps/tnirps_reduction.o \
	${OBJECTDIR}/common/base_c/os_tls_posix.o \
	${OBJECTDIR}/tnirps/tnirps_scheme_tree.o \
	${OBJECTDIR}/tnirps/tnirps_common.o \
	${OBJECTDIR}/common/base_cpp/string_pool.o \
	${OBJECTDIR}/common/base_cpp/smart_output.o \
	${OBJECTDIR}/common/base_cpp/shmem_posix.o \
	${OBJECTDIR}/common/base_c/bitarray.o \
	${OBJECTDIR}/tnirps/tnirps_bigint.o \
	${OBJECTDIR}/common/base_cpp/d_bitset.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib -lpthread -lgmp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/tnirps

dist/Release/GNU-Linux-x86/tnirps: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tnirps ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/common/base_cpp/os_thread_wrapper.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/os_thread_wrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/os_thread_wrapper.o common/base_cpp/os_thread_wrapper.cpp

${OBJECTDIR}/tnirps/tnirps_script.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_script.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_script.o tnirps/tnirps_script.cpp

${OBJECTDIR}/tnirps/tnirps_monomial.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_monomial.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_monomial.o tnirps/tnirps_monomial.cpp

${OBJECTDIR}/common/base_cpp/output.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/output.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/output.o common/base_cpp/output.cpp

${OBJECTDIR}/common/base_cpp/profiling.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/profiling.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/profiling.o common/base_cpp/profiling.cpp

${OBJECTDIR}/common/base_cpp/os_sync_wrapper.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/os_sync_wrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/os_sync_wrapper.o common/base_cpp/os_sync_wrapper.cpp

${OBJECTDIR}/tnirps/tnirps_var_map.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_var_map.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_var_map.o tnirps/tnirps_var_map.cpp

${OBJECTDIR}/tnirps/tnirps_scheme_simple.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_scheme_simple.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_scheme_simple.o tnirps/tnirps_scheme_simple.cpp

${OBJECTDIR}/common/base_cpp/chunk_storage.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/chunk_storage.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/chunk_storage.o common/base_cpp/chunk_storage.cpp

${OBJECTDIR}/common/base_cpp/bitinworker.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/bitinworker.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/bitinworker.o common/base_cpp/bitinworker.cpp

${OBJECTDIR}/common/base_c/nano_posix.o: nbproject/Makefile-${CND_CONF}.mk common/base_c/nano_posix.c 
	${MKDIR} -p ${OBJECTDIR}/common/base_c
	${RM} $@.d
	$(COMPILE.c) -O2 -Icommon -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_c/nano_posix.o common/base_c/nano_posix.c

${OBJECTDIR}/common/base_c/os_thread_posix.o: nbproject/Makefile-${CND_CONF}.mk common/base_c/os_thread_posix.c 
	${MKDIR} -p ${OBJECTDIR}/common/base_c
	${RM} $@.d
	$(COMPILE.c) -O2 -Icommon -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_c/os_thread_posix.o common/base_c/os_thread_posix.c

${OBJECTDIR}/tnirps/tnirps_monopool.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_monopool.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_monopool.o tnirps/tnirps_monopool.cpp

${OBJECTDIR}/common/base_cpp/scanner.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/scanner.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/scanner.o common/base_cpp/scanner.cpp

${OBJECTDIR}/common/base_cpp/exception.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/exception.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/exception.o common/base_cpp/exception.cpp

${OBJECTDIR}/tnirps/tnirps_scheme_gorner.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_scheme_gorner.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_scheme_gorner.o tnirps/tnirps_scheme_gorner.cpp

${OBJECTDIR}/common/base_cpp/bitoutworker.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/bitoutworker.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/bitoutworker.o common/base_cpp/bitoutworker.cpp

${OBJECTDIR}/common/base_cpp/gray_codes.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/gray_codes.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/gray_codes.o common/base_cpp/gray_codes.cpp

${OBJECTDIR}/common/base_cpp/tlscont.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/tlscont.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/tlscont.o common/base_cpp/tlscont.cpp

${OBJECTDIR}/tnirps/tnirps_scheme.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_scheme.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_scheme.o tnirps/tnirps_scheme.cpp

${OBJECTDIR}/tnirps/tnirps_ring.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_ring.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_ring.o tnirps/tnirps_ring.cpp

${OBJECTDIR}/common/base_cpp/crc32.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/crc32.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/crc32.o common/base_cpp/crc32.cpp

${OBJECTDIR}/common/base_c/os_sync_posix.o: nbproject/Makefile-${CND_CONF}.mk common/base_c/os_sync_posix.c 
	${MKDIR} -p ${OBJECTDIR}/common/base_c
	${RM} $@.d
	$(COMPILE.c) -O2 -Icommon -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_c/os_sync_posix.o common/base_c/os_sync_posix.c

${OBJECTDIR}/tnirps/main.o: nbproject/Makefile-${CND_CONF}.mk tnirps/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/main.o tnirps/main.cpp

${OBJECTDIR}/tnirps/tnirps_polynomial.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_polynomial.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_polynomial.o tnirps/tnirps_polynomial.cpp

${OBJECTDIR}/common/base_c/os_dir_posix.o: nbproject/Makefile-${CND_CONF}.mk common/base_c/os_dir_posix.c 
	${MKDIR} -p ${OBJECTDIR}/common/base_c
	${RM} $@.d
	$(COMPILE.c) -O2 -Icommon -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_c/os_dir_posix.o common/base_c/os_dir_posix.c

${OBJECTDIR}/tnirps/tnirps_reduction.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_reduction.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_reduction.o tnirps/tnirps_reduction.cpp

${OBJECTDIR}/common/base_c/os_tls_posix.o: nbproject/Makefile-${CND_CONF}.mk common/base_c/os_tls_posix.c 
	${MKDIR} -p ${OBJECTDIR}/common/base_c
	${RM} $@.d
	$(COMPILE.c) -O2 -Icommon -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_c/os_tls_posix.o common/base_c/os_tls_posix.c

${OBJECTDIR}/tnirps/tnirps_scheme_tree.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_scheme_tree.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_scheme_tree.o tnirps/tnirps_scheme_tree.cpp

${OBJECTDIR}/tnirps/tnirps_common.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_common.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_common.o tnirps/tnirps_common.cpp

${OBJECTDIR}/common/base_cpp/string_pool.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/string_pool.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/string_pool.o common/base_cpp/string_pool.cpp

${OBJECTDIR}/common/base_cpp/smart_output.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/smart_output.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/smart_output.o common/base_cpp/smart_output.cpp

${OBJECTDIR}/common/base_cpp/shmem_posix.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/shmem_posix.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/shmem_posix.o common/base_cpp/shmem_posix.cpp

${OBJECTDIR}/common/base_c/bitarray.o: nbproject/Makefile-${CND_CONF}.mk common/base_c/bitarray.c 
	${MKDIR} -p ${OBJECTDIR}/common/base_c
	${RM} $@.d
	$(COMPILE.c) -O2 -Icommon -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_c/bitarray.o common/base_c/bitarray.c

${OBJECTDIR}/tnirps/tnirps_bigint.o: nbproject/Makefile-${CND_CONF}.mk tnirps/tnirps_bigint.cpp 
	${MKDIR} -p ${OBJECTDIR}/tnirps
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/tnirps/tnirps_bigint.o tnirps/tnirps_bigint.cpp

${OBJECTDIR}/common/base_cpp/d_bitset.o: nbproject/Makefile-${CND_CONF}.mk common/base_cpp/d_bitset.cpp 
	${MKDIR} -p ${OBJECTDIR}/common/base_cpp
	${RM} $@.d
	$(COMPILE.cc) -O2 -Werror -Itnirps -Icommon -Icommon/base_c -Icommon/base_cpp -MMD -MP -MF $@.d -o ${OBJECTDIR}/common/base_cpp/d_bitset.o common/base_cpp/d_bitset.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/tnirps

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
