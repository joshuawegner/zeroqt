#ifndef MACROCONFIG_H
#define MACROCONFIG_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

/**
 * @brief MacroConfig - Manages macro button configurations
 * 
 * This class handles loading, saving, and managing macro configurations.
 * Macros are stored as JSON and can be customized by the user.
 */
class MacroConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList macros READ macros NOTIFY macrosChanged)
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)

public:
    explicit MacroConfig(QObject *parent = nullptr);

    /**
     * @brief Structure representing a single macro
     */
    struct Macro {
        QString id;
        QString name;
        QString icon;
        QString color;
        QVariantList sequence;  // List of actions
    };

    QVariantList macros() const;
    int columns() const;
    int rows() const;

    void setColumns(int columns);
    void setRows(int rows);

public slots:
    /**
     * @brief Load macros from the configuration file
     */
    bool loadConfig(const QString &filePath = QString());

    /**
     * @brief Save macros to the configuration file
     */
    bool saveConfig(const QString &filePath = QString());

    /**
     * @brief Get a macro by ID
     */
    QVariantMap getMacro(const QString &id) const;

    /**
     * @brief Get macro sequence by ID
     */
    QVariantList getMacroSequence(const QString &id) const;

    /**
     * @brief Add a new macro
     */
    void addMacro(const QVariantMap &macro);

    /**
     * @brief Update an existing macro
     */
    void updateMacro(const QString &id, const QVariantMap &macro);

    /**
     * @brief Remove a macro
     */
    void removeMacro(const QString &id);

    /**
     * @brief Reset to default configuration
     */
    void resetToDefaults();

    /**
     * @brief Create a simple key press action
     */
    static QVariantMap createKeyAction(int keyCode, int modifiers = 0);

    /**
     * @brief Create a text typing action
     */
    static QVariantMap createTextAction(const QString &text);

    /**
     * @brief Create a delay action
     */
    static QVariantMap createDelayAction(int milliseconds);

    /**
     * @brief Create a key combination action
     */
    static QVariantMap createComboAction(const QVariantList &keys, int modifiers);

signals:
    void macrosChanged();
    void columnsChanged();
    void rowsChanged();
    void configLoaded();
    void configSaved();
    void error(const QString &message);

private:
    void createDefaultMacros();
    QVariantMap macroToVariantMap(const Macro &macro) const;
    Macro variantMapToMacro(const QVariantMap &map) const;

    QList<Macro> m_macros;
    int m_columns;
    int m_rows;
    QString m_configPath;
};

#endif // MACROCONFIG_H
