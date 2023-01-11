#ifndef SETTINGSBASE_HPP
#define SETTINGSBASE_HPP

#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTextStream>

namespace Config
{
    template <class Map>
    class SettingsBase
    {

    public:
        SettingsBase() { mMultiValue = false; }
        ~SettingsBase() = default;

        inline QString value(const QString& key, const QString& defaultValue = QString()) const
        {
            return mSettings.value(key).isEmpty() ? defaultValue : mSettings.value(key);
        }

        inline void setValue(const QString& key, const QString& value) { mSettings.replace(key, value); }

        inline void setMultiValue(const QString& key, const QString& value)
        {
            QStringList values = mSettings.values(key);
            if (!values.contains(value))
                mSettings.insert(key, value);
        }

        inline void setMultiValueEnabled(bool enable) { mMultiValue = enable; }

        inline void remove(const QString& key) { mSettings.remove(key); }

        Map getSettings() const { return mSettings; }

        bool readFile(QTextStream& stream)
        {
            Map cache;

            QString sectionPrefix;

            QRegularExpression sectionRe(QRegularExpression::anchoredPattern("^\\[([^]]+)\\]"));
            QRegularExpression keyRe("^([^=]+)\\s*=\\s*(.+)$");

            while (!stream.atEnd())
            {
                QString line = stream.readLine();

                if (line.isEmpty() || line.startsWith("#"))
                    continue;

                QRegularExpressionMatch sectionMatch = sectionRe.match(line);
                if (sectionMatch.hasMatch())
                {
                    sectionPrefix = sectionMatch.captured(1);
                    sectionPrefix.append("/");
                    continue;
                }

                QRegularExpressionMatch match = keyRe.match(line);
                if (match.hasMatch())
                {
                    QString key = match.captured(1).trimmed();
                    QString value = match.captured(2).trimmed();

                    if (!sectionPrefix.isEmpty())
                        key.prepend(sectionPrefix);

                    mSettings.remove(key);

                    QStringList values = cache.values(key);

                    if (!values.contains(value))
                    {
                        if (mMultiValue)
                        {
                            cache.insert(key, value);
                        }
                        else
                        {
                            cache.remove(key);
                            cache.insert(key, value);
                        }
                    }
                }
            }

            if (mSettings.isEmpty())
            {
                mSettings = cache; // This is the first time we read a file
                return true;
            }

            // Merge the changed keys with those which didn't
            mSettings.unite(cache);
            return true;
        }

        void clear() { mSettings.clear(); }

    private:
        Map mSettings;

        bool mMultiValue;
    };
}
#endif // SETTINGSBASE_HPP
