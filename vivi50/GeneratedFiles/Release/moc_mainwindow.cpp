/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Wed Apr 13 13:04:44 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      34,   11,   11,   11, 0x08,
      44,   11,   11,   11, 0x08,
      56,   11,   51,   11, 0x08,
      63,   11,   51,   11, 0x08,
      72,   11,   11,   11, 0x08,
      86,   11,   11,   11, 0x08,
     100,   11,   11,   11, 0x08,
     125,  117,   11,   11, 0x08,
     137,   11,   11,   11, 0x08,
     157,  152,   11,   11, 0x08,
     178,   11,   11,   11, 0x08,
     192,   11,   11,   11, 0x08,
     199,   11,   11,   11, 0x08,
     212,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0documentWasModified()\0"
    "newFile()\0open()\0bool\0save()\0saveAs()\0"
    "open(QString)\0save(QString)\0"
    "openRecentFile()\0lineNum\0doJump(int)\0"
    "showAboutDlg()\0text\0showMessage(QString)\0"
    "printBuffer()\0font()\0doUnitTest()\0"
    "doBenchmark()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: documentWasModified(); break;
        case 1: newFile(); break;
        case 2: open(); break;
        case 3: { bool _r = save();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: { bool _r = saveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: open((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: save((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: openRecentFile(); break;
        case 8: doJump((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: showAboutDlg(); break;
        case 10: showMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: printBuffer(); break;
        case 12: font(); break;
        case 13: doUnitTest(); break;
        case 14: doBenchmark(); break;
        default: ;
        }
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
