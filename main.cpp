#include "mainwindow.h"
#include "matchtemplate.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();
    QLabel *lb = new QLabel("",0);
    QImage im;
    QImageReader src_reader("D:\\QTcode\\SimilarImg\\src.png");
    QImageReader dst_reader("D:\\QTcode\\SimilarImg\\dst.png");
    src_reader.setAutoTransform(true);
    dst_reader.setAutoTransform(true);
    QImage srcImage = src_reader.read();
    QImage dstImage = dst_reader.read();
    MatchTemplate match_process(&srcImage, &dstImage);
    bool match_succ;
    match_succ = match_process.match(SQ_DIFF_NOR);
    if(match_succ)
    {
        im = match_process.get_result_img();
        QPixmap pix;
        pix.convertFromImage(im);
        lb->setPixmap(pix);
        lb->setGeometry(pix.rect());
        lb->show();
    }
    //image_viewer->SetImage(newImage);
    return a.exec();
}
