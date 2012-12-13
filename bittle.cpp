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
    ui->spinImageHeight->setMaximum(MAXHEIGHT);
    ui->spinImageStride->setMaximum(MAXWIDTH / 8);
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(false);
    vpWidth = MAXWIDTH;
    vpHeight = MAXHEIGHT;
    firstStripe = 0;
    stride = 1;
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
    if (imageFile == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageFile is null."));
        return;
    }

    uint maxStrips = imageFile->size() / (stride * vpHeight);
    uint vpStrips = vpWidth / (stride * 8 * vpHeight);

    ui->offsetHandleBar->setMinimum(0);
    ui->offsetHandleBar->setMaximum(maxStrips - vpStrips);

    if (imageData == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageData is null."));
        return;
    }

    if (imageLabel == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageLabel object is null."));
        return;
    }

    uint stripeSize = stride * vpHeight;
    uint offset = firstStripe * stripeSize;

    if (offset >= imageFile->size()) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("offset beyond file end"));
        return;
    }

    QImage::Format format = lsbFirst ? QImage::Format_MonoLSB : QImage::Format_Mono;

    painter->eraseRect(0, 0, vpWidth, vpHeight);
    uchar *p = imageData + firstStripe * stripeSize;
    uchar *maxp = imageData + imageFile->size();
    uint xoff = 0;
    while (p < maxp && xoff <= (vpWidth - stride * 8) ) {
        uint bytes_avail = maxp - p;
        uint lines = vpHeight;
        if (bytes_avail < vpHeight)
            lines = bytes_avail;
        QImage image = QImage(p, stride * 8, lines, stride, format);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Bittle"),
                                     tr("Error creating image object."));
            return;
        }
        painter->drawImage(xoff, 0, image);
        p += stride * lines;
        xoff += stride * 8;
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
    uint stripeOffset = firstStripe * stride * vpHeight;
    painter->eraseRect(0, 0, vpWidth, vpHeight);
    vpHeight = h;
    firstStripe = stripeOffset / ( stride * vpHeight );
    on_update();
}

void Bittle::on_offset_changed(int o)
{
    firstStripe = o;
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

        stride = 8;
        vpHeight = MAXHEIGHT;
        firstStripe = 0;
        ui->offsetHandleBar->setValue(0);
        on_update();

    }
}
