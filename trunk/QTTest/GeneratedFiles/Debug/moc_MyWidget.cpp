/****************************************************************************
** Meta object code from reading C++ file 'MyWidget.h'
**
** Created: Thu 17. May 09:40:35 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MyWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyMainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   14,   13,   13, 0x08,
      49,   14,   13,   13, 0x08,
      82,   14,   13,   13, 0x08,
     115,   14,   13,   13, 0x08,
     146,   14,   13,   13, 0x08,
     177,   14,   13,   13, 0x08,
     208,   14,   13,   13, 0x08,
     239,   14,   13,   13, 0x08,
     270,   14,   13,   13, 0x08,
     301,   14,   13,   13, 0x08,
     321,   13,   13,   13, 0x08,
     338,   13,   13,   13, 0x08,
     353,   13,   13,   13, 0x08,
     360,   13,   13,   13, 0x08,
     376,   13,   13,   13, 0x08,
     393,   13,   13,   13, 0x08,
     411,  405,   13,   13, 0x08,
     437,   13,   13,   13, 0x08,
     447,   13,   13,   13, 0x08,
     474,   13,   13,   13, 0x08,
     483,   13,   13,   13, 0x08,
     493,   13,   13,   13, 0x08,
     505,   13,   13,   13, 0x08,
     516,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MyMainWindow[] = {
    "MyMainWindow\0\0v\0changeScaleCounterValueX(double)\0"
    "changeScaleCounterValueY(double)\0"
    "changeScaleCounterValueZ(double)\0"
    "changeRotCounterValueX(double)\0"
    "changeRotCounterValueY(double)\0"
    "changeRotCounterValueZ(double)\0"
    "changeTraCounterValueX(double)\0"
    "changeTraCounterValueY(double)\0"
    "changeTraCounterValueZ(double)\0"
    "test_comboAnim(int)\0ClearComboAnim()\0"
    "changeColour()\0test()\0destroyEntity()\0"
    "CreateNewLight()\0SetShadow()\0texts\0"
    "SetComboAnim(QStringList)\0SetAnim()\0"
    "SetPositionSelctedEntity()\0rotate()\0"
    "scaleUp()\0scaleDown()\0pick_one()\0"
    "debug_row()\0"
};

void MyMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MyMainWindow *_t = static_cast<MyMainWindow *>(_o);
        switch (_id) {
        case 0: _t->changeScaleCounterValueX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->changeScaleCounterValueY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->changeScaleCounterValueZ((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->changeRotCounterValueX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->changeRotCounterValueY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->changeRotCounterValueZ((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->changeTraCounterValueX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->changeTraCounterValueY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->changeTraCounterValueZ((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->test_comboAnim((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->ClearComboAnim(); break;
        case 11: _t->changeColour(); break;
        case 12: _t->test(); break;
        case 13: _t->destroyEntity(); break;
        case 14: _t->CreateNewLight(); break;
        case 15: _t->SetShadow(); break;
        case 16: _t->SetComboAnim((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 17: _t->SetAnim(); break;
        case 18: _t->SetPositionSelctedEntity(); break;
        case 19: _t->rotate(); break;
        case 20: _t->scaleUp(); break;
        case 21: _t->scaleDown(); break;
        case 22: _t->pick_one(); break;
        case 23: _t->debug_row(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MyMainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MyMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MyMainWindow,
      qt_meta_data_MyMainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyMainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyMainWindow))
        return static_cast<void*>(const_cast< MyMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MyMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
