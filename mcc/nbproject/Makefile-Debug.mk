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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/29de025/ast.o \
	${OBJECTDIR}/_ext/29de025/parser.o \
	${OBJECTDIR}/_ext/29de025/stream_utils.o \
	${OBJECTDIR}/_ext/29de025/string_utils.o \
	${OBJECTDIR}/src/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mcc

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mcc: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mcc ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/29de025/ast.o: ../mC/ast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29de025
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../mC -Iinclude -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29de025/ast.o ../mC/ast.cpp

${OBJECTDIR}/_ext/29de025/parser.o: ../mC/parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29de025
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../mC -Iinclude -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29de025/parser.o ../mC/parser.cpp

${OBJECTDIR}/_ext/29de025/stream_utils.o: ../mC/stream_utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29de025
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../mC -Iinclude -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29de025/stream_utils.o ../mC/stream_utils.cpp

${OBJECTDIR}/_ext/29de025/string_utils.o: ../mC/string_utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29de025
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../mC -Iinclude -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29de025/string_utils.o ../mC/string_utils.cpp

${OBJECTDIR}/src/main.o: src/main.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -I../mC -Iinclude -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mcc

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
