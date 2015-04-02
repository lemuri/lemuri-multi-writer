#include "writerthread.h"

#include <QFile>
#include <QTime>
#include <QCryptographicHash>
#include <QDebug>

WriterThread::WriterThread(QObject *parent) : QThread(parent)
{

}

WriterThread::~WriterThread()
{

}

void WriterThread::setOrigin(const QString &path)
{
    m_path = path;
}

void WriterThread::setDestinations(const QStringList &paths)
{
    m_destinations = paths;
}

void WriterThread::cancel()
{
    m_cancel = true;
}

void WriterThread::run()
{
    qDebug() << "Thread running..." << m_path << m_destinations;

    QFile image(m_path);
    if (!image.open(QFile::ReadOnly)) {
        qDebug() << "Failed to open orign:" << m_path;
        return;
    }

    QCryptographicHash imageHash(QCryptographicHash::Sha256);

    bool openFailed = false;
    QList<QFile *> destinations;
    Q_FOREACH (const QString &path, m_destinations) {
        QFile *destination = new QFile(path);
        destinations.append(destination);
        if (!destination->open(QFile::WriteOnly)) {
            qDebug() << "Failed to open destination:" << path;
            openFailed = true;
            break;
        }
    }

    // Clear open QFiles
    if (openFailed) {
        qDeleteAll(destinations);
        return;
    }

    QTime time;
    time.start();

    qint64 imageSize = image.size();
    int currentProgress = 0;
    int bufferSize = m_bufferSize;
    char *buffer = new char[bufferSize];
    qDebug() << image.atEnd() << image.size();
    while (!image.atEnd() || destinations.isEmpty()) {
        int readSize = bufferSize;
        if (image.pos() + bufferSize > imageSize) {
            readSize = imageSize - image.pos();
        }

        qint64 ret = image.read(buffer, readSize);
        if (ret != readSize) {
            qWarning() << "Failed to read:" << ret << readSize;
            break;
        }
        imageHash.addData(buffer, ret);

        int i = 0;
        while (i < destinations.size()) {
            if (m_cancel) {
                qDebug() << "canceling...";
                break;
            }

            QFile *dest = destinations.at(i);
            ret = dest->write(buffer, readSize);
            if (ret != readSize) {
                qWarning() << "Failed to write to:" << dest->fileName();
                delete dest;
                destinations.removeAt(i);
                continue;
            }
            ++i;
        }

        int newProgress = (double(image.pos()) / imageSize) * 100;
        if (newProgress != currentProgress) {
            currentProgress = newProgress;
            Q_EMIT updateProgress(currentProgress);
            qDebug() << "progress" << newProgress;
        }

        if (m_cancel) {
            qDebug() << "canceling...";
            break;
        }
    }
    delete [] buffer;
    qDeleteAll(destinations);

    Q_EMIT finishedJob(time.elapsed());

    qDebug() << "finished" << imageHash.result().toHex();
}
