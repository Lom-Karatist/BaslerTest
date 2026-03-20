#ifndef SAVINGMODULE_H
#define SAVINGMODULE_H


#include <QObject>
#include <QString>
#include <QByteArray>
#include <QImage>
#include "Types.h"

class SavingModule : public QObject
{
    Q_OBJECT
public:
    explicit SavingModule(QObject *parent = nullptr);

    void saveData(const QByteArray &data, int width, int height, int pixelFormat, QString appendix, QString timeStamp);
    void setSavingPath(const QString &newSavingPath);
    void setFormat(BaslerConstants::SavingFormat newFormat);

    bool isNeedToSave() const;
    void setIsNeedToSave(bool newIsNeedToSave);
    static QImage convertToQImage(const QByteArray &data, int width, int height, int pixelFormat);

signals:

private:
    void saveAsBmp(const QByteArray &data, int width, int height, int pixelFormat, const QString &prefix, QString timeStamp);
    void saveAsBinary(const QByteArray &data, const QString &prefix, QString timeStamp);
    QString generateFileName(const QString &prefix, QString timeStamp) const;

    bool m_isNeedToSave;
    QString m_savingPath;
    BaslerConstants::SavingFormat m_format;
};

#endif // SAVINGMODULE_H
