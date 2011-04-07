/****************************************************************************
** Meta object code from reading C++ file 'PlainTextEdit.h'
**
** Created: Fri Apr 8 08:38:06 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../PlainTextEdit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlainTextEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PlainTextEdit[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   14,   14,   14, 0x0a,
      43,   14,   14,   14, 0x0a,
      49,   14,   14,   14, 0x0a,
      57,   14,   14,   14, 0x0a,
      64,   14,   14,   14, 0x0a,
      71,   14,   14,   14, 0x0a,
      83,   14,   14,   14, 0x0a,
     105,   14,   14,   14, 0x0a,
     128,   14,   14,   14, 0x0a,
     149,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PlainTextEdit[] = {
    "PlainTextEdit\0\0showMessage(QString)\0"
    "copy()\0cut()\0paste()\0undo()\0redo()\0"
    "selectAll()\0setFontPointSize(int)\0"
    "setFontFamily(QString)\0makeFontBigger(bool)\0"
    "onBlockCountChanged()\0"
};

const QMetaObject PlainTextEdit::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_PlainTextEdit,
      qt_meta_data_PlainTextEdit, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PlainTextEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PlainTextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PlainTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PlainTextEdit))
        return static_cast<void*>(const_cast< PlainTextEdit*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int PlainTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void PlainTextEdit::showMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
