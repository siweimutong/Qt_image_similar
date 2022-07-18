#include "matchtemplate.h"


MatchTemplate::MatchTemplate(QImage* srcImage, QImage* dstImage)
    :_src_img(srcImage),_dst_img(dstImage)
{
    QVector<double> vec(srcImage->width());
    for(int i = 0; i < srcImage->height(); i++)
    {
        _src_mat.append(vec);
        _dst_mat.append(vec);
    }
}

MatchTemplate::~MatchTemplate()
{

}

bool MatchTemplate::match(int match_style)
{
    switch (match_style) {
    case 0:
        this->match_with_sq_diff();
        break;
    case 1:
        this->match_with_sq_diff_normed();
        break;
    case 2:
        this->match_with_ccorr();
        break;
    case 3:
        this->match_with_ccorr_normed();
        break;
    case 4:
        this->match_with_ccoeff();
        break;
    case 5:
        this->match_with_ccoeff_normed();
        break;
    default:
        return false;
    }
    return true;
}

QImage MatchTemplate::get_dst_img()
{
    return *this->_dst_img;
}

QImage MatchTemplate::get_src_img()
{
    return *this->_src_img;
}

QImage MatchTemplate::get_result_img()
{
    QImage res = this->_dst_img->copy(0,0,this->_dst_img->width(),this->_dst_img->height());
    for(int i = 0; i < this->_src_img->width(); i++)
    {
        for(int j = 0; j < this->_src_img->height(); j++)
        {
            res.setPixel(this->_result_pos.x() + i, this->_result_pos.y() + j, 0xFF000000);
        }
    }
    return res;
}

QVector2D MatchTemplate::get_result_pos()
{
    return this->_result_pos;
}

void MatchTemplate::match_with_sq_diff()
{
    double min_diff = MAX_DIFF;
    for(int i = 0; i < this->_dst_img->height() - this->_src_img->width(); i++)
    {
        for(int j = 0; j < this->_dst_img->width() - this->_src_img->height(); j++)
        {
            QImage dst_pimg = this->_dst_img->copy(i,j,this->_src_img->width(),this->_src_img->height());
            QImage dst_grey = dst_pimg.convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            QImage src_grey = this->_src_img->convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            this->_src_mat = trans_matrix(&src_grey);
            this->_dst_mat = trans_matrix(&dst_grey);
            double diff_val = sq_diff();
            if(diff_val < min_diff)
            {
                min_diff = diff_val;
                this->_result_pos.setX(i);
                this->_result_pos.setY(j);
                std::cout << diff_val << std::endl;
            }
        }
    }
}

double MatchTemplate::sq_diff()
{
    double sum = 0.0;
    for(int i = 0 ; i < this->_src_mat.size(); i++)
    {
        for(int j = 0; j < this->_src_mat.at(0).size(); j++)
        {
            sum = qPow((this->_src_mat.at(i).at(j) - this->_dst_mat.at(i).at(j)),2) + sum;
        }
    }
    return sum;
}

void MatchTemplate::match_with_sq_diff_normed()
{
    double min_diff = MAX_DIFF;
    for(int i = 0; i < this->_dst_img->height() - this->_src_img->width(); i++)
    {
        for(int j = 0; j < this->_dst_img->width() - this->_src_img->height(); j++)
        {
            QImage dst_pimg = this->_dst_img->copy(i,j, this->_src_img->width(), this->_src_img->height());
            QImage dst_grey = dst_pimg.convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            QImage src_grey = this->_src_img->convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            this->_src_mat = trans_matrix(&src_grey);
            this->_dst_mat = trans_matrix(&dst_grey);
            double diff_val = sq_diff_normed();
            if(diff_val < min_diff)
            {
                min_diff = diff_val;
                this->_result_pos.setX(i);
                this->_result_pos.setY(j);
                std::cout << diff_val << std::endl;
            }
        }
    }
}

