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
    vpWidth = vpHeight = 256;
    pixmap = new QPixmap(vpWidth, vpHeight);
    painter = new QPainter(pixmap);
    ui->scrollArea->setWidget(imageLabel);
}

Bittle::~Bittle()
{
    delete ui;
}

void Bittle::on_update()
{
    int maxStrips = imageFile->size() / (stride * vpHeight);
    int vpStrips = vpWidth / (stride * 8 * vpHeight);
    ui->offsetHandleBar->setMinimum(0);
    ui->offsetHandleBar->setMaximum(maxStrips - vpStrips);
    if (imageData == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageData is null."));
        return;
    }

    QImage::Format format = QImage::Format_Mono;
    if (lsbFirst)
        format = QImage::Format_MonoLSB;

    QImage image = QImage(imageData + offset, stride, vpHeight, stride / 8, format);
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
    stride = w;
    on_update();
}

void Bittle::on_height_changed(int h)
{
    vpHeight = h;
    on_update();
}

void Bittle::on_offset_changed(int o)
{
    offset = o * (vpHeight * stride / 8);
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

        stride = 256;
        vpHeight = 400;
        offset = 0;
        ui->offsetHandleBar->setValue(0);
        on_update();

    }
}
