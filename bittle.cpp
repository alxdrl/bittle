#include "bittle.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>

Bittle::Bittle(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Bittle)
{
    ui->setupUi(this);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(false);
    pixmap = new QPixmap(512, 512);
    painter = new QPainter(pixmap);
    ui->scrollArea->setWidget(imageLabel);
}

Bittle::~Bittle()
{
    delete ui;
}

void Bittle::on_update()
{
    int viewsize = (height * width / 8);
    ui->offsetHandleBar->setMinimum(0);
    ui->offsetHandleBar->setMaximum((dataSize / viewsize) - 1);
    if (imageData == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageData is null."));
        return;
    }

    QImage::Format format = QImage::Format_Mono;
    if (lsbFirst)
        format = QImage::Format_MonoLSB;


    QImage image = QImage(imageData + offset, width, height, width / 8, format);
    if (image.isNull()) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("Error creating image object."));
        return;
    }

    painter->eraseRect(0,0, 512, 512);
    painter->drawImage(0, 0, image);
    if (imageLabel == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageLabel object is null."));
        return;
    }
    imageLabel->setPixmap(*pixmap);
}

void Bittle::on_lsb_changed(int state)
{
    lsbFirst = (state == Qt::Checked) ? true : false;
    on_update();
}
void Bittle::on_width_changed(int w)
{
    width = w;
    on_update();
}

void Bittle::on_height_changed(int h)
{
    height = h;
    on_update();
}

void Bittle::on_offset_changed(int o)
{
    offset = o * (height * width / 8);
    on_update();
}

void Bittle::on_actionOuvrir_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {

        imageFile = new QFile(fileName);

        if (!imageFile) {
            QMessageBox::information(this, tr("Bittle"),
                                     tr("Error creating QFile object for %1.").arg(fileName));
            return;
        }

        imageFile->open(QIODevice::ReadOnly);

        dataSize = imageFile->size();
        imageData = imageFile->map(0, dataSize);

        if (imageData == NULL) {
            QMessageBox::information(this, tr("Bittle"),
                                     tr("Cannot map file %1.").arg(fileName));
            return;
        }

        width = 256;
        height = 400;
        offset = 0;
        ui->offsetHandleBar->setValue(0);
        on_update();

    }
}
