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

QTADS_VER_MAJ      = 1
QTADS_VER_MIN      = 0
QTADS_VER_PAT      = 0
QTADS_VERSION      = $${QTADS_VER_MAJ}.$${QTADS_VER_MIN}.$${QTADS_VER_PAT}

##############################################################################
# Install paths
##############################################################################

QTADS_INSTALL_PREFIX = $$[QT_INSTALL_PREFIX]

unix {
    QTADS_INSTALL_PREFIX    = /usr/local/qtads-$$QTADS_VERSION
}

win32 {
    wince60Beckhoff_HMI_600-x86-msvc2005 {
        QTADS_INSTALL_PREFIX    = C:/QtADS-$${QTADS_VERSION}-WinCE
    } else {
        QTADS_INSTALL_PREFIX    = C:/QtADS-$$QTADS_VERSION
    }
}

QTADS_INSTALL_DOCS      = $${QTADS_INSTALL_PREFIX}/doc
QTADS_INSTALL_HEADERS   = $${QTADS_INSTALL_PREFIX}/include
QTADS_INSTALL_LIBS      = $${QTADS_INSTALL_PREFIX}/lib

######################################################################
# Features
# When building a QtADS application with qmake you might want to load
# the compiler/linker flags, that are required to build a QtADS application
# from QtADS.prf. Therefore all you need to do is to add "CONFIG += QtADS"
# to your project file and take care, that QtADS.prf can be found by qmake.
# ( see http://doc.qt.io/qt-5/qmake-advanced-usage.html#adding-new-configuration-features )
# I recommend not to install the QtADS features together with the
# Qt features, because you will have to reinstall the QtADS features,
# with every Qt upgrade. 
######################################################################

QTADS_INSTALL_FEATURES  = $${QTADS_INSTALL_PREFIX}/features

######################################################################
# Build the static/shared libraries.
# If QtADSDll is enabled, a shared library is built, otherwise
# it will be a static library.
######################################################################

!wince60Beckhoff_HMI_600-x86-msvc2005 {
    QTADS_CONFIG += QtADSDll
}

######################################################################
# If you want to auto build the tests, enable the line below
# Otherwise you have to build them from the tests directory.
######################################################################

QTADS_CONFIG           += QtADSTests

######################################################################
# When Qt has been built as framework qmake ( qtAddLibrary ) wants 
# to link frameworks instead of regular libs
######################################################################

macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
    QTADS_CONFIG += QtADSFramework
}

######################################################################
# Create and install pc files for pkg-config
# See http://www.freedesktop.org/wiki/Software/pkg-config/
# Also must use ADSLib since TwinCAT runs only on Windows OS
# The user must specify ADSPATH in qmake or the build will fail
######################################################################

unix {
    
    QTADS_CONFIG     += QtADSPkgConfig
}

######################################################################
# On Windows use TwinCAT3 installation by default
# To use ADSLib comment out the code below and set ADSPATH to the
# ADSLib build path
######################################################################

win32 {

    QTADS_CONFIG     += QtADSTwinCAT3
}
