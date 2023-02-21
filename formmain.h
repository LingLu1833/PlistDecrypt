#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class FormMain;
}
QT_END_NAMESPACE

class FormMain : public QWidget
{
        Q_OBJECT

    public:
        FormMain(QWidget *parent = nullptr);
        ~FormMain();

        QString ByteToString(unsigned char byte);

        unsigned char StringToByte(QString pSrc);

    private slots:
        void on_pushButtonBrower_clicked();

        void on_pushButtonDecrypt_clicked();

        void on_pushButtonEncrypt_clicked();

    private:
        Ui::FormMain *ui;

        bool keyIsValid;

        QString selectPath;
};
#endif // FORMMAIN_H
