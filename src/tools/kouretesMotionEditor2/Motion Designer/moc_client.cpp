/****************************************************************************
** Meta object code from reading C++ file 'client.h'
**
** Created: Mon Mar 28 03:09:27 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "client.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Client[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      25,    7,    7,    7, 0x0a,
      37,    7,    7,    7, 0x0a,
      48,    7,    7,    7, 0x0a,
      79,   67,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Client[] = {
    "Client\0\0newData(QString)\0connectTo()\0"
    "readData()\0writeData(QString)\0socketError\0"
    "displayError(QAbstractSocket::SocketError)\0"
};

const QMetaObject Client::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Client,
      qt_meta_data_Client, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Client::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Client::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Client))
        return static_cast<void*>(const_cast< Client*>(this));
    return QWidget::qt_metacast(_clname);
}

int Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: connectTo(); break;
        case 2: readData(); break;
        case 3: writeData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: displayError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Client::newData(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
