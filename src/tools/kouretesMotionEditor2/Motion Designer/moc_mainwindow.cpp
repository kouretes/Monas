/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Mar 28 18:11:07 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      33,   11,   11,   11, 0x08,
      46,   11,   42,   11, 0x08,
      58,   11,   11,   11, 0x08,
      70,   11,   11,   11, 0x08,
      90,   11,   11,   11, 0x08,
     104,   11,   11,   11, 0x08,
     116,   11,   11,   11, 0x08,
     129,   11,   11,   11, 0x08,
     147,   11,   11,   11, 0x08,
     158,   11,   11,   11, 0x08,
     168,   11,   11,   11, 0x08,
     179,   11,   11,   11, 0x08,
     194,   11,   11,   11, 0x08,
     213,   11,   11,   11, 0x08,
     228,   11,   11,   11, 0x08,
     245,   11,   11,   11, 0x08,
     258,   11,   11,   11, 0x08,
     273,   11,   11,   11, 0x08,
     291,   11,   11,   11, 0x08,
     310,   11,   11,   11, 0x08,
     327,   11,   11,   11, 0x08,
     344,   11,   11,   11, 0x08,
     363,   11,   11,   11, 0x08,
     379,   11,   11,   11, 0x08,
     396,   11,   11,   11, 0x08,
     412,   11,   11,   11, 0x08,
     423,   11,   11,   11, 0x08,
     454,   11,  446,   11, 0x08,
     474,   11,  446,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_stiffOn_clicked()\0"
    "myShow()\0int\0maybeSave()\0showAbout()\0"
    "showDocumentation()\0exportToKME()\0"
    "viewGraph()\0viewJoints()\0setNaoAcademics()\0"
    "setNaoRC()\0newFile()\0openFile()\0"
    "loadNewRobot()\0somethingChanged()\0"
    "moveUpAction()\0moveDownAction()\0"
    "swapAction()\0removeAction()\0"
    "storePoseAction()\0insertPoseAction()\0"
    "gotoPoseAction()\0stepPoseAction()\0"
    "playMotionAction()\0stiffOnAction()\0"
    "stiffOffAction()\0connectAction()\0"
    "sendPose()\0decodeCommand(QString)\0"
    "QString\0compileMessage(int)\0"
    "compileMessage()\0"
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
        case 0: on_stiffOn_clicked(); break;
        case 1: myShow(); break;
        case 2: { int _r = maybeSave();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: showAbout(); break;
        case 4: showDocumentation(); break;
        case 5: exportToKME(); break;
        case 6: viewGraph(); break;
        case 7: viewJoints(); break;
        case 8: setNaoAcademics(); break;
        case 9: setNaoRC(); break;
        case 10: newFile(); break;
        case 11: openFile(); break;
        case 12: loadNewRobot(); break;
        case 13: somethingChanged(); break;
        case 14: moveUpAction(); break;
        case 15: moveDownAction(); break;
        case 16: swapAction(); break;
        case 17: removeAction(); break;
        case 18: storePoseAction(); break;
        case 19: insertPoseAction(); break;
        case 20: gotoPoseAction(); break;
        case 21: stepPoseAction(); break;
        case 22: playMotionAction(); break;
        case 23: stiffOnAction(); break;
        case 24: stiffOffAction(); break;
        case 25: connectAction(); break;
        case 26: sendPose(); break;
        case 27: decodeCommand((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 28: { QString _r = compileMessage((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 29: { QString _r = compileMessage();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 30;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
