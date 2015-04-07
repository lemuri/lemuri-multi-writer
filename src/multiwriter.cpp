#include "multiwriter.h"
#include "ui_multiwriter.h"

#include "writerthread.h"

//#include <libudev.h>

#include <QCoreApplication>

#include <QTime>
#include <QFile>
#include <QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QStandardItemModel>
#include <QDebug>

MultiWriter::MultiWriter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiWriter)
{
    ui->setupUi(this);

    m_writer = new WriterThread;
    connect(m_writer, &WriterThread::updateProgress,
            ui->progresPB, &QProgressBar::setValue);
    connect(m_writer, &WriterThread::finishedJob,
            this, &MultiWriter::finishedJob);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(writeImage()));

//    udev *udev = udev_new();

//    udev_enumerate *enumerate = udev_enumerate_new(udev);
//    udev_enumerate_add_match_subsystem(enumerate, "block");
//    udev_enumerate_scan_devices(enumerate);
//    udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);

//    QStandardItemModel *deviceModel = new QStandardItemModel(this);
//    QStandardItemModel *deviceModel2 = new QStandardItemModel(this);

//    ui->harddiskForImageCB->setModel(deviceModel2);
//    ui->destinationView->setModel(deviceModel);

//    udev_list_entry *entry;
//    udev_list_entry_foreach(entry, devices) {
//        const char *path;

//        /* Get the filename of the /sys entry for the device
//               and create a udev_device object (dev) representing it */
//        path = udev_list_entry_get_name(entry);
//        udev_device *dev = udev_device_new_from_syspath(udev, path);
//        if (!dev) {
//            continue;
//        }

//        if (strcmp(udev_device_get_devtype(dev), "disk") == 0) {
//            QString deviceName = udev_device_get_sysname(dev);
//            const char *cdrom = udev_device_get_property_value(dev, "ID_CDROM");
//            const char *model = udev_device_get_property_value(dev, "ID_MODEL");
//            if (cdrom && strcmp(cdrom, "1") == 0) {
//                continue; // skip cdrom
//            }

//            qDebug() << deviceName << model << path;
//            if (model) {
//                QString path = QString::fromLatin1(udev_device_get_devnode(dev));

//                QStandardItem *item1 = new QStandardItem(QString("%1 (%2)").arg(model, deviceName));
//                item1->setData(path, DevPathRole);
//                deviceModel2->appendRow(item1);

//                QStandardItem *item2 = new QStandardItem(QString("%1 (%2)").arg(model, deviceName));
//                item2->setData(path, DevPathRole);
//                deviceModel->appendRow(item2);
//            }
//        }

//        udev_device_unref(dev);
//    }

//    /* Free the enumerator object */
//    udev_enumerate_unref(enumerate);

//    udev_unref(udev);
}

MultiWriter::~MultiWriter()
{
    delete ui;
}

void MultiWriter::writeImage()
{
    qDebug() << Q_FUNC_INFO << m_writer->isRunning();

    if (m_writer->isRunning()) {
        m_writer->cancel();
        return;
    }

    QString origin;
    if (ui->harddiskForImageCB->isEnabled()) {
        origin = ui->harddiskForImageCB->currentData(DevPathRole).toString();
    } else {
        origin = ui->imageLE->text();
    }

    QStringList destinations;
    Q_FOREACH (const QModelIndex &index, ui->destinationView->selectionModel()->selectedIndexes()) {
        destinations.append(index.data(DevPathRole).toString());
    }

    if (destinations.contains(origin)) {
        qDebug() << "origin must not be in destinations";
        return;
    }

    m_writer->setOrigin(origin);
    m_writer->setDestinations(destinations);
    if (origin.isEmpty() || destinations.isEmpty()) {
        qDebug() << "need to select an origin and a destination";
        return;
    }

    m_writer->start();
}

void MultiWriter::on_browseTB_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isNull()) {
        ui->imageLE->setText(fileName);
    }
}

void MultiWriter::finishedJob(int elapsed)
{
    QTime time = QTime::fromMSecsSinceStartOfDay(elapsed);
    QMessageBox::information(this, tr("Job finished"), tr("Successfuly wrote the image, took: %1").arg(time.toString()));
    qDebug() << elapsed << time.toString();
}
