#ifndef WRITERTHREAD_H
#define WRITERTHREAD_H

#include <QThread>
#include <QStringList>

class WriterThread : public QThread
{
    Q_OBJECT
public:
    explicit WriterThread(QObject *parent = 0);
    ~WriterThread();

    void setOrigin(const QString &path);
    void setDestinations(const QStringList &paths);
    void cancel();

    virtual void run();

Q_SIGNALS:
    void updateProgress(int value);
    void finishedJob(int elapsed);

private:
    QString m_path;
    QStringList m_destinations;
    int m_bufferSize = 8192;
    bool m_cancel = false;
};

#endif // WRITERTHREAD_H
