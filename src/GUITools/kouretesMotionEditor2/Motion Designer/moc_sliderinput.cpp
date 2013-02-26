/****************************************************************************
** Meta object code from reading C++ file 'sliderinput.h'
**
** Created: Mon Mar 28 03:09:21 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "sliderinput.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sliderinput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_sliderInput[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      27,   12,   12,   12, 0x0a,
      45,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_sliderInput[] = {
    "sliderInput\0\0poseChanged()\0setMySpinBox(int)\0"
    "setMySlider(double)\0"
};

const QMetaObject sliderInput::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_sliderInput,
      qt_meta_data_sliderInput, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &sliderInput::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *sliderInput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *sliderInput::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_sliderInput))
        return static_cast<void*>(const_cast< sliderInput*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int sliderInput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: poseChanged(); break;
        case 1: setMySpinBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setMySlider((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void sliderInput::poseChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
