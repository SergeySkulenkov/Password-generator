#include "widget.h"
#include "ui_widget.h"
#include <QFont>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QSettings>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setFonts();
    setDefaultValues();
    loadQSSFile();
    connect(ui->generateButton,       &QPushButton::clicked,    this, &Widget::generateSlot);
    connect(ui->clearButton,          &QPushButton::clicked,    this, &Widget::clearSlot);
    connect(ui->saveSettingsButton,   &QPushButton::clicked,    this, &Widget::saveSettingsSlot);
    connect(ui->passwordCheckBox,     &QCheckBox::stateChanged, this, &Widget::passowrdsChangeStatusSlot);
    connect(ui->usersCheckBox,        &QCheckBox::stateChanged, this, &Widget::usersChangeStatusSlot);
    connect(ui->fileCheckBox,         &QCheckBox::stateChanged, this, &Widget::fileChangeStatusSlot);
    connect(ui->latinCheckBox,        &QCheckBox::stateChanged, this, &Widget::lettersChangeStatusSlot);
    connect(ui->rusCheckBox,          &QCheckBox::stateChanged, this, &Widget::lettersChangeStatusSlot);
    connect(ui->didgitsCheckBox,      &QCheckBox::stateChanged, this, &Widget::didgitalChangeStatusSlot);
    connect(ui->otherSymbolsCheckBox, &QCheckBox::stateChanged, this, &Widget::otherSymbolsChangeStatusSlot);
    connect(ui->selectFileButton,     &QPushButton::clicked,    this, &Widget::openFileDialogSlot);
    connect(ui->countPasswordsSpin,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this,
            &Widget::countPasswordsChangeSlot);
    readSettings();
    generator = QRandomGenerator::global();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::generateSlot()
{

    QStringList usersList;
    QStringList passwordsList;
    if(ui->passwordCheckBox->isChecked() && !ui->usersCheckBox->isChecked()){
        //Passwords only
        passwordsList = getPasswords();
        if(passwordsList.isEmpty()){
            return;
        }
        for(QString& pas: passwordsList){
            ui->resultEdit->appendPlainText(pas);
        }
    }else if(!ui->passwordCheckBox->isChecked() && ui->usersCheckBox->isChecked()){
        //Users only
        usersList = getUsers();
        if(usersList.isEmpty()){
            return;
        }
        for(QString& user: usersList){
            ui->resultEdit->appendPlainText(user);
        }
    }else if(ui->passwordCheckBox->isChecked() && ui->usersCheckBox->isChecked()){
        //Users and passwords
        passwordsList = getPasswords();
        usersList = getUsers();
        if(usersList.isEmpty() || passwordsList.isEmpty()){
            return;
        }
        int countValues = passwordsList.size();
        for(int i=0; i<countValues; i++){
            ui->resultEdit->appendPlainText(usersList.at(i) + " " + passwordsList.at(i));
        }
    }
    if(ui->fileCheckBox->isChecked()){
        if(ui->filePathEdit->text().trimmed().length()==0){
            showWarning("Ошибка", "Не указан путь для сохранения в файл!");
            return;
        }
        QFile file(ui->filePathEdit->text().trimmed());
        if(!file.open(QIODevice::WriteOnly)){
            showWarning("Ошибка","Файл недоступен для записи!");
            return;
        }
        QChar delimiter = (ui->comboBox->currentIndex()==0)? ' ': ';';
        qDebug() << ui->comboBox->currentIndex();
        if(passwordsList.size()>0 && usersList.size()>0){
            int countValues = passwordsList.size();
            for(int i=0; i<countValues; i++){
                QString str = usersList.at(i) + delimiter + passwordsList.at(i) + "\n";
                file.write(str.toUtf8());
            }
        }else if(passwordsList.size()>0){
            for(QString& pass: passwordsList){
               file.write(pass.toUtf8()+"\n");
            }
        }else if(usersList.size()>0){
            for(QString& user: usersList){
               file.write(user.toUtf8()+"\n");
            }
        }
        file.close();
    }
}

