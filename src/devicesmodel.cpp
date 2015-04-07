#include "devicesmodel.h"

#include <QDebug>
#include <UDisks2Qt5/UDisksClient>
#include <UDisks2Qt5/UDisksFilesystem>
#include <UDisks2Qt5/UDisksBlock>
#include <UDisks2Qt5/UDisksDrive>

DevicesModel::DevicesModel(QObject *parent) :
    QStandardItemModel(parent)
{
    m_client = new UDisksClient(this);
    connect(m_client, SIGNAL(objectsAvailable()),
            this, SLOT(objectsAvailable()));
    connect(m_client, SIGNAL(objectAdded(UDisksObject::Ptr)),
            this, SLOT(objectAdded(UDisksObject::Ptr)));
    connect(m_client, SIGNAL(objectRemoved(UDisksObject::Ptr)),
            this, SLOT(objectRemoved(UDisksObject::Ptr)));
    connect(m_client, SIGNAL(interfacesAdded(UDisksObject::Ptr)),
            this, SLOT(objectChanged(UDisksObject::Ptr)));
    m_client->init();

    m_roleNames.insert(RoleId, "roleId");
    m_roleNames.insert(RoleName, "roleName");
    m_roleNames.insert(RoleSize, "roleSize");
    m_roleNames.insert(RoleDevPath, "roleDevPath");
}

QHash<int, QByteArray> DevicesModel::roleNames() const
{
    return m_roleNames;
}

void DevicesModel::objectsAvailable(bool available)
{
    UDisksClient *client = qobject_cast<UDisksClient *>(sender());
    foreach (const UDisksObject::Ptr &object, client->getObjects(UDisksObject::Drive)) {
        objectAdded(object);
    }
}

void DevicesModel::objectAdded(const UDisksObject::Ptr &object)
{
    UDisksDrive *drive = object->drive();
    qDebug() << Q_FUNC_INFO << object->kind() << object->path().path();
    if (drive) {
        qDebug() << Q_FUNC_INFO << drive->vendor() << drive->model();

        QStandardItem *stdItem = new QStandardItem;
        QString text;
        if (drive->vendor().isEmpty()) {
            text = drive->model();
        } else if (drive->model().isEmpty()) {
            text = drive->vendor();
        } else {
            text = QString("%1 %2").arg(drive->vendor(), drive->model());
        }
        stdItem->setData(object->path().path(), RoleId);
        stdItem->setData(text, RoleName);
        UDisksBlock *block = drive->getBlock();
        if (block) {
            stdItem->setData(block->preferredDevice(), RoleDevPath);
            stdItem->setData(block->size(), RoleSize);
        }
        appendRow(stdItem);
    }
}

void DevicesModel::objectRemoved(const UDisksObject::Ptr &object)
{
    QString objectPath = object->path().path();
    qDebug() << Q_FUNC_INFO << objectPath;
    for (int i = 0; i < rowCount(); ++i) {
        qDebug() << Q_FUNC_INFO << i << item(i)->data(RoleId).toString() << objectPath;
        if (item(i)->data(RoleId).toString() == objectPath) {
            qDebug() << Q_FUNC_INFO << i << objectPath;
            removeRow(i);
            break;
        }
    }
}

QString DevicesModel::getDeviceName(int row)
{
    QStandardItem *stdItem = item(row);
    if (stdItem) {
        return stdItem->data(RoleDevPath).toString();
    }
    return QString();
}

UDisksDrive *DevicesModel::getDrive(int row)
{
    QStandardItem *stdItem = item(row);
    UDisksObject::Ptr object = m_client->getObject(stdItem->data().value<QDBusObjectPath>());

    if (object) {
        return object->drive();
    }
    return 0;
}
