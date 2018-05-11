################################################################
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
################################################################

defineReplace(qtadsLibraryTarget) {
    
    unset(LIBRARY_NAME)
    LIBRARY_NAME = $$1
    
    mac:contains(QTADS_CONFIG, QtADSFramework) {
        
        QMAKE_FRAMEWORK_BUNDLE_NAME = $$LIBRARY_NAME
        export(QMAKE_FRAMEWORK_BUNDLE_NAME)
    }
    
    contains(TEMPLATE, .*lib):CONFIG(debug, debug|release) {
        
        !debug_and_release|build_pass {
            
            mac:RET = $$member(LIBRARY_NAME, 0)_debug
            win32:RET = $$member(LIBRARY_NAME, 0)d
        }
    }
    
    isEmpty(RET):RET = $$LIBRARY_NAME
    return($$RET)
}

defineTest(qtadsAddLibrary) {
    
    LIB_PATH = $$1
    LIB_NAME = $$2
    
    mac:contains(QTADS_CONFIG, QtADSFramework) {
        
        LIBS         *= -F$${LIB_PATH}
        QMAKE_LFLAGS *= '-Wl,-rpath,\'$${LIB_PATH}\''
    }
    else {
        
        unix:lessThan(QT_MAJOR_VERSION, 5) {
            
            # Many Linux distributions install Qt third-party libraries in the same directory
            # as the Qt libs and thus we need to prepend the path for the local build
            # to avoid conflicting with the installed version.
            # Qt5 qmake appends ( instead of prepending ) the path to the Qt libs
            # to LIBS, but for Qt4 we need to use the QMAKE_LIBDIR_FLAGS.
            
            QMAKE_LIBDIR_FLAGS *= -L$${LIB_PATH}
        }
        else {
            LIBS *= -L$${LIB_PATH}
        }
    }
    
    unset(LINKAGE)
    
    mac:contains(QTADS_CONFIG, QtADSFramework) {
        
        LINKAGE = -framework $${LIB_NAME}
    }
    
    isEmpty(LINKAGE) {
        
        if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
            
            mac:LINKAGE = -l$${LIB_NAME}_debug
            win32:LINKAGE = -l$${LIB_NAME}d
        }
    }
    
    isEmpty(LINKAGE) {
        
        LINKAGE = -l$${LIB_NAME}
    }
    
    !isEmpty(QMAKE_LSB) {
        
        QMAKE_LFLAGS *= --lsb-shared-libs=$${LIB_NAME}
    }
    
    LIBS += $$LINKAGE
    export(LIBS)
    export(QMAKE_LFLAGS)
    export(QMAKE_LIBDIR_FLAGS)
    
    return(true)
}
