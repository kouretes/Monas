/****************************************************************************
** Meta object code from reading C++ file 'jointsliders.h'
**
** Created: Mon Mar 28 11:36:19 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "jointsliders.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jointsliders.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_JointSliders[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_JointSliders[] = {
    "JointSliders\0\0poseChanged()\0"
};

const QMetaObject JointSliders::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_JointSliders,
      qt_meta_data_JointSliders, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &JointSliders::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *JointSliders::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *JointSliders::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_JointSliders))
        return static_cast<void*>(const_cast< JointSliders*>(this));
    return QWidget::qt_metacast(_clname);
}

int JointSliders::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: poseChanged(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void JointSliders::poseChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
