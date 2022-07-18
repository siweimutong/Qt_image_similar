#ifndef MATCHTEMPLATE_H
#define MATCHTEMPLATE_H

#include <QImage>
#include <QImageReader>
#include <QVector2D>
#include <QPixmap>
#include <QLabel>
#include <QtCore/qmath.h>
#include <iostream>
#include <QColor>
#include <complex>
#include <QByteArray>
#include <QMatrix>

#define MAX_DIFF 65535
#define MIN_DIFF 0
#define SQ_DIFF 0
#define SQ_DIFF_NOR 1
#define CCORR 2
#define CCORR_NOR 3
#define CCOEFF 4
#define CCOEFF_NOR 5

class MatchTemplate
{
public:
    MatchTemplate(QImage* srcImage, QImage* dstImage);
    ~MatchTemplate();

    QImage get_src_img();
    QImage get_dst_img();
    QImage get_result_img();
    QVector2D get_result_pos();
    bool match(int match_style);

private:
// member function
    double sq_diff();
    double sq_diff_normed();
    double ccorr();
    double ccorr_normed();
    double ccoeff();
    double ccoeff_normed();

    void match_with_sq_diff();
    void match_with_sq_diff_normed();
    void match_with_ccorr();
    void match_with_ccorr_normed();
    void match_with_ccoeff();
    void match_with_ccoeff_normed();
    QVector<QVector<double>> remove_avg(QVector<QVector<double>>* image_mat);
    QVector<QVector<double>> trans_matrix(QImage *image);
    QVector<unsigned long int> image_integral(QImage* image);
    void readImage(std::complex<int> data[], const QImage &Image);

// member var
    QImage* _src_img;
    QImage* _dst_img;
    QVector2D _result_pos;
    QVector<QVector<double>> _src_mat;
    QVector<QVector<double>> _dst_mat;

};

#endif // MATCHTEMPLATE_H
