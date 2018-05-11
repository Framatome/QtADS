###############################################################################
# QtADS Library
# Copyright (C) 2012-2018   Framatome, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
###############################################################################

QTADS_ROOT = $${PWD}/..
include( $${QTADS_ROOT}/QtADSconfig.pri )
include( $${QTADS_ROOT}/QtADSbuild.pri )
include( $${QTADS_ROOT}/QtADSfunctions.pri )

QT       -= gui

TEMPLATE          = lib
TARGET            = $$qtadsLibraryTarget(QtADS)

DESTDIR           = $${OUT_PWD}/../lib

contains(QTADS_CONFIG, QtADSDll) {

    CONFIG += dll
    win32|symbian: DEFINES += QT_DLL QTADS_DLL QTADS_MAKEDLL

    mac {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
    }
    
    !mac:!isEmpty( QMAKE_LFLAGS_SONAME ) {
        
        # we increase the SONAME for every minor number
        
        QTADS_SONAME=libqtads.so.$${VER_MAJ}.$${VER_MIN}
        QMAKE_LFLAGS *= $${QMAKE_LFLAGS_SONAME}$${QTADS_SONAME}
        QMAKE_LFLAGS_SONAME=
    }
}
else {
    CONFIG += staticlib
} 

# Allow creation as framework on Mac.
contains(QTADS_CONFIG, QtADSFramework) {
    CONFIG += lib_bundle
}

HEADERS += \
    qadsglobal.h \
    qadssymbols.h \
    qadsarray.h \
    qadsarrayfloatingpointbase.h \
    qadsarrayintegerbase.h \
    qadsarrayofbool.h \
    qadsarrayofdint.h \
    qadsarrayofint.h \
    qadsarrayoflreal.h \
    qadsarrayofreal.h \
    qadsarrayofstring.h \
    qadsarrayofudint.h \
    qadsarrayofuint.h \
    qadsbool.h \
    qadsbyte.h \
    qadsdint.h \
    qadsdword.h \
    qadsenum.h \
    qadsfloatingpointbase.h \
    qadsint.h \
    qadsintegerbase.h \
    qadslreal.h \
    qadsobject.h \
    qadsplcvariable.h \
    qadsreal.h \
    qadssint.h \
    qadsstring.h \
    qadstime.h \
    qadsudint.h \
    qadsuint.h \
    qadsusint.h \
    qadsword.h

CLASSHEADERS += \
    QADSARRAYOFBOOL \
    QADSARRAYOFDINT \
    QADSARRAYOFINT \
    QADSARRAYOFLREAL \
    QADSARRAYOFREAL \
    QADSARRAYOFSTRING \
    QADSARRAYOFUDINT \
    QADSARRAYOFUINT \
    QADSBOOL \
    QADSBYTE \
    QADSDINT \
    QADSDWORD \
    QADSENUM \
    QADSINT \
    QADSLREAL \
    QADSObject \
    QADSPLCVariable \
    QADSREAL \
    QADSSINT \
    QADSSTRING \
    QADSTIME \
    QADSUDINT \
    QADSUINT \
    QADSUSINT \
    QADSWORD

SOURCES += \
    qadsarrayfloatingpointbase.cpp \
    qadsarrayintegerbase.cpp \
    qadsarrayofbool.cpp \
    qadsarrayofdint.cpp \
    qadsarrayofint.cpp \
    qadsarrayoflreal.cpp \
    qadsarrayofreal.cpp \
    qadsarrayofstring.cpp \
    qadsarrayofudint.cpp \
    qadsarrayofuint.cpp \
    qadsbool.cpp \
    qadsbyte.cpp \
    qadsdint.cpp \
    qadsdword.cpp \
    qadsenum.cpp \
    qadsfloatingpointbase.cpp \
    qadsint.cpp \
    qadsintegerbase.cpp \
    qadslreal.cpp \
    qadsobject.cpp \
    qadsplcvariable.cpp \
    qadsreal.cpp \
    qadssint.cpp \
    qadsstring.cpp \
    qadstime.cpp \
    qadsudint.cpp \
    qadsuint.cpp \
    qadsusint.cpp \
    qadsword.cpp


# Install directives

target.path    = $${QTADS_INSTALL_LIBS}

doc.files      = $${QTADS_ROOT}/doc/html
unix:doc.files += $${QTADS_ROOT}/doc/man
doc.path       = $${QTADS_INSTALL_DOCS}

INSTALLS       = target doc

# Allow creation as framework on Mac.
CONFIG(lib_bundle) {

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $${HEADERS}
    FRAMEWORK_HEADERS.files += $${CLASSHEADERS}
    FRAMEWORK_HEADERS.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
}
else {

    headers.files  = $${HEADERS}
    headers.files  += $${CLASSHEADERS}
    headers.path   = $${QTADS_INSTALL_HEADERS}
    INSTALLS += headers
}

contains(QTADS_CONFIG, QtADSPkgConfig) {
    
    CONFIG     += create_pc create_prl no_install_prl
    
    QMAKE_PKGCONFIG_NAME = QtADS$${QTADS_VER_MAJ}
    QMAKE_PKGCONFIG_DESCRIPTION = Qt TwinCAT Interface Library via ADS
    
    QMAKE_PKGCONFIG_LIBDIR = $${QTADS_INSTALL_LIBS}
    QMAKE_PKGCONFIG_INCDIR = $${QTADS_INSTALL_HEADERS}
    
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    
    greaterThan(QT_MAJOR_VERSION, 4) {
        
        QMAKE_PKGCONFIG_FILE = Qt$${QT_MAJOR_VERSION}$${QMAKE_PKGCONFIG_NAME}
        QMAKE_PKGCONFIG_REQUIRES = Qt5Core
        
        QMAKE_DISTCLEAN += $${DESTDIR}/$${QMAKE_PKGCONFIG_DESTDIR}/$${QMAKE_PKGCONFIG_FILE}.pc
    }
    else {
        
        # there is no QMAKE_PKGCONFIG_FILE fo Qt4
        QMAKE_PKGCONFIG_REQUIRES = QtCore
        
        QMAKE_DISTCLEAN += $${DESTDIR}/$${QMAKE_PKGCONFIG_DESTDIR}/$${TARGET}.pc
    }
    
    QMAKE_DISTCLEAN += $${DESTDIR}/libqtads.prl
}
