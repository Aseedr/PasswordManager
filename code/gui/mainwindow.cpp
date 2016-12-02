#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->textEdit->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    Client *client = new Client('l');
    if (client->initSokcets() == false) {
        return;
    }
    if (client->initOpenSokcet() == false) {
        return;
    }

    char *ip = "127.0.0.1";
    //cout << "Input ip address: ";
    //cin >> ip;
    //system("cls");
    client->connectToServer(ip);
    client->startClient();

    delete client;
    ui->textBrowser->append(client->getInfo());
}
