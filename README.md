# LQObjectSerializer
LQObjectSerializer is a small library that I use to serialize and deserialize data automatically in the form of QObject's. This is a bit similar to what gson does for Java and Kotlin, althought much simpler.

The entire library is contained in a header and a source file. You can include those files directly or include with cmake.

## Example

Given is this json string:
```json
{"menu": {
    "header": "SVG Viewer",
    "items": [
        {"id": "Open"},
        {"id": "OpenNew", "label": "Open New"},
        null,
        {"id": "ZoomIn", "label": "Zoom In"},
        {"id": "ZoomOut", "label": "Zoom Out"},
        {"id": "OriginalView", "label": "Original View"},
        null,
        {"id": "Quality"},
        {"id": "Pause"},
        {"id": "Mute"},
        null,
        {"id": "Find", "label": "Find..."},
        {"id": "FindAgain", "label": "Find Again"},
        {"id": "Copy"},
        {"id": "CopyAgain", "label": "Copy Again"},
        {"id": "CopySVG", "label": "Copy SVG"},
        {"id": "ViewSVG", "label": "View SVG"},
        {"id": "ViewSource", "label": "View Source"},
        {"id": "SaveAs", "label": "Save As"},
        null,
        {"id": "Help"},
        {"id": "About", "label": "About Adobe CVG Viewer..."}
    ]
}}
```
I like to deserialize it to structured Qt data, that I write like this:
```c++
L_BEGIN_CLASS(Item)
L_RW_PROP_AS(QString, id)
L_RW_PROP_AS(QString, label)
L_END_CLASS

L_BEGIN_CLASS(Menu)
L_RW_PROP_AS(QString, header)
L_RW_PROP_ARRAY_WITH_ADDER(Item*, items, setItems)
L_END_CLASS

L_BEGIN_CLASS(MenuRoot)
L_RW_PROP_AS(Menu*, menu, nullptr)
L_END_CLASS
```
with a call to:
```c++
lqo::Deserializer<MenuRoot> deserializer;
QScopedPointer<MenuRoot> g(deserializer.deserialize(jsonString));
```
Only properties can be serialized and deserialized.

The macros you see are provided by another Qt module I wrote: [lqtutils](https://github.com/carlonluca/lqtutils.git). All the macros are included in a single header, no need to add dependent libs.

Those strongly reduce the amount of lines needed to define Qt properties. You can however use this module without it, writing regular Qt properties. `Item`, `Menu` and `MenuRoot` are simple QObject's with regular `Q_PROPERTY`'s.

## JSON deserialization to `QObject`

To deserialize a JSON to QObject, simply create your model using `QObject`'s or Qt gadgets, adding the needed `Q_PROPERTY`'s to match the structure of the JSON (see example above). The types provided by JSON are less than those available in Qt/C++. This is a schema of the available conversions:

| JSON | lqobjectserializer |
| --- | ----------- |
| string | `QString` or any C++ object by providing a proper `lqo::Stringifier` instance |
| boolean | `bool` |
| number | `int`, `uint`, `qlonglong`, `qulonglong`, `double`, `float` |
| array | `QList<T>` or `QStringList`, where `T` is `QString`, `int`, `long`, `float`, `double`, `bool` or a `QObject` subclass |
| object | `QObject` subclass or gadget |

All JSON types can be deserialized to the corresponding variant counterpart:

| JSON | lqobjectserializer |
| --- | ----------- |
| string | `QVariant` |
| boolean | `QVariant` |
| number | `QVariant` |
| array | `QVariantList` |
| object | `QVariantMap` or `QVariantHash` |

## `QObject` serialization to JSON

`QObject`'s can store more types than JSON, so not everything is supported. This is a schema:

| lqobjectserializer | JSON |
| --- | ----------- |
| `QObject`* | object |
| gadget* | object |
| `bool` | boolean |
| `ushort`, `short`, `ulonglong`, `ulong`, `longlong`, `long`, `int`, `uint` | number
| `float`, `double` | number |
| `QString` | string |
| `QVariantMap` | object |
| `QVariantHash` | object |
| `QVariantList` | array |

If the type is then convertible to `QVariantList`, `QVariantHash`, `QVariantMap` or `QString` is converted and serialized according to that conversion.

## JSON serialization/deserialization to/from Q_GADGET
It is possible to serialize/deserialize to a gadget class. This is an example:

```c++
L_BEGIN_GADGET(ImageData)
L_RW_GPROP(QString, jpg, setJpg)
L_RW_GPROP(int, contrast, setContrast)
L_RW_GPROP(int, brightness, setBrightness)
L_END_GADGET
Q_DECLARE_METATYPE(QSharedPointer<ImageData>)

[...]

lqo::Deserializer<ImageData> des;
QSharedPointer<ImageData> data(des.deserialize(path));
```

## Serializing custom types to string

It is also possible to serialize/deserialize custom types to/from string. To do this, you'll have to create a serialization class by inheriting `lqo::Stringifier` and overriding the two methods. Example:

```c++
class RectStringifier : public Stringifier
{
public:
    QString stringify(const QVariant& v) override {
        if (v.isNull() || !v.canConvert<QRectF>())
            return QString();
        return lqt::string_from_rect(v.value<QRectF>());
    }

    QVariant destringify(const QString& s) override {
        QRectF ret = lqt::string_to_rect(s);
        return QVariant::fromValue(ret);
    }
};
```
Stringifiers must be passed to the `lqo::Serializer` instance.
## How to inlcude in your project

Everything is included in just two files, so you can include those alone. Otherwise, you can include through cmake:

```cmake
include(${CMAKE_CURRENT_SOURCE_DIR}/<path_to_lqobjectserializer>/LQObjectSerializer/CMakeLists.txt)
target_link_libraries(yourproj lqobjectserializer)
```

## Memory management considerations

QObject's are instantiated as needed during deserialization. Each QObject child is created with the proper parent, which means you can always ignore deallocation of children. The root object instead is returned and is handed to you.

Qt gadgets do not have a parent, and you should take care of deallocating manually. You can dealloc in destructors, for example.

## What is missing?
* Most types are supported, but something is still missing.
* No support for nested arrays.