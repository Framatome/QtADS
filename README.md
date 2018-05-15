# QtADS
The QtADS library contains utility classes for integrating Qt with Beckhoff's [TwinCAT](https://www.beckhoff.com/english.asp?twincat/default.htm) real-time software PLC system using optionally the open-source [ADS library](https://github.com/Beckhoff/ADS) or on Windows operating systems the proprietary ADS library installed with TwinCAT (the default for Windows).

The library only depends on QtCore so it can be used as a [command-line utility](https://github.com/Framatome/QtADS/tree/master/tests/qtadstest) without any GUI dependencies.  It also works well in both classical [Widgets based desktop](https://github.com/Framatome/QtADS/tree/master/example/QtWidgetsADSExample) applications or in QML applications (untested).