QStringList Widget::getPasswords()
{
    QStringList list;
    int comboIndex = ui->registrComboBox->currentIndex();
    if(ui->latinCheckBox->isChecked()){
        if(comboIndex==0){
            alphabet += latinLower + latinUpper;
        }else if(comboIndex==1){
            alphabet += latinLower;
        }else{
            alphabet += latinUpper;
        }
    }
    if(ui->rusCheckBox->isChecked()){
        if(comboIndex==0){
            alphabet += ruLower + ruUpper;
        }else if(comboIndex==1){
            alphabet += ruLower;
        }else{
            alphabet += ruUpper;
        }
    }
    if(ui->didgitsCheckBox->isChecked()){
        if(ui->didgitsEdit->text().trimmed().isEmpty()){
            showWarning("Ошибка", "Поле Цифры пустое!");
            return list;
        }
        alphabet += didgits;
    }
    if(ui->otherSymbolsCheckBox->isChecked()){
        if(ui->otherSymbolsEdit->text().trimmed().isEmpty()){
           showWarning("Ошибка", "Поле Другие символы пустое!");
            return list;
        }
        alphabet += otherSymbols;
    }
    int countPasswords = ui->countPasswordsSpin->value();
    for(int i=0; i<countPasswords; i++){
        list.append(getPassword());
    }
    return list;
}

QStringList Widget::getUsers()
{
    QStringList list;
    int stop;
    if(ui->passwordCheckBox->isChecked())
        stop = ui->countPasswordsSpin->value() + ui->startValueSpin->value();
    else
        stop = ui->countUsersSpin->value() + ui->startValueSpin->value();

    int step = ui->stepValueSipn->value();
    QString prefix = ui->prefixEdit->text().trimmed();

    for(int i=ui->stepValueSipn->value(); i<stop; i += step){
        list.append(prefix + QString::number(i));
    }
    return list;
}

void Widget::loadQSSFile()
{
    QFile qssFile(":/qss/style.qss");
    qssFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QString style = qssFile.readAll();
    this->setStyleSheet(style);
    qssFile.close();
}

void Widget::readSettings()
{
    QSettings settings;
    if(!settings.contains("main/passwords"))
        return;
    ui->passwordCheckBox->setChecked(settings.value("main/passwords").toBool());
    ui->usersCheckBox->setChecked(settings.value("main/users").toBool());
    ui->fileCheckBox->setChecked(settings.value("main/file").toBool());
}
void Widget::showWarning(const QString title, const QString text)
{
    QMessageBox msg(this);
    msg.setWindowTitle(title);
    msg.setText(text);
    msg.setIcon(QMessageBox::Warning);
    msg.addButton("Понятно", QMessageBox::AcceptRole);
    msg.exec();
}

QString Widget::getPassword()
{
    int countLetters = ui->countLettersSpin->value();
    QString str;
    int abMax = alphabet.length()-1;
    for(int i=0;i<countLetters; i++){
        int index = generator->bounded(0,abMax);
        str += alphabet.at(index);
    }

    return str;
}

void Widget::clearSlot(){
    ui->resultEdit->clear();
}

void Widget::saveSettingsSlot(){
    QSettings settings;
    settings.setValue("main/passwords", ui->passwordCheckBox->isChecked());
    settings.setValue("main/users",     ui->usersCheckBox->isChecked());
    settings.setValue("main/file",      ui->fileCheckBox->isChecked());
    showWarning("Сохранение настроек","Настройки сохранены!");
}

void Widget::passowrdsChangeStatusSlot()
{
    if(ui->passwordCheckBox->isChecked()){
        ui->passwordsBox->setEnabled(true);
        if(ui->usersCheckBox->isChecked()){
            ui->countUsersSpin->setValue(ui->countPasswordsSpin->value());
            ui->countUsersSpin->setEnabled(false);
        }else{
            ui->countUsersSpin->setEnabled(true);
        }
    }else{
        ui->passwordsBox->setEnabled(false);
         ui->countUsersSpin->setEnabled(true);
    }
    validGenerate();
}

void Widget::usersChangeStatusSlot()
{
    if(ui->usersCheckBox->isChecked()){
        ui->usersBox->setEnabled(true);
        if(ui->passwordCheckBox->isChecked()){
            ui->countUsersSpin->setValue(ui->countPasswordsSpin->value());
            ui->countUsersSpin->setEnabled(false);
        }else{
            ui->countUsersSpin->setEnabled(true);
        }
    }else{
        ui->usersBox->setEnabled(false);
        ui->countUsersSpin->setEnabled(true);

    }
    validGenerate();
}

