#include <QWidget>
#include <QLabel>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX)
    #define NOMINMAX
#endif
#include <windows.h>

int __stdcall
WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    int argc = 1;
    char empty[] = "what";
    char *argv[] = { empty, nullptr };
    QApplication app { argc, argv };
    auto const mainwnd = new QWidget { };
    auto const text = new QLabel { "GAMERS BILL OF RIGHTS" };
    auto const layout = new QVBoxLayout { };
    layout->addWidget(text);
    mainwnd->setLayout(layout);

    auto const btngroup = new QDialogButtonBox { };
    auto const ok = new QPushButton { "ok" };
    auto const no = new QPushButton { "no" };
    btngroup->addButton(ok, QDialogButtonBox::AcceptRole);
    btngroup->addButton(no, QDialogButtonBox::RejectRole);

    layout->addWidget(btngroup);
    mainwnd->show();
    app.exec();
    return 0;
}

#include "../stubs/c++-stubs.cpp"