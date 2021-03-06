#include "writerthread.h"

#include <QFile>
#include <QTime>
#include <QCryptographicHash>
#include <QSemaphore>
#include <QDebug>

#include <unistd.h>

class ReaderThread : public QThread
{
public:
    ReaderThread(QFile *file_image, int buffer_size, bool *cancel)
        : freeChunks(chunkCount)
        , imageHash(QCryptographicHash::Sha256)
        , m_cancel(cancel)
    {
        bufferSize = buffer_size * chunkCount;
        chunkSize = buffer_size;
        chunks = file_image->size() / chunkSize;
        if (chunks % file_image->size()) {
            // TODO find out if there is a more
            // correct way to round up
            ++chunks;
        }
        image = file_image;

        buffer = new char[bufferSize];
        imageSize = file_image->size();
    }

    ~ReaderThread()
    {
        delete [] buffer;
    }

    void run() Q_DECL_OVERRIDE
    {
        for (qint64 i = 0; i < chunks; ++i) {
            if (*m_cancel) {
                return;
            }

            freeChunks.acquire();
            char *ptr = buffer + ((i % chunkCount) * chunkSize);
            int ret = image->read(ptr, chunkSize);
            imageHash.addData(buffer + ((i % chunkCount) * chunkSize), ret);
            usedChunks.release();
        }
    }

    char *buffer;
    int bufferSize;
    int chunkSize;
    int chunks;
    const int chunkCount = 100;
    qint64 imageSize;
    QCryptographicHash imageHash;

    QFile *image;

    QSemaphore freeChunks;
    QSemaphore usedChunks;

private:
    bool *m_cancel;
};

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

    ReaderThread *reader = new ReaderThread(&image, m_bufferSize, &m_cancel);

    QTime time;
    time.start();

    reader->start();

    qint64 imageSize = image.size();
    qint64 writePos = 0;
    int currentProgress = 0;

    for (qint64 i = 0; i < reader->chunks; ++i) {
        if (m_cancel) {
            qDebug() << "canceling...";
            break;
        }

        reader->usedChunks.acquire();

        // Do not pass the end of the buffer
        int readSize = reader->chunkSize;
        if (writePos + readSize > imageSize) {
            readSize = imageSize - writePos;
        }

        // Write chunk
        int j = 0;
        while (j < destinations.size()) {
            if (m_cancel) {
                qDebug() << "canceling...";
                break;
            }

            QFile *dest = destinations.at(j);
            int ret = dest->write(reader->buffer + ((i % reader->chunkCount) * reader->chunkSize), readSize);
            if (ret != readSize) {
                qWarning() << "Failed to write to:" << dest->fileName();
                delete dest;
                destinations.removeAt(j);
                continue;
            }

            ++j;
        }
        writePos += readSize;

        int newProgress = (double(writePos) / imageSize) * 100;
        if (newProgress != currentProgress) {
            currentProgress = newProgress;
            Q_EMIT updateProgress(currentProgress);
            qDebug() << "progress" << newProgress << reader->usedChunks.available() << reader->freeChunks.available();
        }

        reader->freeChunks.release();
    }

    // sync
    int i = 0;
    while (i < destinations.size()) {
        if (m_cancel) {
            qDebug() << "canceling...";
            break;
        }

        QFile *dest = destinations.at(i);
        if (!dest->flush()) {
            qWarning() << "Failed to flush to:" << dest->fileName();
        }
        ++i;
    }


    if (destinations.isEmpty()) {
        cancel();
    }

    QByteArray hashResult = reader->imageHash.result();

    reader->wait();
    delete reader;

    qDeleteAll(destinations);

    sync();

    Q_EMIT finishedJob(time.elapsed());

    qDebug() << "finished" << hashResult.toHex();
}