double MatchTemplate::sq_diff_normed()
{
    double sum = 0.0;
    double sum_normed_src = 0.0;
    double sum_normed_dst = 0.0;
    for(int i = 0 ; i < this->_src_mat.size(); i++)
    {
        for(int j = 0; j < this->_src_mat.at(0).size(); j++)
        {
            sum = qPow((this->_src_mat.at(i).at(j) - this->_dst_mat.at(i).at(j)),2) + sum;
            sum_normed_src = qPow(this->_src_mat.at(i).at(j),2) + sum_normed_src;
            sum_normed_dst = qPow(this->_dst_mat.at(i).at(j),2) + sum_normed_dst;
        }
    }
    sum = sum / (qSqrt(sum_normed_src) * qSqrt(sum_normed_dst));
    return sum;
}

void MatchTemplate::match_with_ccorr()
{
    double max_diff = MIN_DIFF;
    for(int i = 0; i < this->_dst_img->height() - this->_src_img->width(); i++)
    {
        for(int j = 0; j < this->_dst_img->width() - this->_src_img->height(); j++)
        {
            QImage dst_pimg = this->_dst_img->copy(i,j,this->_src_img->width(),this->_src_img->height());
            QImage dst_grey = dst_pimg.convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            QImage src_grey = this->_src_img->convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            this->_src_mat = trans_matrix(&src_grey);
            this->_dst_mat = trans_matrix(&dst_grey);
            double diff_val = ccorr();
            if(diff_val > max_diff)
            {
                max_diff = diff_val;
                this->_result_pos.setX(i);
                this->_result_pos.setY(j);
                std::cout << diff_val << std::endl;
            }
        }
    }
}

double MatchTemplate::ccorr()
{
    double sum = 0.0;
    for(int i = 0 ; i < this->_src_mat.size(); i++)
    {
        for(int j = 0; j < this->_src_mat.at(0).size(); j++)
        {
            sum = qPow((this->_src_mat.at(i).at(j) * this->_dst_mat.at(i).at(j)),2) + sum;
        }
    }
    return sum;
}

void MatchTemplate::match_with_ccorr_normed()
{
    double max_diff = MIN_DIFF;
    for(int i = 0; i < this->_dst_img->height() - this->_src_img->width(); i++)
    {
        for(int j = 0; j < this->_dst_img->width() - this->_src_img->height(); j++)
        {
            QImage dst_pimg = this->_dst_img->copy(i,j,this->_src_img->width(),this->_src_img->height());
            QImage dst_grey = dst_pimg.convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            QImage src_grey = this->_src_img->convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            this->_src_mat = trans_matrix(&src_grey);
            this->_dst_mat = trans_matrix(&dst_grey);
            double diff_val = ccorr_normed();
            if(diff_val > max_diff)
            {
                max_diff = diff_val;
                this->_result_pos.setX(i);
                this->_result_pos.setY(j);
                std::cout << diff_val << std::endl;
            }
        }
    }
}

double MatchTemplate::ccorr_normed()
{
    double sum = 0.0;
    double sum_normed_src = 0.0;
    double sum_normed_dst = 0.0;
    for(int i = 0 ; i < this->_src_mat.size(); i++)
    {
        for(int j = 0; j < this->_src_mat.at(0).size(); j++)
        {
            sum = qPow((this->_src_mat.at(i).at(j) - this->_dst_mat.at(i).at(j)),2) + sum;
            sum_normed_src = qPow(this->_src_mat.at(i).at(j),2) + sum_normed_src;
            sum_normed_dst = qPow(this->_dst_mat.at(i).at(j),2) + sum_normed_dst;
        }
    }
    sum = sum / (qSqrt(sum_normed_src) * qSqrt(sum_normed_dst));
    return sum;
}


