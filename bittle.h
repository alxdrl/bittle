#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMainWindow>

#define MAXWIDTH 512
#define MAXHEIGHT 512

namespace Ui {
class Bittle;
}

class Bittle : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Bittle(QWidget *parent = 0);
    ~Bittle();
    
public slots:
    void on_width_changed(int w);
    void on_height_changed(int h);
    void on_offset_changed(int o);
    void on_lsb_changed(int state);

private slots:
    void on_actionOuvrir_triggered();

private:
        void on_update();

private:
    Ui::Bittle *ui;
    QFile *imageFile;
    uchar *imageData;
    QLabel *imageLabel;
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
