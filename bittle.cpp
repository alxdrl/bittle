/*	Bittle
    Copyright (C) 2012 Alexis Deruelle <alexis.deruelle@gmail.com>

    This program is free software; you can redistribute it and/or modify

    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

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
    height = MAXHEIGHT;
    blockIndex = 0;
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

    uint blockSize = stride * height;
    uint maxBlocks = imageFile->size() / blockSize;
    uint vpBlocks = vpWidth / blockSize;

    ui->offsetHandleBar->setMinimum(0);
    ui->offsetHandleBar->setMaximum(maxBlocks - vpBlocks);

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

    uint offset = blockIndex * blockSize;

    if (offset >= imageFile->size()) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("offset beyond file end"));
        return;
    }

    QImage::Format format = lsbFirst ? QImage::Format_MonoLSB : QImage::Format_Mono;

    painter->eraseRect(0, 0, vpWidth, vpHeight);
    uchar *p = imageData + offset;
    uchar *maxp = imageData + imageFile->size();
    uint xoff = 0;
    uint yoff = 0;
    while (p < maxp && height <= (vpHeight - yoff)) {
        uint bytes_avail = maxp - p;
        uint lines = height;
        if (bytes_avail < height * stride)
            lines = bytes_avail / stride;
        QImage image = QImage(p, stride * 8, lines, stride, format);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Bittle"),
                                     tr("Error creating image object."));
            return;
        }
        painter->drawImage(xoff, yoff, image);
        p += stride * lines;
        xoff += stride * 8;
        if (xoff >= vpWidth) {
            yoff += height;
            xoff = 0;
        }
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
    uint blockOffset = blockIndex * (stride * height);
    painter->eraseRect(0, 0, vpWidth, vpHeight);
    height = h;
    blockIndex = blockOffset / (stride * height);
    on_update();
}

void Bittle::on_offset_changed(int o)
{
    blockIndex = o;
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
        blockIndex = 0;
        ui->offsetHandleBar->setValue(0);
        on_update();

    }
}