void MatchTemplate::match_with_ccoeff()
{
    double max_diff = MIN_DIFF;
    for(int i = 0; i < this->_dst_img->height() - this->_src_img->width(); i++)
    {
        for(int j = 0; j < this->_dst_img->width() - this->_src_img->height(); j++)
        {
            QImage dst_pimg = this->_dst_img->copy(i,j,this->_src_img->width(),this->_src_img->height());
            QImage dst_grey = dst_pimg.convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            QImage src_grey = this->_src_img->convertToFormat(QImage::Format_Grayscale8, Qt::AutoColor);
            this->_src_mat = trans_matrix(&src_grey);
            this->_dst_mat = trans_matrix(&dst_grey);
            double diff_val = ccoeff();
            if(diff_val > max_diff)
            {
                max_diff = diff_val;
                this->_result_pos.setX(i);
                this->_result_pos.setY(j);
                std::cout << diff_val << std::endl;
            }
        }
    }
}

double MatchTemplate::ccoeff()
{
    for(int i = 0; i < this->_dst_img->height() - this->_src_img->width(); i++)
    {
        for(int j = 0; j < this->_dst_img->width() - this->_src_img->height(); j++)
        {
            //avg_src =
        }
    }
    return 0.0;
}

void MatchTemplate::match_with_ccoeff_normed()
{

}

QVector<QVector<double>> MatchTemplate::remove_avg(QVector<QVector<double>> *image_mat)
{
    double sum = 0.0;
    for(int i = 0; i < image_mat->size(); i++)
    {
        for(int j = 0; j < image_mat->at(0).size(); j++)
        {
            sum += image_mat->at(i).at(j);
        }
    }
    sum = sum / (image_mat->size() * image_mat->at(0).size());

    QVector<QVector<double>> res_mat;
    for(int i = 0; i < image_mat->size(); i++)
    {
        QVector<double> tmp_mat;
        for(int j = 0; j < image_mat->at(0).size(); j++)
        {
            tmp_mat.append(image_mat->at(i).at(j) - sum);
        }
        res_mat.append(tmp_mat);
    }
    return res_mat;
}

QVector<QVector<double>> MatchTemplate::trans_matrix(QImage *image)
{
    QVector<QVector<double>> res_mat;
    for(int i = 0 ; i < image->width() ; i++)
    {
        QVector<double> tmp_mat;
        for(int j = 0 ; j < image->height() ; j++)
        {
            QColor pos_color = image->pixel(i,j);
            tmp_mat.append((pos_color.red() / 256.0));
        }
        res_mat.append(tmp_mat);
    }
    res_mat = remove_avg(&res_mat);
    return res_mat;
}

QVector<unsigned long int> MatchTemplate::image_integral(QImage *image)
{
    QVector<unsigned long int> rgb;

    unsigned long int image_val_r = 0;
    unsigned long int image_val_g = 0;
    unsigned long int image_val_b = 0;

    for(int i = 0; i < image->height(); i++)
    {
        for(int j = 0; j < image->width(); j++)
        {
            QColor color =image->pixel(i,j);
            image_val_r = image_val_r + color.red();
            image_val_g = image_val_g + color.green();
            image_val_b = image_val_b + color.blue();
        }
    }
    rgb.append(image_val_r);
    rgb.append(image_val_g);
    rgb.append(image_val_b);
    return rgb;
}


void MatchTemplate::readImage(std::complex<int> *data, const QImage &image)
{
    const uchar* pImageBytes(image.bits());
    const uchar* pBytes;
    int depth = image.depth();
    int lineBytes = image.bytesPerLine();
    int w = image.width();
    int h = image.height();
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            if(depth == 8)
            {
                pBytes = pImageBytes + i*lineBytes + j;
                data[i * w + j] = std::complex<const uchar>(*pBytes, 0);
            }
            if(depth == 32)
            {
                const uchar pixelValue = (uchar)(0.299*(float)pBytes[0] + 0.587 * (float)pBytes[1] + 0.114*(float)pBytes[2]);
                data[i * w + j] = std::complex<const uchar>(pixelValue, 0);
            }
        }
    }

}

