#include "formmain.h"
#include "ui_formmain.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

FormMain::FormMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormMain)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());

    keyIsValid = false;

    selectPath = ".";
}

FormMain::~FormMain()
{
    delete ui;
}

QString FormMain::ByteToString(unsigned char byte)
{
    QString tab = "0123456789ABCDEF";
    QString dst;

    dst.append('0');
    dst.append('x');
    dst.append(tab[byte >> 4]);
    dst.append(tab[byte & 0x0f]);

    return dst;
}

unsigned char FormMain::StringToByte(QString src)
{
    unsigned char highByte, lowByte, byte = 0;

    if (src.size() != 4 || src[0] != '0' || src[1] != 'x')
    {
        keyIsValid = false;
    }

    else
    {
        highByte = src[2].toUpper().toLatin1();
        lowByte = src[3].toUpper().toLatin1();

        if (highByte >= '0' && highByte <= '9')
        {
            highByte -= 0x30;
        }

        else
        {
            highByte -= 0x37;
        }

        if (lowByte >= '0' && lowByte <= '9')
        {
            lowByte -= 0x30;
        }

        else
        {
            lowByte -= 0x37;
        }

        byte = (highByte << 4) | lowByte;
        keyIsValid = true;
    }

    return byte;
}

void FormMain::on_pushButtonBrower_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", selectPath,  "plist文件(*.plist)");
    ui->lineEditFilePath->setText(fileName);
}

void FormMain::on_pushButtonDecrypt_clicked()
{
    if (ui->lineEditFilePath->text().isEmpty())
    {
        QMessageBox::information(this, "提示", "请先选择需要解密的plist文件！");
        return;
    }

    QFile file(ui->lineEditFilePath->text());

    QFileInfo fileInfo(file);
    selectPath = fileInfo.path();

    file.open(QIODevice::ReadOnly);
    const QByteArray fileByteDecrypt = file.readAll();
    file.close();

    if (fileByteDecrypt[0] == 0x12 && fileByteDecrypt[1] == 0x34 && fileByteDecrypt[2] == 0x56)
    {
        QFile fileDecrypt(ui->lineEditFilePath->text().split(".plist")[0] + "_Decrypt.plist");
        fileDecrypt.open(QIODevice::WriteOnly);

        QByteArray byteArrayDecrypt;

        for (int i = 4; i < fileByteDecrypt.size(); i++)
        {
            byteArrayDecrypt.append(fileByteDecrypt[i] ^ fileByteDecrypt[3]);
        }

        fileDecrypt.write(byteArrayDecrypt, byteArrayDecrypt.size());
        fileDecrypt.close();
        QMessageBox::information(this, "提示", "文件解密成功！");
        ui->lineEditKey->setText(ByteToString(fileByteDecrypt[3]));
    }

    else
    {
        QMessageBox::information(this, "提示", "当前选择文件未加密！");
    }
}

void FormMain::on_pushButtonEncrypt_clicked()
{
    if (ui->lineEditFilePath->text().isEmpty())
    {
        QMessageBox::information(this, "提示", "请先选择需要加密的plist文件！");
        return;
    }

    if (ui->lineEditKey->text().isEmpty())
    {
        QMessageBox::information(this, "提示", "请先输入加密的key值！");
        return;
    }

    unsigned char byteKey = StringToByte(ui->lineEditKey->text());

    if (!keyIsValid)
    {
        QMessageBox::information(this, "提示", "请按标准格式输入key值，如0x13.");
        return;
    }

    QFile file(ui->lineEditFilePath->text());

    QFileInfo fileInfo(file);
    selectPath = fileInfo.path();

    file.open(QIODevice::ReadOnly);
    const QByteArray fileByteEncrypt = file.readAll();
    file.close();

    if (fileByteEncrypt[0] == 0x3C)
    {
        QFile fileEncrypt(ui->lineEditFilePath->text().split(".plist")[0] + "_Encrypt.plist");
        fileEncrypt.open(QIODevice::WriteOnly);

        QByteArray byteArrayEncrypt;

        byteArrayEncrypt.append(0x12);
        byteArrayEncrypt.append(0x34);
        byteArrayEncrypt.append(0x56);
        byteArrayEncrypt.append(byteKey);

        for (int i = 0; i < fileByteEncrypt.size(); i++)
        {
            byteArrayEncrypt.append(fileByteEncrypt[i] ^ byteKey);
        }

        fileEncrypt.write(byteArrayEncrypt);
        fileEncrypt.close();
        QMessageBox::information(this, "提示", "文件加密成功！");
    }

    else
    {
        QMessageBox::information(this, "提示", "当前选择文件未解密！");
    }
}
