/****************************************************************************
** Meta object code from reading C++ file 'TextDocument.h'
**
** Created: Sat Apr 9 13:53:41 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../TextDocument.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextDocument.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextDocument[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      47,   14,   13,   13, 0x05,
      85,   13,   13,   13, 0x05,
     103,   13,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TextDocument[] = {
    "TextDocument\0\0position,charsRemoved,charsAdded\0"
    "contentsChange(index_t,size_t,size_t)\0"
    "contentsChanged()\0blockCountChanged()\0"
};

const QMetaObject TextDocument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TextDocument,
      qt_meta_data_TextDocument, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextDocument::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextDocument))
        return static_cast<void*>(const_cast< TextDocument*>(this));
    return QObject::qt_metacast(_clname);
}

int TextDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contentsChange((*reinterpret_cast< index_t(*)>(_a[1])),(*reinterpret_cast< size_t(*)>(_a[2])),(*reinterpret_cast< size_t(*)>(_a[3]))); break;
        case 1: contentsChanged(); break;
        case 2: blockCountChanged(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void TextDocument::contentsChange(index_t _t1, size_t _t2, size_t _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TextDocument::contentsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void TextDocument::blockCountChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
