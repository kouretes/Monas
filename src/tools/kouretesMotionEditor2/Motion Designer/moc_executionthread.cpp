/****************************************************************************
** Meta object code from reading C++ file 'executionthread.h'
**
** Created: Mon Mar 28 03:09:26 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "executionthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'executionthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_executionThread[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      35,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_executionThread[] = {
    "executionThread\0\0stepExecuted(int)\0"
    "finished()\0killThread()\0"
};

const QMetaObject executionThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_executionThread,
      qt_meta_data_executionThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &executionThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *executionThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *executionThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_executionThread))
        return static_cast<void*>(const_cast< executionThread*>(this));
    return QThread::qt_metacast(_clname);
}

int executionThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stepExecuted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: finished(); break;
        case 2: killThread(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void executionThread::stepExecuted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void executionThread::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
