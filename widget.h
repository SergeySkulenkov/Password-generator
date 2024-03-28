#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void generateSlot();
    void clearSlot();
    void saveSettingsSlot();
    void passowrdsChangeStatusSlot();
    void usersChangeStatusSlot();
    void lettersChangeStatusSlot();
    void didgitalChangeStatusSlot();
    void otherSymbolsChangeStatusSlot();
    void fileChangeStatusSlot();
    void countPasswordsChangeSlot(int arg);
    void openFileDialogSlot();



private:
    Ui::Widget *ui;

    QString latinLower      = "abcdefghijklmnopqrstuvwxyz";
    QString latinUpper      = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString ruLower         = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    QString ruUpper         = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    QString didgits         = "0123456789";
    QString otherSymbols    = "@$%_-.~!";
    QString filePath;
    QString alphabet;

    const int maxUsersPass  = 10000;
    QRandomGenerator* generator;

    void setFonts();
    void setDefaultValues();
    void validGenerate();
    void showWarning(const QString title, const QString text);

    QStringList getPasswords();
    QString     getPassword();
    QStringList getUsers();

    void loadQSSFile();
    void readSettings();


};
#endif // WIDGET_H
