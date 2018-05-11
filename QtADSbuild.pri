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

###############################################################################
# qmake internal options
###############################################################################

CONFIG           += qt
CONFIG           += no_keywords
CONFIG           += warn_on

###############################################################################
# include ADS headers
###############################################################################

# Try to find the ADS libraries automatically.
# On Windows it will search for TwinCAT installations
# using the registry.  Unix OS's will have to use the
# open-source ADS library (https://github.com/Beckhoff/ADS)
# and specify ADSPATH manually.

!isEmpty(ADSPATH):!exists($${ADSPATH}) {
    ADSPATH =
}

win32:contains(QTADS_CONFIG, QtADSTwinCAT3) {

    isEmpty(ADSPATH) {
        ADSPATH = C:/TwinCAT3/
        !exists($${ADSPATH}):ADSPATH =

        isEmpty(ADSPATH) {
            message( "Searching for TwinCAT3 32-bit" )
            SYSTEM_CMD = reg query HKEY_LOCAL_MACHINE\\SOFTWARE\\Beckhoff\\TwinCAT3 /v "TwinCATDir"
            system($${SYSTEM_CMD}) {
                SYSTEM_RET = $$system($${SYSTEM_CMD})
                FORLOOP = 3 4 5 6 7
                for(i, FORLOOP):ADSPATH += $$member(SYSTEM_RET, $${i})
            }
        }

        isEmpty(ADSPATH) {
            message( "Searching for TwinCAT3 64-bit" )
            SYSTEM_CMD = reg query HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Beckhoff\\TwinCAT3 /v "TwinCATDir"
            system($${SYSTEM_CMD}) {
                SYSTEM_RET = $$system($${SYSTEM_CMD})
                FORLOOP = 3 4 5 6 7
                for(i, FORLOOP):ADSPATH += $$member(SYSTEM_RET, $${i})
            }
        }
    }
}

win32:contains(QTADS_CONFIG, QtADSTwinCAT2) {
    isEmpty(ADSPATH) {
        message( "Searching for TwinCAT2 32-bit" )
        SYSTEM_CMD = reg query HKEY_LOCAL_MACHINE\\SOFTWARE\\Beckhoff\\TwinCAT /v "InstallationPath"
        system($${SYSTEM_CMD}) {
            SYSTEM_RET = $$system($${SYSTEM_CMD})
            FORLOOP = 7 8 9 10 11 12 13 14 15 16
            for(i, FORLOOP):ADSPATH += $$member(SYSTEM_RET, $${i})
        }
    }

    isEmpty(ADSPATH) {
        message( "Searching for TwinCAT2 64-bit" )
        SYSTEM_CMD = reg query HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Beckhoff\\TwinCAT /v "InstallationPath"
        system($${SYSTEM_CMD}) {
            SYSTEM_RET = $$system($${SYSTEM_CMD})
            FORLOOP = 3 4 5 6 7
            for(i, FORLOOP):ADSPATH += $$member(SYSTEM_RET, $${i})
        }
    }
}

isEmpty(ADSPATH):error("This library requires ADS but ADSPATH was not specified or an installation of TwinCAT was not found.")
message( "ADSPATH = $${ADSPATH}" )

win32:contains(QTADS_CONFIG, QtADSTwinCAT2) | contains(QTADS_CONFIG, QtADSTwinCAT3) {

    DEFINES *= USE_TWINCAT

    # Use WinCE target first if available
    wince60Beckhoff_HMI_600-x86-msvc2005 {

        message( "Building for TwinCAT on WinCE" )

        contains(QTADS_CONFIG, QtADSTwinCAT3) {
            ADSINCLUDE = "$${ADSPATH}AdsApi/TcAdsDll/Ce/Include"
            ADSLIB = "$${ADSPATH}AdsApi/TcAdsDll/Ce/lib/x86/TcAdsDllCe.lib"
        } else {
            ADSINCLUDE = "$${ADSPATH}ADS Api/TcAdsDllCe/Include"
            ADSLIB = "$${ADSPATH}ADS Api/TcAdsDllCe/lib/x86/TcAdsDllCe.lib"
        }

    } else  {
        contains(QTADS_CONFIG, QtADSTwinCAT3) {

            ADSINCLUDE = "$${ADSPATH}AdsApi/TcAdsDll/Include"
            contains(QMAKE_TARGET.arch, x86_64):{
                message( "Building for 64 bit TwinCAT3")
                ADSLIB = "$${ADSPATH}AdsApi/TcAdsDll/x64/lib/TcAdsDll.lib"
            }

            contains(QMAKE_TARGET.arch, x86):{
                message( "Building for 32 bit TwinCAT3")
                ADSLIB = "$${ADSPATH}AdsApi/TcAdsDll/Lib/TcAdsDll.lib"
            }

        } else {

            ADSINCLUDE = "$${ADSPATH}ADS Api/TcAdsDll/Include"
            contains(QMAKE_TARGET.arch, x86_64):{
                message( "Building for 64 bit TwinCAT2")
                ADSLIB = "$${ADSPATH}ADS Api/TcAdsDll/x64/lib/TcAdsDll.lib"
            }

            contains(QMAKE_TARGET.arch, x86):{
                message( "Building for 32 bit TwinCAT2")
                ADSLIB *= "$${ADSPATH}ADS Api/TcAdsDll/Lib/TcAdsDll.lib"
            }
        }
    }

    !exists($${ADSINCLUDE}):error("Unable to locate  ADS include path $${ADSINCLUDE}")
    !exists($${ADSLIB}):error("Unable to locate  ADSLib $${ADSLIB}")
    INCLUDEPATH *= $${ADSINCLUDE}
    LIBS *= $${ADSLIB}
} else {
    
    message( "Building for static ADS library")
    INCLUDEPATH *= "$${ADSPATH}AdsLib"

    unix {
        SYSTEM_RET = $$system(uname)
        exists("$${ADSPATH}AdsLib-$${SYSTEM_RET}.a") {
            LIBS *= "$${ADSPATH}AdsLib-$${SYSTEM_RET}.a"
        } else {
            error("Unable to locate ADSLib: $${ADSPATH}AdsLib-$${SYSTEM_RET}.a")
        }
    } else {
        exists("$${ADSPATH}Release/AdsLib.lib"):release {
            LIBS *= "$${ADSPATH}Release/AdsLib.lib"
        } else {
            error("Unable to locate ADSLib: $${ADSPATH}Release/AdsLib.lib")
        }
        exists("$${ADSPATH}Debug/AdsLib.lib"):debug {
            LIBS *= "$${ADSPATH}Debug/AdsLib.lib"
        }
    }
    
}

######################################################################
# release/debug mode
######################################################################

win32 {
    # On Windows you can't mix release and debug libraries.
    # The designer is built in release mode. If you like to use it
    # you need a release version. For your own application development you
    # might need a debug version. 
    # Enable debug_and_release + build_all if you want to build both.

    CONFIG           += debug_and_release
    CONFIG           += build_all
} else {

    CONFIG           += release

    VER_MAJ           = $${QTADS_VER_MAJ}
    VER_MIN           = $${QTADS_VER_MIN}
    VER_PAT           = $${QTADS_VER_PAT}
    VERSION           = $${QTADS_VERSION}
}

######################################################################
# paths for building QtADS
######################################################################

MOC_DIR      = moc
RCC_DIR      = resources
!debug_and_release {
    OBJECTS_DIR       = obj
}

