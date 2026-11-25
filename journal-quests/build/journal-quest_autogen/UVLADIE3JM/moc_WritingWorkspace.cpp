/****************************************************************************
** Meta object code from reading C++ file 'WritingWorkspace.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/WritingWorkspace.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WritingWorkspace.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16WritingWorkspaceE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN16WritingWorkspaceE = QtMocHelpers::stringData(
    "WritingWorkspace",
    "entrySaved",
    "",
    "DiaryEntry",
    "entry",
    "requestToggleFocus",
    "docxFileDropped",
    "filePath",
    "manualSave",
    "onContentOrTitleChanged",
    "performAutoSave",
    "setBold",
    "setItalic",
    "setUnderline",
    "setStrike",
    "setAlignLeft",
    "setAlignCenter",
    "setAlignRight",
    "insertBulletList",
    "insertNumberedList",
    "insertQuote",
    "insertTable",
    "onHeadingChanged",
    "index"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN16WritingWorkspaceE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  122,    2, 0x06,    1 /* Public */,
       5,    0,  125,    2, 0x06,    3 /* Public */,
       6,    1,  126,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,  129,    2, 0x0a,    6 /* Public */,
       9,    0,  130,    2, 0x08,    7 /* Private */,
      10,    0,  131,    2, 0x08,    8 /* Private */,
      11,    0,  132,    2, 0x08,    9 /* Private */,
      12,    0,  133,    2, 0x08,   10 /* Private */,
      13,    0,  134,    2, 0x08,   11 /* Private */,
      14,    0,  135,    2, 0x08,   12 /* Private */,
      15,    0,  136,    2, 0x08,   13 /* Private */,
      16,    0,  137,    2, 0x08,   14 /* Private */,
      17,    0,  138,    2, 0x08,   15 /* Private */,
      18,    0,  139,    2, 0x08,   16 /* Private */,
      19,    0,  140,    2, 0x08,   17 /* Private */,
      20,    0,  141,    2, 0x08,   18 /* Private */,
      21,    0,  142,    2, 0x08,   19 /* Private */,
      22,    1,  143,    2, 0x08,   20 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   23,

       0        // eod
};

Q_CONSTINIT const QMetaObject WritingWorkspace::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN16WritingWorkspaceE.offsetsAndSizes,
    qt_meta_data_ZN16WritingWorkspaceE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN16WritingWorkspaceE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WritingWorkspace, std::true_type>,
        // method 'entrySaved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DiaryEntry &, std::false_type>,
        // method 'requestToggleFocus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'docxFileDropped'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'manualSave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onContentOrTitleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'performAutoSave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setBold'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setItalic'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setUnderline'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setStrike'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setAlignLeft'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setAlignCenter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setAlignRight'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'insertBulletList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'insertNumberedList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'insertQuote'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'insertTable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onHeadingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void WritingWorkspace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WritingWorkspace *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->entrySaved((*reinterpret_cast< std::add_pointer_t<DiaryEntry>>(_a[1]))); break;
        case 1: _t->requestToggleFocus(); break;
        case 2: _t->docxFileDropped((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->manualSave(); break;
        case 4: _t->onContentOrTitleChanged(); break;
        case 5: _t->performAutoSave(); break;
        case 6: _t->setBold(); break;
        case 7: _t->setItalic(); break;
        case 8: _t->setUnderline(); break;
        case 9: _t->setStrike(); break;
        case 10: _t->setAlignLeft(); break;
        case 11: _t->setAlignCenter(); break;
        case 12: _t->setAlignRight(); break;
        case 13: _t->insertBulletList(); break;
        case 14: _t->insertNumberedList(); break;
        case 15: _t->insertQuote(); break;
        case 16: _t->insertTable(); break;
        case 17: _t->onHeadingChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (WritingWorkspace::*)(const DiaryEntry & );
            if (_q_method_type _q_method = &WritingWorkspace::entrySaved; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (WritingWorkspace::*)();
            if (_q_method_type _q_method = &WritingWorkspace::requestToggleFocus; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (WritingWorkspace::*)(const QString & );
            if (_q_method_type _q_method = &WritingWorkspace::docxFileDropped; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *WritingWorkspace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WritingWorkspace::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN16WritingWorkspaceE.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int WritingWorkspace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void WritingWorkspace::entrySaved(const DiaryEntry & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WritingWorkspace::requestToggleFocus()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void WritingWorkspace::docxFileDropped(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
