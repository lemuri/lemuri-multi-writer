#ifndef DEVICESMODEL_H
#define DEVICESMODEL_H

#include <QStandardItemModel>
#include <UDisks2Qt5/UDisksObject>

class UDisksClient;
class DevicesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum {
        RoleId = Qt::UserRole,
        RoleName,
        RoleSize,
        RoleDevPath
    };
    explicit DevicesModel(QObject *parent = 0);

    virtual QHash<int,QByteArray> roleNames() const;

public slots:
    void objectsAvailable(bool available = true);
    void objectAdded(const UDisksObject::Ptr &object);
    void objectRemoved(const UDisksObject::Ptr &object);
    QString getDeviceName(int row);
    UDisksDrive* getDrive(int row);

private:
    QHash<int,QByteArray> m_roleNames;
    UDisksClient *m_client;
};

#endif // DEVICESMODEL_H
