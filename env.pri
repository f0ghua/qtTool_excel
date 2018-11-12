
TOPSRCDIR   = $$PWD
TOPBUILDDIR = $$shadowed($$PWD)

DESTDIR 	= $$TOPBUILDDIR/output

LIBIDEVCTRLPATH     = $${TOPSRCDIR}/libs/iDevCtrl_qt
LIBQSLOGPATH        = $${TOPSRCDIR}/3rdparty/QsLog
LIBXlSXPATH         = $${TOPSRCDIR}/3rdparty/qtxlsx/src/xlsx

CONFIG(debug, debug|release) {
    LIBIDEVCTRL     = iDevCtrl_qtd
    LIBQSLOG        = QsLogd
    LIBXlSX         = Qt5Xlsxd
} else {
    LIBIDEVCTRL     = iDevCtrl_qt
    LIBQSLOG        = QsLog
    LIBXlSX         = Qt5Xlsx
}

