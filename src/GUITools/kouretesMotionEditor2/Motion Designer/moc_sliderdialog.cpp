/****************************************************************************
** Meta object code from reading C++ file 'sliderdialog.h'
**
** Created: Mon Mar 28 18:11:09 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "sliderdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sliderdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_sliderDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      34,   13,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_sliderDialog[] = {
    "sliderDialog\0\0slidersClosed(bool)\0"
    "poseChanged()\0"
};

const QMetaObject sliderDialog::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_sliderDialog,
      qt_meta_data_sliderDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &sliderDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *sliderDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *sliderDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_sliderDialog))
        return static_cast<void*>(const_cast< sliderDialog*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int sliderDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slidersClosed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: poseChanged(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void sliderDialog::slidersClosed(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void sliderDialog::poseChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
