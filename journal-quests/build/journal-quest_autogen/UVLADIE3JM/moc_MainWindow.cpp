/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/MainWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10MainWindowE = QtMocHelpers::stringData(
    "MainWindow",
    "onNavItemSelected",
    "",
    "QListWidgetItem*",
    "item",
    "createNewEntry",
    "openEntryById",
    "id",
    "openRandomMemory",
    "onWritingEntrySaved",
    "DiaryEntry",
    "entry",
    "onDetailsChanged",
    "toggleFocusMode",
    "importDocxFile",
    "specificPath",
    "exportCurrentAsDocx",
    "saveBackToOriginalDocx",
    "moveCurrentToTrash",
    "restoreEntry",
    "permanentDeleteEntry",
    "exportBackupJson",
    "importBackupJson",
    "applyTheme",
    "toggleSidebar"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  122,    2, 0x08,    1 /* Private */,
       5,    0,  125,    2, 0x08,    3 /* Private */,
       6,    1,  126,    2, 0x08,    4 /* Private */,
       8,    0,  129,    2, 0x08,    6 /* Private */,
       9,    1,  130,    2, 0x08,    7 /* Private */,
      12,    0,  133,    2, 0x08,    9 /* Private */,
      13,    0,  134,    2, 0x08,   10 /* Private */,
      14,    1,  135,    2, 0x08,   11 /* Private */,
      14,    0,  138,    2, 0x28,   13 /* Private | MethodCloned */,
      16,    0,  139,    2, 0x08,   14 /* Private */,
      17,    0,  140,    2, 0x08,   15 /* Private */,
      18,    0,  141,    2, 0x08,   16 /* Private */,
      19,    1,  142,    2, 0x08,   17 /* Private */,
      20,    1,  145,    2, 0x08,   19 /* Private */,
      21,    0,  148,    2, 0x08,   21 /* Private */,
      22,    0,  149,    2, 0x08,   22 /* Private */,
      23,    0,  150,    2, 0x08,   23 /* Private */,
      24,    0,  151,    2, 0x08,   24 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    7,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    7,
    QMetaType::Void, QMetaType::LongLong,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN10MainWindowE.offsetsAndSizes,
    qt_meta_data_ZN10MainWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10MainWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'onNavItemSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'createNewEntry'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openEntryById'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'openRandomMemory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onWritingEntrySaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DiaryEntry &, std::false_type>,
        // method 'onDetailsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleFocusMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'importDocxFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'importDocxFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exportCurrentAsDocx'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveBackToOriginalDocx'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'moveCurrentToTrash'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restoreEntry'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'permanentDeleteEntry'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'exportBackupJson'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'importBackupJson'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'applyTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleSidebar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onNavItemSelected((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 1: _t->createNewEntry(); break;
        case 2: _t->openEntryById((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 3: _t->openRandomMemory(); break;
        case 4: _t->onWritingEntrySaved((*reinterpret_cast< std::add_pointer_t<DiaryEntry>>(_a[1]))); break;
        case 5: _t->onDetailsChanged(); break;
        case 6: _t->toggleFocusMode(); break;
        case 7: _t->importDocxFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->importDocxFile(); break;
        case 9: _t->exportCurrentAsDocx(); break;
        case 10: _t->saveBackToOriginalDocx(); break;
        case 11: _t->moveCurrentToTrash(); break;
        case 12: _t->restoreEntry((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 13: _t->permanentDeleteEntry((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 14: _t->exportBackupJson(); break;
        case 15: _t->importBackupJson(); break;
        case 16: _t->applyTheme(); break;
        case 17: _t->toggleSidebar(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10MainWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
    }
    return _id;
}
QT_WARNING_POP
