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

#define PIX_PER_BYTE 8
#define DEFAULT_STRIDE 1
#define DEFAULT_HEIGHT 128

Bittle::Bittle(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Bittle),
	vpWidth(MAXWIDTH), vpHeight(MAXHEIGHT)
{
    ui->setupUi(this);
    pixmap = new QPixmap(vpWidth, vpHeight);
    painter = new QPainter(pixmap);
	setControlDefaults();
	setDefaults();
}

void Bittle::revcopy(uchar *dst, const uchar *src, size_t n)
{
    size_t i;
    for (i=0; i < n; ++i)
        dst[n-1-i] = src[i];
}

void Bittle::setControlDefaults()
{
    ui->spinImageHeight->setMinimum(1);
    ui->spinImageHeight->setMaximum(vpHeight);
    ui->spinImageHeight->setValue(vpHeight);
    ui->spinImageStride->setMinimum(1);
    ui->spinImageStride->setMaximum(vpWidth / PIX_PER_BYTE);
    ui->spinImageStride->setValue(DEFAULT_STRIDE);
    ui->offsetHandleBar->setMinimum(0);
    ui->offsetHandleBar->setMaximum(0);
    ui->offsetHandleBar->setValue(0);
}

void Bittle::setDefaults()
{
    blockIndex = 0;
    height = DEFAULT_HEIGHT;
    stride = DEFAULT_STRIDE;
	lsbFirst = false;
}

Bittle::~Bittle()
{
	delete painter;
	delete pixmap;
    delete ui;
}

uint Bittle::blockSize()
{
	return stride * height;
}

void Bittle::updateScrollbarLimits()
{
	uint maxStrides = imageFile->size() / stride;
	uint vpStrides = vpWidth * vpHeight / stride;
    uint scrollbarMax = ( maxStrides > vpStrides) ? maxStrides - vpStrides : 0;
    ui->offsetHandleBar->setMinimum(0);
    ui->offsetHandleBar->setMaximum(scrollbarMax);
} 

uint Bittle::lineSize()
{
	return vpWidth / PIX_PER_BYTE;
}

uint Bittle::getOffset()
{
	uint off = blockIndex * stride;
	ui->statusBar->showMessage(tr("Offset : 0x%1").arg(off, 8, 16, QChar('0'))); 
	return off;
}

void Bittle::updateView()
{
	if (imageFile == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageFile is null."));
        return;
    }

    if (imageData == NULL) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("imageData is null."));
        return;
    }

    uint offset = getOffset();

    if (offset >= imageFile->size()) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("offset beyond file end (%1).").arg(offset));
        return;
    }

    QImage::Format format = lsbFirst ? QImage::Format_MonoLSB : QImage::Format_Mono;

    painter->eraseRect(0, 0, vpWidth, vpHeight);
    uchar *p = imageData + offset;
    uchar *maxp = imageData + imageFile->size();
    uint xoff = 0;
    uint yoff = 0;
    uchar buf[MAXWIDTH * MAXHEIGHT];
	if ((size_t)buf % 4 != 0)
            QMessageBox::information(this, tr("Bittle"),
                                     tr("unaligned buffer"));
    while (p < maxp && height <= (vpHeight - yoff)) {
        uint bytes_avail = maxp - p;
        uint lines = height;
        if (bytes_avail < blockSize())
            lines = bytes_avail / stride;
        memcpy(buf, p, stride * PIX_PER_BYTE * lines);
        QImage image = QImage(buf, stride * PIX_PER_BYTE, lines, stride, format);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Bittle"),
                                     tr("Error creating image object."));
            return;
        }
        painter->drawImage(xoff, yoff, image);
        p += stride * lines;
        xoff += stride * PIX_PER_BYTE;
        if (xoff >= vpWidth) {
            yoff = yoff + height;
            xoff = 0;
        }
    }

    ui->imageLabel->setPixmap(*pixmap);
}

void Bittle::on_lsb_changed(int state)
{
    lsbFirst = (state == Qt::Checked) ? true : false;
    updateView();
}

void Bittle::on_width_changed(int w)
{
	if (w <= 0) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("illegal stride value (%1).").arg(w));
        return;
    }
    stride = w;
	updateScrollbarLimits();
    updateView();
}

void Bittle::on_height_changed(int h)
{
	if (h <= 0) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("illegal height value (%1).").arg(h));
        return;
    }
    painter->eraseRect(0, 0, vpWidth, vpHeight);
    height = h;
	updateScrollbarLimits();
    updateView();
}

void Bittle::on_offset_changed(int o)
{
	if (o < 0) {
        QMessageBox::information(this, tr("Bittle"),
                                 tr("illegal offset value (%1).").arg(o));
        return;
		blockIndex = 0;
    }
    blockIndex = o;
    updateView();
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

		setControlDefaults();
		setDefaults();
		updateScrollbarLimits();
		updateView();
    }
}
