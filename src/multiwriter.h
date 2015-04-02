#ifndef MULTIWRITER_H
#define MULTIWRITER_H

#include <QDialog>

namespace Ui {
class MultiWriter;
}

class WriterThread;
class MultiWriter : public QDialog
{
    Q_OBJECT
public:
    enum {
        DevPathRole = Qt::UserRole
    };
    explicit MultiWriter(QWidget *parent = 0);
    ~MultiWriter();

public Q_SLOTS:
    void writeImage();
    void on_browseTB_clicked();
    void finishedJob(int elapsed);

private:
    WriterThread *m_writer;
    Ui::MultiWriter *ui;
};

#endif // MULTIWRITER_H
