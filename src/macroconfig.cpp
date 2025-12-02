#include "macroconfig.h"

#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

MacroConfig::MacroConfig(QObject *parent)
    : QObject(parent)
    , m_columns(4)
    , m_rows(3)
{
    // Default config path
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    m_configPath = configDir + "/macropad/macros.json";
}

QVariantList MacroConfig::macros() const
{
    QVariantList result;
    for (const Macro &macro : m_macros) {
        result.append(macroToVariantMap(macro));
    }
    return result;
}

int MacroConfig::columns() const
{
    return m_columns;
}

int MacroConfig::rows() const
{
    return m_rows;
}

void MacroConfig::setColumns(int columns)
{
    if (m_columns != columns && columns > 0 && columns <= 8) {
        m_columns = columns;
        emit columnsChanged();
    }
}

void MacroConfig::setRows(int rows)
{
    if (m_rows != rows && rows > 0 && rows <= 6) {
        m_rows = rows;
        emit rowsChanged();
    }
}

bool MacroConfig::loadConfig(const QString &filePath)
{
    QString path = filePath.isEmpty() ? m_configPath : filePath;
    
    QFile file(path);
    if (!file.exists()) {
        qDebug() << "Config file not found, creating defaults";
        createDefaultMacros();
        saveConfig(path);
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        emit error("Failed to open config file: " + file.errorString());
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit error("Failed to parse config file: " + parseError.errorString());
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Load grid settings
    if (root.contains("columns")) {
        m_columns = root["columns"].toInt(4);
    }
    if (root.contains("rows")) {
        m_rows = root["rows"].toInt(3);
    }
    
    // Load macros
    m_macros.clear();
    QJsonArray macrosArray = root["macros"].toArray();
    
    for (const QJsonValue &value : macrosArray) {
        QJsonObject obj = value.toObject();
        Macro macro;
        macro.id = obj["id"].toString();
        macro.name = obj["name"].toString();
        macro.icon = obj["icon"].toString();
        macro.color = obj["color"].toString();
        
        QJsonArray seqArray = obj["sequence"].toArray();
        for (const QJsonValue &seqVal : seqArray) {
            macro.sequence.append(seqVal.toObject().toVariantMap());
        }
        
        m_macros.append(macro);
    }
    
    emit macrosChanged();
    emit columnsChanged();
    emit rowsChanged();
    emit configLoaded();
    
    return true;
}

bool MacroConfig::saveConfig(const QString &filePath)
{
    QString path = filePath.isEmpty() ? m_configPath : filePath;
    
    // Ensure directory exists
    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QJsonObject root;
    root["columns"] = m_columns;
    root["rows"] = m_rows;
    
    QJsonArray macrosArray;
    for (const Macro &macro : m_macros) {
        QJsonObject obj;
        obj["id"] = macro.id;
        obj["name"] = macro.name;
        obj["icon"] = macro.icon;
        obj["color"] = macro.color;
        
        QJsonArray seqArray;
        for (const QVariant &step : macro.sequence) {
            seqArray.append(QJsonObject::fromVariantMap(step.toMap()));
        }
        obj["sequence"] = seqArray;
        
        macrosArray.append(obj);
    }
    
    root["macros"] = macrosArray;
    
    QJsonDocument doc(root);
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error("Failed to save config file: " + file.errorString());
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    emit configSaved();
    return true;
}

QVariantMap MacroConfig::getMacro(const QString &id) const
{
    for (const Macro &macro : m_macros) {
        if (macro.id == id) {
            return macroToVariantMap(macro);
        }
    }
    return QVariantMap();
}

QVariantList MacroConfig::getMacroSequence(const QString &id) const
{
    for (const Macro &macro : m_macros) {
        if (macro.id == id) {
            return macro.sequence;
        }
    }
    return QVariantList();
}

void MacroConfig::addMacro(const QVariantMap &macroMap)
{
    Macro macro = variantMapToMacro(macroMap);
    
    // Generate ID if not provided
    if (macro.id.isEmpty()) {
        macro.id = "macro_" + QString::number(m_macros.size() + 1);
    }
    
    m_macros.append(macro);
    emit macrosChanged();
}

void MacroConfig::updateMacro(const QString &id, const QVariantMap &macroMap)
{
    for (int i = 0; i < m_macros.size(); ++i) {
        if (m_macros[i].id == id) {
            m_macros[i] = variantMapToMacro(macroMap);
            m_macros[i].id = id;  // Preserve the ID
            emit macrosChanged();
            return;
        }
    }
}

void MacroConfig::removeMacro(const QString &id)
{
    for (int i = 0; i < m_macros.size(); ++i) {
        if (m_macros[i].id == id) {
            m_macros.removeAt(i);
            emit macrosChanged();
            return;
        }
    }
}

void MacroConfig::resetToDefaults()
{
    createDefaultMacros();
    emit macrosChanged();
}

QVariantMap MacroConfig::createKeyAction(int keyCode, int modifiers)
{
    QVariantMap action;
    action["type"] = "key";
    action["keyCode"] = keyCode;
    action["modifiers"] = modifiers;
    return action;
}

QVariantMap MacroConfig::createTextAction(const QString &text)
{
    QVariantMap action;
    action["type"] = "text";
    action["text"] = text;
    return action;
}

QVariantMap MacroConfig::createDelayAction(int milliseconds)
{
    QVariantMap action;
    action["type"] = "delay";
    action["ms"] = milliseconds;
    return action;
}

QVariantMap MacroConfig::createComboAction(const QVariantList &keys, int modifiers)
{
    QVariantMap action;
    action["type"] = "combo";
    action["keys"] = keys;
    action["modifiers"] = modifiers;
    return action;
}

void MacroConfig::createDefaultMacros()
{
    m_macros.clear();
    m_columns = 4;
    m_rows = 3;
    
    // Default macro buttons
    
    // Copy (Ctrl+C)
    Macro copy;
    copy.id = "copy";
    copy.name = "Copy";
    copy.icon = "📋";
    copy.color = "#4CAF50";
    copy.sequence.append(createKeyAction(0x06, 0x01));  // C with Ctrl
    m_macros.append(copy);
    
    // Paste (Ctrl+V)
    Macro paste;
    paste.id = "paste";
    paste.name = "Paste";
    paste.icon = "📄";
    paste.color = "#2196F3";
    paste.sequence.append(createKeyAction(0x19, 0x01));  // V with Ctrl
    m_macros.append(paste);
    
    // Cut (Ctrl+X)
    Macro cut;
    cut.id = "cut";
    cut.name = "Cut";
    cut.icon = "✂️";
    cut.color = "#FF9800";
    cut.sequence.append(createKeyAction(0x1B, 0x01));  // X with Ctrl
    m_macros.append(cut);
    
    // Undo (Ctrl+Z)
    Macro undo;
    undo.id = "undo";
    undo.name = "Undo";
    undo.icon = "↩️";
    undo.color = "#9C27B0";
    undo.sequence.append(createKeyAction(0x1D, 0x01));  // Z with Ctrl
    m_macros.append(undo);
    
    // Redo (Ctrl+Y)
    Macro redo;
    redo.id = "redo";
    redo.name = "Redo";
    redo.icon = "↪️";
    redo.color = "#E91E63";
    redo.sequence.append(createKeyAction(0x1C, 0x01));  // Y with Ctrl
    m_macros.append(redo);
    
    // Save (Ctrl+S)
    Macro save;
    save.id = "save";
    save.name = "Save";
    save.icon = "💾";
    save.color = "#00BCD4";
    save.sequence.append(createKeyAction(0x16, 0x01));  // S with Ctrl
    m_macros.append(save);
    
    // Select All (Ctrl+A)
    Macro selectAll;
    selectAll.id = "select_all";
    selectAll.name = "Select All";
    selectAll.icon = "🔲";
    selectAll.color = "#607D8B";
    selectAll.sequence.append(createKeyAction(0x04, 0x01));  // A with Ctrl
    m_macros.append(selectAll);
    
    // Find (Ctrl+F)
    Macro find;
    find.id = "find";
    find.name = "Find";
    find.icon = "🔍";
    find.color = "#795548";
    find.sequence.append(createKeyAction(0x09, 0x01));  // F with Ctrl
    m_macros.append(find);
    
    // New Tab (Ctrl+T)
    Macro newTab;
    newTab.id = "new_tab";
    newTab.name = "New Tab";
    newTab.icon = "➕";
    newTab.color = "#3F51B5";
    newTab.sequence.append(createKeyAction(0x17, 0x01));  // T with Ctrl
    m_macros.append(newTab);
    
    // Close Tab (Ctrl+W)
    Macro closeTab;
    closeTab.id = "close_tab";
    closeTab.name = "Close Tab";
    closeTab.icon = "❌";
    closeTab.color = "#F44336";
    closeTab.sequence.append(createKeyAction(0x1A, 0x01));  // W with Ctrl
    m_macros.append(closeTab);
    
    // Mute
    Macro mute;
    mute.id = "mute";
    mute.name = "Mute";
    mute.icon = "🔇";
    mute.color = "#9E9E9E";
    mute.sequence.append(createKeyAction(0x7F, 0x00));  // Volume Mute
    m_macros.append(mute);
    
    // Play/Pause (Media key)
    Macro playPause;
    playPause.id = "play_pause";
    playPause.name = "Play/Pause";
    playPause.icon = "⏯️";
    playPause.color = "#8BC34A";
    playPause.sequence.append(createKeyAction(0xCD, 0x00));  // Media Play/Pause
    m_macros.append(playPause);
}

QVariantMap MacroConfig::macroToVariantMap(const Macro &macro) const
{
    QVariantMap map;
    map["id"] = macro.id;
    map["name"] = macro.name;
    map["icon"] = macro.icon;
    map["color"] = macro.color;
    map["sequence"] = macro.sequence;
    return map;
}

MacroConfig::Macro MacroConfig::variantMapToMacro(const QVariantMap &map) const
{
    Macro macro;
    macro.id = map.value("id").toString();
    macro.name = map.value("name").toString();
    macro.icon = map.value("icon").toString();
    macro.color = map.value("color", "#666666").toString();
    macro.sequence = map.value("sequence").toList();
    return macro;
}
