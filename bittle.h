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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMainWindow>

#define MAXWIDTH 512
#define MAXHEIGHT 512
#define PIX_PER_BYTE 8
#define DEFAULT_STRIDE 1
#define DEFAULT_HEIGHT 128

namespace Ui {
class Bittle;
}

class Bittle : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Bittle(QWidget *parent = 0);
    ~Bittle();
    void Bittle::resizeEvent(QResizeEvent *);
    
public slots:
    void on_width_changed(int w);
    void on_height_changed(int h);
    void on_offset_changed(int o);
    void on_lsb_changed(int state);

private slots:
    void on_actionOuvrir_triggered();

private:
    void updateScrollbarLimits();
    void updateView();
	void setDefaults();
	void setControlDefaults();
	void revcopy(uchar *dst, const uchar *src, size_t n);
	uint getOffset();
	uint blockSize();
	uint lineSize();

private:
    Ui::Bittle *ui;
    QFile *imageFile;
    uchar *imageData;
    QPixmap *pixmap;
    QPainter *painter;
    uint dataSize;
    uint stride;
    uint height;
    uint vpWidth;
    uint vpHeight;
    uint blockIndex;
    bool lsbFirst;
};

#endif // MAINWINDOW_H