void Widget::lettersChangeStatusSlot()
{
    if(!ui->latinCheckBox->isChecked() && !ui->rusCheckBox->isChecked()){
        ui->registrComboBox->setEnabled(false);
    }else{
        ui->registrComboBox->setEnabled(true);
    }
    validGenerate();
}

void Widget::didgitalChangeStatusSlot()
{
    if(ui->didgitsCheckBox->isChecked()){
        ui->didgitsEdit->setEnabled(true);
    }else{
        ui->didgitsEdit->setEnabled(false);
    }
    validGenerate();
}

void Widget::otherSymbolsChangeStatusSlot()
{
    if(ui->otherSymbolsCheckBox->isChecked()){
        ui->otherSymbolsEdit->setEnabled(true);
    }else{
        ui->otherSymbolsEdit->setEnabled(false);
    }
    validGenerate();
}

void Widget::fileChangeStatusSlot()
{
    if(ui->fileCheckBox->isChecked())
        ui->fileBox->setEnabled(true);
    else
        ui->fileBox->setEnabled(false);
    validGenerate();
}

void Widget::countPasswordsChangeSlot(int arg)
{
    if(ui->usersCheckBox->isChecked())
        ui->countUsersSpin->setValue(ui->countPasswordsSpin->value());
}


void Widget::openFileDialogSlot()
{
    filePath = QFileDialog::getSaveFileName(this, "Сохранение");
    if(filePath.isEmpty())
        return;
    ui->filePathEdit->setText(filePath);
}

void Widget::setFonts(){
    QFont font = this->font();
    font.setPointSize(10);
    this->setFont(font);
    QFont editorFont("Consolas", 12);
    ui->resultEdit->setFont(editorFont);
}

void Widget::setDefaultValues()
{
    //mainBox
    ui->usersBox->setChecked(false);
    ui->passwordCheckBox->setChecked(true);
    ui->fileCheckBox->setChecked(false);

    //passwordsBox
    ui->passwordsBox->setEnabled(true);
    //passwords count
    ui->countPasswordsSpin->setMinimum(1);
    ui->countPasswordsSpin->setMaximum(maxUsersPass);
    ui->countPasswordsSpin->setValue(1);

    //count password letters
    ui->countLettersSpin->setMinimum(4);
    ui->countLettersSpin->setMaximum(100);
    ui->countLettersSpin->setValue(8);

    //all letters
    ui->latinCheckBox->setChecked(true);
    ui->rusCheckBox->setChecked(false);
    ui->registrComboBox->setCurrentIndex(0);
    ui->didgitsCheckBox->setChecked(true);
    ui->didgitsEdit->setText(didgits);
    ui->didgitsEdit->setEnabled(true);
    ui->otherSymbolsCheckBox->setChecked(true);
    ui->otherSymbolsEdit->setText(otherSymbols);
    ui->otherSymbolsEdit->setEnabled(true);

    //usersBox
    ui->usersBox->setEnabled(false);

    //count users
    ui->countUsersSpin->setMinimum(1);
    ui->countUsersSpin->setMaximum(maxUsersPass);

    //prefix
    ui->prefixEdit->setText("user_");

    //begin counter value
    ui->startValueSpin->setMinimum(1);
    ui->startValueSpin->setMaximum(maxUsersPass);
    ui->startValueSpin->setValue(1);

    //counter step
    ui->stepValueSipn->setMinimum(1);
    ui->stepValueSipn->setMaximum(100);
    ui->stepValueSipn->setValue(1);

    //fileBox
    ui->fileBox->setEnabled(false);
}

void Widget::validGenerate()
{
    if(!ui->passwordCheckBox->isChecked() && !ui->usersCheckBox->isChecked()){
        ui->generateButton->setEnabled(false);
    }else{
        ui->generateButton->setEnabled(true);
    }
    if(ui->passwordCheckBox->isChecked()
            && !ui->latinCheckBox->isChecked()
            && !ui->rusCheckBox->isChecked()
            && !ui->didgitsCheckBox->isChecked()
            && !ui->otherSymbolsCheckBox->isChecked()){
        ui->generateButton->setEnabled(false);
    }
}

