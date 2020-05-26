# LQObjectSerializer
LQObjectSerializer is a small library that I use to serialize and deserialize data automatically in the form of QObject's. This is a bit similar to what gson does for Java and Kotlin, althought much simpler.
## JSON deserialization to QObject
Given is this json string:
```
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
The library can deserialize it to these objects
```
class Item : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, id, setId, QString())
    L_RW_PROP(QString, label, setLabel, QString())
public:
    Q_INVOKABLE Item(QObject* parent = nullptr) : QObject(parent) {}
};

class Menu : public QObject
{
    Q_OBJECT
    L_RW_PROP(QString, header, setHeader)
    L_RW_PROP(QList<Item*>, items, setItems)
public:
    Q_INVOKABLE Menu(QObject* parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE void add_items(QObject* obj) { m_items.append(static_cast<Item*>(obj)); }
};

class MenuRoot : public QObject
{
    Q_OBJECT
    L_RW_PROP(Menu*, menu, setMenu, nullptr)
public:
    Q_INVOKABLE MenuRoot(QObject* parent = nullptr) : QObject(parent) {}
};
```
with a call to:
```
LDeserializer<MenuRoot> deserializer(factory);
QScopedPointer<MenuRoot> g(deserializer.deserialize(jsonString));
```
where factory is a hashmap mapping object types to meta objects:
```
QHash<QString, QMetaObject> factory {
    { QSL("Item*"), Item::staticMetaObject },
    { QSL("Menu*"), Menu::staticMetaObject }
};
```
The L_RW_PROP is a macro defined in the lqtutils submodule.
## QObject serialization to JSON
```
QScopedPointer mr(new MenuRoot);
[...]
LSerializer serializer;
QJsonObject obj = serializer.serialize(mr.data());
```
## What is missing?
Most of the library is in place. Bugs are more than likely though. Most types are supported, but some are still missing. Also, you won't be able to deserialize nested arrays.