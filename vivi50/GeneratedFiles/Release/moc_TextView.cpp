/****************************************************************************
** Meta object code from reading C++ file 'TextView.h'
**
** Created: Tue Apr 12 20:18:49 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../TextView.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,    9,    9,    9, 0x0a,
      38,    9,    9,    9, 0x0a,
      44,    9,    9,    9, 0x0a,
      52,    9,    9,    9, 0x0a,
      59,    9,    9,    9, 0x0a,
      66,    9,    9,    9, 0x0a,
      78,    9,    9,    9, 0x0a,
     100,    9,    9,    9, 0x0a,
     123,    9,    9,    9, 0x0a,
     144,    9,    9,    9, 0x09,
     166,    9,    9,    9, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_TextView[] = {
    "TextView\0\0showMessage(QString)\0copy()\0"
    "cut()\0paste()\0undo()\0redo()\0selectAll()\0"
    "setFontPointSize(int)\0setFontFamily(QString)\0"
    "makeFontBigger(bool)\0onBlockCountChanged()\0"
    "onTimer()\0"
};

const QMetaObject TextView::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_TextView,
      qt_meta_data_TextView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextView))
        return static_cast<void*>(const_cast< TextView*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int TextView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: copy(); break;
        case 2: cut(); break;
        case 3: paste(); break;
        case 4: undo(); break;
        case 5: redo(); break;
        case 6: selectAll(); break;
        case 7: setFontPointSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: setFontFamily((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: makeFontBigger((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: onBlockCountChanged(); break;
        case 11: onTimer(); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void TextView::showMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
